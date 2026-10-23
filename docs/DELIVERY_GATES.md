# Delivery gates and blockers (Do not claim finished until passed)

| Gate | Acceptance test | Current state |
|---|---|---|
| G1 User typed API | Generate selected command/report handlers; user file stays intact | PASS for synthetic primitive IDL |
| G2 C++ common protocol | Command status/ack/update/cancel/recovery policy, report, collection tests | PASS for tested prototype paths |
| G3 Independent simulator | Runs external process, rejects invalid traces, verifies expected scenarios | PASS using stdio fixture |
| G4 SDK distribution | Install CMake target and import from external consumer | PASS |
| G5 Official IDL 6.0 | Compile official IDL of each supported service, incl. enums/typedefs/annotations/sequences | BLOCKED (IDL ZIP unavailable; parser intentionally limited) |
| G6 Service catalog | Every generated topic, role, required QoS validated against official Component Definitions/ICDs | BLOCKED (complete specs not supplied) |
| G7 Full protocol | LargeSet/List full DDS atomic semantics, specializations, startup/recovery, liveliness, all reasons | OPEN |
| G8 Cyclone DDS | Real DDS participant/reader/writer over official IDL, keyed/Dispose/liveliness/QoS tests | BLOCKED (missing Cyclone DDS-CXX + generated types) |
| G9 RTI Connext 7.7 | Build/run same user handler on RTI + licensing | BLOCKED (missing RTI SDK/license) |
| G10 Cross vendor | Cyclone↔RTI both directions with specified QoS and XTypes | BLOCKED |
| G11 Independent conformance | Full ICD+Compliance Specification test corpus and evidence | BLOCKED (missing compliance materials) |
| G12 Production | Fuzzing, race/leak/resource/timeout/security tests, hardware-level functional safety | NOT STARTED |

## Inputs and environment needed to clear blockers

1. Acquire and legally review the authoritative UMAA 6.0 IDL ZIP, full Compliance Specification and Component Definitions; keep original files in their own managed locations with their license/TOU; do not simply bundle into SDK releases.
2. Install pinned Cyclone DDS + CycloneDDS-CXX + code generator and required generated UMAA types; build upstream SDK Common and capture passing regression results before refactor.
3. Install licensed RTI Connext 7.7 with `rtiddsgen` and verify supported Presentation QoS; set up two-way DDS interoperability test processes.
4. Replace synthetic fixtures with actual, service-specific official bindings; keep simulator's conformance oracle independent.
5. Add the missing protocol rules, dynamic/distributed lifecycle and exhaustive negative tests; do not silently emulate unsupported QoS.

If these gates cannot be met, the correct delivery status is *partial* rather than success.