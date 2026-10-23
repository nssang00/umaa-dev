# UMAA SDK + Independent Simulator

Two **separate projects** for development and independent verification of UMAA services:

- [sdk/](sdk/): synthetic-subset IDL generator, typed C++ Handler examples, Command/Report/Collection prototype, versioned native service module loader (DLL/SO), installable CMake package and tests.
- [simulator/](simulator/): independent scenario runner and protocol trace verifier. Baseline synthetic fixture exchanges JSON over stdio. Additional native DDS simulator programs in `simulator/dds/` independently receive samples and Dispose via **real Cyclone DDS**.

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

## CycloneDDS real-wire proof (official and synthetic)

- [Linux official AUVSI UMAA 6.0 BatteryReportType + SO module → Cyclone DDS → separate Simulator test](https://github.com/nssang00/umaa-dev/actions/runs/39952219497) — PASS.
- [Windows vcpkg Cyclone DDS 0.10.5 + DLL plugin + synthetic test IDL → separate Simulator](https://github.com/nssang00/umaa-dev/actions/runs/39952395220) — PASS.
- [Official AUVSI IDL archive verification and Engine Command/Status TypeSupport compilation](https://github.com/nssang00/umaa-dev/actions/runs/39952552116) — PASS.

The official IDL ZIP is downloaded to ephemeral CI space for each applicable run; it is **not committed or redistributed**. UMAA IDL TOU must be reviewed for any downstream licensing/distribution.

**Conformance boundaries:** the BatteryStatus adapter currently uses manual C TypeSupport glue and a shortened DDS topic string for testing, with some metadata fields unset. It is a valid on-wire **type and lifecycle proof**, *not an UMAA ICD service conformance verdict*. LargeCollection QoS, startup/recovery, timestamps, service addressing, official Topic naming, CycloneDDS-CXX parity, and EngineControl full flow are still open.

## Scope / limits

This repository contains a **technology prototype**, not a complete UMAA 6.0 implementation or official K-MOSA/UMAA certification. `sdk/fixtures/demo.idl` is synthetic, the generator implements a deliberately restricted IDL subset, and the independent simulator uses a test-fixture stdio protocol. **Real Cyclone DDS tests are now available:** Linux and Windows synthetic-IDL module-backed cross-process samples; Linux official AUVSI UMAA 6.0 `BatteryReportType` compiled from downloaded IDL and verified over DDS with an independent simulator (20→80→Dispose). Official `EngineCommandType` and `EngineCommandStatusType` TypeSupport generation/compilation also passed on Linux, but **actual EngineControl protocol flow is not yet tested**. RTI Connext DDS and mixed-vendor tests have not been executed. Official data types are not yet integrated into the user-facing general-purpose generator. The native module ABI is project-defined, not an official K-MOSA ABI, and does not implement live hot swap of active commands.

Read [delivery gates](docs/DELIVERY_GATES.md), [architecture](docs/ARCHITECTURE.md) and [validation evidence](docs/CROSS_PLATFORM_VALIDATION.md) before treating any subset as production-ready.
