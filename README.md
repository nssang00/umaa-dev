# UMAA SDK + Independent Simulator (verified prototype slice)

This repository contains **two independent projects**, `sdk/` and `simulator/`, plus a reproducible local integration harness. No existing AUVSI SDK source files are redistributed here.

## Read this first

**This is NOT an end-to-end UMAA 6.0 SDK, a full OMG IDL compiler, an actual DDS simulator, or an UMAA compliance certification.** The source archive supplied for review contained AUVSI SDK Common, but did not contain official UMAA 6.0 IDL ZIP, required vendor development packages, or the entire compliance corpus. To avoid false success, the included `sdk/fixtures/demo.idl` is explicitly synthetic and the independent simulator uses a development-only stdio event protocol. Actual Cyclone/RTI transports remain a documented integration gate.

What **does** work now: strict subset-IDL to typed C++ Handler generation; per-service explicit manifest lookup (no guessing); generated user code preserved on regeneration; shared C++ command status engine; report cache/provider; atomic LargeSet/LargeList staging; cross-process independent simulator oracle; CMake export/install with consumer smoke test.

## Cross-platform CI and K-MOSA-aligned service modules

- `.github/workflows/ci.yml`: Ubuntu 24.04 (GCC and Clang) + Windows 2022 (MSVC) matrix, 3 checks. **Authored but NOT run on GitHub**: there is no configured UMAA GitHub repository in the connected accounts. The Linux GCC/Clang equivalents have passed locally; Windows remains pending.
- `python tools/ci_verify.py`: portable Python+CMake verification entry point, used by CI. It builds a separate `EngineControl` and `BatteryStatus` plugin as `.so` on Linux or `.dll` on Windows, tests module ABI and schema/service mismatch rejection, generated Handler smoke and independent Simulator scenarios.
- `docs/K_MOSA_DYNAMIC_MODULES.md`: service-per-module design using a private project-owned C ABI, **not an official K-MOSA ABI**. The module host retains DDS/UMAA protocol ownership. Live hot swap and session drain are NOT implemented in the demo.
- `docs/GITHUB_ACTIONS.md`: matrix, proof/limitations and activation steps.

## Reproduce validation

```bash
bash validate.sh
cat evidence/summary.json
```

Requirements for legacy `validate.sh`: Linux, Python 3.10+, g++ or compatible C++17 compiler, CMake, Ninja, `ctest`. For cross-platform CI use `python tools/ci_verify.py` (MSVC on Windows 2022; GCC/Clang on Ubuntu). The validator uses standard-library-only Python; it never connects to an external service. It intentionally does **not** claim a DDS test.

The C++ generated API is an *application development convenience API*; it does not alter standard UMAA topic names, type definitions, or on-wire behavior. The current example is a synthetic harness and cannot yet emit real UMAA DDS samples.

## Artifacts

- `sdk/`: C++ core, IDL-subset generator, typed example, tests, installable CMake INTERFACE package.
- `simulator/`: independent Python verifier and scenario runner. No imports from SDK.
- `docs/ARCHITECTURE.md`: system boundaries and future real-DDS design.
- `docs/TRACEABILITY.md`: EO ICD mapped requirements, evidence, gaps.
- `docs/DELIVERY_GATES.md`: precise blocking requirements for production.
- `evidence/`: replayable scenario traces and test logs.
- `PROGRESS.md`: verified work and remaining blockers.

## Separate lifecycle

`simulator/` is an independent source project. It accepts any process that implements the test-fixture text protocol in `simulator/README.md`. The SDK itself does not embed or import the simulator. When real DDS adapters exist, the fixture protocol should be retired for target-conformance runs: both projects would exchange official UMAA DDS topics directly.