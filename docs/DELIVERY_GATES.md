# Delivery gates and blockers (Do not claim finished until passed)

| Gate | Acceptance test | Current state |
|---|---|---|
| G1 User typed API | Generate selected command/report handlers; user file stays intact | PASS for synthetic primitive IDL + generated, official-TypeSupport-verified EngineControl ignition-only Handler/DLL-SO; remaining fields/services OPEN |
| G2 C++ common protocol | Command status/ack/update/cancel/recovery policy, report, collection tests | PASS for tested prototype paths |
| G3 Independent simulator | Runs external process, rejects invalid traces, verifies expected scenarios | PASS for stdio fixture + independent official UMAA Cyclone DDS BatteryStatus/EngineControl Command/Update cross-process scenarios on Windows and Linux; full conformance still OPEN |
| G4 SDK distribution | Install CMake target and import from external consumer | PASS |
| G5 Official IDL 6.0 | Generate and C-compile all AUVSI 6.0 IDL types | **PASS for Cyclone DDS C on Ubuntu: 352/352 generated and 352/352 compiled**, official ZIP SHA-pinned; not equivalent to all UMAA service codegen
| G6 Service catalog | Every generated topic, role, required QoS validated against official Component Definitions/ICDs | OPEN (Service Catalog not independently derived or verified across all ICDs; current TypeSupport is official but topic name/QoS and all service fields need compliance review) |
| G7 Full protocol | LargeSet/List full DDS atomic semantics, specializations, startup/recovery, liveliness, all reasons | OPEN |
| G8 Cyclone DDS | Real participant/reader/writer over official IDL incl. keyed/Dispose/liveliness/QoS | **PARTIAL:** real official BatteryStatus and EngineControl Command/Update/Ack/Status/Dispose PASS on Linux+Windows, comprehensive QoS/liveliness/recovery still OPEN
| G9 RTI Connext 7.7 | Build/run same user handler on RTI + licensing | BLOCKED (missing RTI SDK/license) |
| G10 Cross vendor | Cyclone↔RTI both directions with specified QoS and XTypes | BLOCKED |
| G11 Independent conformance | Full ICD+Compliance Specification test corpus and evidence | BLOCKED (missing compliance materials) |
| G12 Production | Fuzzing, race/leak/resource/timeout/security tests, hardware-level functional safety | NOT STARTED |

## Inputs and environment needed to clear blockers

1. Authoritative UMAA 6.0 IDL ZIP is downloaded at CI build time (not redistributed); review its license/TOU and acquire the full Compliance Specification and Component Definitions, which have not been fully assessed.
2. Cyclone DDS C with idlc is installed in Linux and Windows Actions. Expand to CycloneDDS-CXX when reusing upstream SDK Common, pin versions, and capture upstream regression tests.
3. Install licensed RTI Connext 7.7 with `rtiddsgen` and verify supported Presentation QoS; set up two-way DDS interoperability test processes.
4. BatteryStatus now has a manually bridged official UMAA BatteryReportType wire proof on Linux; replace synthetic user-facing codegen with official service-specific typed Handler generation, then independently test all mandatory protocol behavior.
5. Add the missing protocol rules, dynamic/distributed lifecycle and exhaustive negative tests; do not silently emulate unsupported QoS.

If these gates cannot be met, the correct delivery status is *partial* rather than success.