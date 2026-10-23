# UMAA SDK + Independent Simulator

Two **separate projects** for development and independent verification of UMAA services:

- [sdk/](sdk/): synthetic-subset IDL generator, typed C++ Handler examples, Command/Report/Collection prototype, versioned native service module loader (DLL/SO), installable CMake package and tests.
- [simulator/](simulator/): independent scenario runner and protocol trace verifier. The working test fixture exchanges JSON events over stdio; it does not use DDS.

## Cross-platform GitHub CI — executed

| GitHub-hosted runner | Result |
| --- | --- |
| Ubuntu 24.04 / GCC | PASS |
| Ubuntu 24.04 / Clang | PASS |
| Windows Server 2022 / MSVC | PASS |

**[Full SDK + Simulator Actions run](https://github.com/nssang00/umaa-dev/actions/runs/39941190249)**: all three matrix jobs passed, verified from GitHub-hosted job logs. Each job performed 5 Python codegen unit tests, 6 independent simulator oracle unit tests, a CMake build, CTest **4/4**, CMake install and external consumer build, and four cross-process simulator scenarios (**32 observed events total**). A separate earlier smoke workflow also verified native DLL/SO modules on the same three environments.

## Local reproduction

Requires Python 3.10+, CMake 3.20+, a C++17 compiler:

```sh
python tools/ci_verify.py --compiler default
# Linux/Clang only:
python tools/ci_verify.py --compiler clang
```

The verifier writes command logs and JSON summaries under `evidence/ci/<platform>/<compiler>/`, while the GitHub workflow uploads job artifacts.

## Scope / limits

This repository contains a **technology prototype**, not a complete UMAA 6.0 implementation or official K-MOSA/UMAA certification. `sdk/fixtures/demo.idl` is synthetic, the generator implements a deliberately restricted IDL subset, and the independent simulator uses a test-fixture stdio protocol. No real Cyclone DDS, RTI Connext DDS, official UMAA 6.0 TypeSupport, or mixed-vendor DDS test has been executed. The native module ABI is project-defined, not an official K-MOSA ABI, and does not implement live hot swap of active commands.

Read [delivery gates](docs/DELIVERY_GATES.md), [architecture](docs/ARCHITECTURE.md) and [validation evidence](docs/CROSS_PLATFORM_VALIDATION.md) before treating any subset as production-ready.
