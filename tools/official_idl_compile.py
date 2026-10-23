#!/usr/bin/env python3
"""Build only the official UMAA IDL transitive TypeSupport closure.

External AUVSI input must be supplied by the user/CI. Never copies or uploads
official source IDLs into the repository. idlc is the authoritative IDL parser.
"""
import argparse
import pathlib
import re
import subprocess

INCLUDE = re.compile(r'^\s*#\s*include\s*["<]([^">]+)[">]', re.MULTILINE)


def collect(root: pathlib.Path, target: pathlib.Path) -> list[pathlib.Path]:
    visiting: set[pathlib.Path] = set()
    visited: set[pathlib.Path] = set()
    result = []

    def walk(path: pathlib.Path):
        path = path.resolve()
        if path in visited:
            return
        if path in visiting:
            raise ValueError(f'IDL include cycle: {path}')
        if root.resolve() not in path.parents:
            raise ValueError(f'IDL include outside root: {path}')
        if not path.is_file():
            raise FileNotFoundError(path)
        visiting.add(path)
        for name in INCLUDE.findall(path.read_text(encoding='utf-8-sig', errors='replace')):
            candidate = root / name
            if not candidate.is_file():
                candidate = path.parent / name
            if not candidate.is_file():
                raise FileNotFoundError(f'Missing {name} imported by {path}')
            walk(candidate)
        visiting.remove(path)
        visited.add(path)
        result.append(path)

    walk(target)
    return result


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', type=pathlib.Path, required=True)
    ap.add_argument('--source', default='UMAA/EO/BatteryStatus/BatteryReportType.idl')
    ap.add_argument('--out', type=pathlib.Path, required=True)
    ap.add_argument('--cc', default='cc')
    args = ap.parse_args()
    root = args.root.resolve()
    out = args.out.resolve()
    closure = collect(root, root / args.source)
    for item in closure:
        parent = out / item.relative_to(root).parent
        parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(['idlc', '-I', str(root), '-o', str(parent), str(item)], check=True)
    target = out / pathlib.Path(args.source).with_suffix('.c')
    assert target.is_file(), str(target)
    subprocess.run([args.cc, '-std=c11', '-fPIC', '-I', str(out),
                    '-I', str(root), '-c', str(target),
                    '-o', str(out / 'official-battery.o')], check=True)
    header = out / pathlib.Path(args.source).with_suffix('.h')
    lines = header.read_text().splitlines()
    flag = False
    for line in lines:
        if line.strip().startswith('typedef struct UMAA_EO_BatteryStatus_BatteryReportType'):
            flag = True
        elif flag and line.strip().startswith('}'):
            flag = False
        elif flag:
            print('OFFICIAL_BATTERY_MEMBER', line.strip()[:140])
    for p in closure:
        print('OFFICIAL_IDL_DEP',p.relative_to(root).as_posix())
    for h in sorted(out.rglob('*.h')):
        txt = h.read_text()
        for symbol in ('UMAA_Common_Measurement_EnergyPercent', 'UMAA_Common_IdentifierType'):
            pos = txt.find('typedef struct ' + symbol)
            if pos == -1:
                continue
            part = txt[pos:pos+900].split('}',1)[0]
            for line in part.splitlines()[1:15]:
                if line.strip():
                    print('OFFICIAL_NESTED_MEMBER',symbol,line.strip()[:140])
    print(f'OFFICIAL_BATTERY_TYPESUPPORT_COMPILED files={len(closure)}')


if __name__ == '__main__':
    main()
