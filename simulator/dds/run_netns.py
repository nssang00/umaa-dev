"""Run real Cyclone DDS binaries across distinct Linux network namespaces.

Three namespace topology is provisioned by tools/dds_netns.sh.  Publisher and
simulator never share loopback; unicast DDS discovery is pinned to veth eth0.
"""
import argparse
import os
import signal
import subprocess
import sys
import time
import xml.etree.ElementTree as ET

NODES = {
    'umaa_ci_pub': '10.233.77.11',
    'umaa_ci_sim1': '10.233.77.12',
    'umaa_ci_sim2': '10.233.77.13',
}


def cyclone_config(node):
    if node not in NODES:
        raise ValueError('Unknown DDS node: ' + node)
    peers = ''.join('<Peer Address="' + address + '"/>'
                    for name, address in NODES.items() if name != node)
    xml = ('<CycloneDDS xmlns="https://cdds.io/config"><Domain Id="any">'
           '<General><Interfaces><NetworkInterface name="eth0"/></Interfaces>'
           '<AllowMulticast>false</AllowMulticast></General>'
           '<Discovery><Peers AddLocalhost="false">' + peers + '</Peers></Discovery>'
           '</Domain></CycloneDDS>')
    ET.fromstring(xml)
    return xml


def command_for_node(node, executable, *arguments):
    if node not in NODES:
        raise ValueError('Unknown DDS node: ' + node)
    return ['ip', 'netns', 'exec', node, executable, *arguments]


def confirm_isolation(nodes):
    seen = set()
    for node in nodes:
        r = subprocess.run(command_for_node(node, 'readlink', '/proc/self/ns/net'),
                           text=True, capture_output=True, timeout=5, check=True)
        ident = r.stdout.strip()
        if not ident.startswith('net:[') or ident in seen:
            raise RuntimeError('Network namespace not isolated: ' + node)
        seen.add(ident)
        r = subprocess.run(command_for_node(node, 'ip', '-4', 'addr', 'show', 'dev', 'eth0'),
                           text=True, capture_output=True, timeout=5, check=True)
        if NODES[node] not in r.stdout:
            raise RuntimeError('Missing veth IP: ' + node)
    print('DDS_NETNS_ISOLATION_PASS: ' + ', '.join(nodes), flush=True)


def spawn(node, executable, *arguments, readers=1):
    env = dict(os.environ)
    env['CYCLONEDDS_URI'] = cyclone_config(node)
    env['UMAA_EXPECT_READERS'] = str(readers)
    return subprocess.Popen(command_for_node(node, executable, *arguments),
                            env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, start_new_session=True)


def stop(proc):
    if proc.poll() is None:
        os.killpg(proc.pid, signal.SIGKILL)
    proc.communicate()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('host')
    parser.add_argument('simulator')
    parser.add_argument('module')
    parser.add_argument('--subscribers', type=int, choices=(1, 2), default=1)
    parser.add_argument('--update', action='store_true')
    args = parser.parse_args()
    if args.update and args.subscribers != 1:
        parser.error('update requires one EngineControl simulator')
    sims = []
    host = None
    sim_nodes = ['umaa_ci_sim1'] + (['umaa_ci_sim2'] if args.subscribers == 2 else [])
    confirm_isolation(['umaa_ci_pub', *sim_nodes])
    try:
        for node in sim_nodes:
            sims.append(spawn(node, args.simulator, *(['--update'] if args.update else [])))
        time.sleep(0.8)
        host = spawn('umaa_ci_pub', args.host, args.module, readers=args.subscribers)
        out, _ = host.communicate(timeout=30)
        print('HOST OUTPUT:\n' + out, flush=True)
        if host.returncode != 0:
            raise RuntimeError('host returned ' + str(host.returncode))
        for node, sim in zip(sim_nodes, sims):
            out, _ = sim.communicate(timeout=25)
            print(node + ' OUTPUT:\n' + out, flush=True)
            if sim.returncode != 0 or not any('PASS' in line and 'SIM:' in line
                                               for line in out.splitlines()):
                raise RuntimeError(node + ': simulator failed, rc=' + str(sim.returncode))
        print('DDS_NETNS_WIRE_PASS: isolated nodes=' + str(1 + len(sims)), flush=True)
        return 0
    except (subprocess.TimeoutExpired, RuntimeError, subprocess.CalledProcessError) as exc:
        print('DDS_NETNS_WIRE_FAIL: ' + str(exc), file=sys.stderr, flush=True)
        return 1
    finally:
        for proc in ([host] if host else []) + sims:
            stop(proc)


if __name__ == '__main__':
    sys.exit(main())
