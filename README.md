# UMAA SDK and independent simulator research workspace

This repository hosts the **cross-platform native service plugin smoke implementation** under `smoke/`. It is a **prototype**, not an official UMAA 6.0 or K-MOSA certified implementation.

## Verified on GitHub-hosted runners
- Ubuntu 24.04 / GCC
- Ubuntu 24.04 / Clang
- Windows Server 2022 / MSVC

[Actions results](https://github.com/nssang00/umaa-dev/actions) run CMake + CTest against native .so/.dll modules. The smoke tests exercise module loading, ABI checks, start/invoke/stop/destroy and reload.

## Scope and gaps
The **full local SDK and independent simulator** are separate projects from earlier prototype artifacts and have not yet been uploaded to this repository. They include synthetic subset-IDL generation, C++ command/report/collection state code and independent stdio scenario verification. Nothing in this GitHub smoke test exercises official UMAA-generated DDS types, Cyclone DDS, RTI Connext, or on-wire interoperability. See `STATUS.md` and `docs/DELIVERY_GATES.md`.

## K-MOSA-aligned proposal
Each UMAA logical service is designed as a separately deployable native module (Windows DLL or Linux SO); the host retains command protocol, DDS, QoS and session ownership. `smoke/include/umaa/module_api.h` is an **internal prototype ABI**, not an official K-MOSA API. Hot swap during active sessions is not yet implemented.

## Local test
```bash
cmake -S smoke -B build/native
cmake --build build/native --config Debug
ctest --test-dir build/native -C Debug --output-on-failure
```
