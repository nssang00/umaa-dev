# Progress and evidence

## Verified checkpoint 1: typed SDK skeleton, 2026-10-23
- Added strictly parsed synthetic IDL fixture and explicit service-catalog driven generator.
- Preserves user Handler code on regeneration; compilation proven.
- Added C++ CommandEngine common protocol skeleton with source/session/timestamp and terminal cleanup.
- Added independent simulator verifier and four subprocess scenarios.
- First pass: 4 SDK parser tests, 5 simulator oracle tests, 1 C++ engine test, 4 end-to-end scenarios.

## Verified checkpoint 2: coverage, corrected cancellation, 2026-10-23
- Found and corrected initial cancel-refused + later completion cleanup edge case (previous session resource leak).
- Added `ReportProvider`, `ReportCache`, `CollectionAssembler` with set/list and pending/committed snapshot separation.
- Added generated Report Handler, C++ tests and packaging.
- Verified 5 SDK parser tests, 6 simulator oracle tests, C++ engine and report/collection tests, generator-compiled report smoke, CMake tests and independent consumer.
- Ran UndefinedBehaviorSanitizer on C++ protocol and collection unit executables: no observed issue.
- Negative-control test: deliberately invalid external SUT trace was rejected by independent simulator (evidence/negative_control.txt).
- Exported and validated CMake installable target `UMAA::Core` with an external consumer project.

## Current verified envelope
- Four independently driven scenarios / 32 command-related events; trace artifacts under evidence/.
- No real DDS tests. See `evidence/vendor_environment.json`.
- External official UMAA 6.0 IDL, full compliance suite and active DDS vendors missing.
- Outcome: **PARTIAL working prototype**, NOT complete UMAA SDK / conformance / production.

## Next work (blocked/deferred, not represented as ongoing background work)
- Clear official input and DDS vendor availability gates in docs/DELIVERY_GATES.md; implement real TypeSupport, runtime adapters, broad protocols, independent DDS simulator and conformance corpus.

## Verified checkpoint 3: CI-ready platform boundary and two service modules, 2026-10-23
- Created `.github/workflows/ci.yml` with `ubuntu-24.04` GCC/Clang and `windows-2022` MSVC jobs, using a single portable `tools/ci_verify.py`; upload logs/artifacts on failures.
- Prototype loader: cross-platform DLL/SO C ABI (`module_api.h`, `module_loader.hpp`), service/schema validation, distinct EngineControl and BatteryStatus modules, incompatible ABI fixture. Host reserves UMAA protocol state; modules implement demo-only actions.
- Re-ran Linux GCC 14 and Clang 17: Python SDK 5/5, Python independent oracle 6/6, CTest 4/4, independent consumer linking, 4 process scenarios (32 events) each. See `evidence/ci/linux/{default,clang}/summary.json`.
- Found and corrected a dirty CMake cache issue when swapping the Clang external-consumer compiler; clean consumer build now passes. CI runners start from clean checkout.
- GitHub smoke run https://github.com/nssang00/umaa-dev/actions/runs/39930834209 confirms native DLL/SO service module prototype on Windows/MSVC and Ubuntu/GCC+Clang (three jobs succeeded). These are **not** full SDK/Simulator or real-DDS results. Full local prototype sources have not yet been imported.
- The K-MOSA ABI is a **proposal** inspired by MOSA principles, not a verified government interface. All official IDL/DDS/compliance gates remain blocked.
## 2026-10-23 — GitHub Windows/Linux full prototype validation

- Full SDK and independent simulator code (31 native source and tool files) integrated to `nssang00/umaa-dev`.
- Linux local GCC and Clang `python tools/ci_verify.py` passed.
- GitHub-hosted matrix Linux/GCC, Linux/Clang, Windows/MSVC passed **after source import and cleanup**: https://github.com/nssang00/umaa-dev/actions/runs/39941404598
- Each job: Python codegen 5 tests, simulator oracle 6 tests, C++ CTest 4/4, installed CMake external consumer, 4 subprocess simulator scenarios (32 observed events).
- Temporary source-transfer bundles and import workflow were removed from current repo tree.
- Open: official UMAA 6.0 IDL and Codegen, CycloneDDS-CXX/RTI transports, wire-level QoS and conformance testing, verified safe hot swap and official K-MOSA conformance.

## 2026-10-23 — Milestone 2 real Cyclone DDS verified (limited BatteryStatus scope)

- Official AUVSI 6.0 ZIP (492 files / 352 .idl), SHA-256 `9674ef84b92cc296a544d001d58149efbb4650f4fafe3a40819636df49c42411`, fetched in ephemeral GitHub Runner.
- BatteryReportType and 5 transitive dependencies: Cyclone C TypeSupport generation and compilation on Linux and Windows.
- Linux: SO BatteryStatus module invokes through project-owned host ABI, publishes official BatteryReportType `chargeRemaining` 20→80 over actual DDS. Separate Simulator validates both samples and DDS keyed Dispose; [Actions run](https://github.com/nssang00/umaa-dev/actions/runs/39952219497) PASS.
- Windows: DLL BatteryStatus module and MSVC official C TypeSupport same on-wire operation, [Actions run](https://github.com/nssang00/umaa-dev/actions/runs/39953508112) PASS.
- EngineCommandType and EngineCommandStatusType generated/compiled via official Cyclone `idlc` on Linux; [run](https://github.com/nssang00/umaa-dev/actions/runs/39952552116) PASS. Actual EngineControl DDS command state machine integration is **NOT** implemented.
- Open: official UMAA service-compliant Topic/Key/Source/time semantics, QoS full mapping, Handler codegen on official IDL, full EngineControl on-wire flow, RTI integration, official K-MOSA conformity and all-ICD conformance. This proof remains a **partial, non-certified prototype**.
