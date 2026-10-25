# Delivery gates and blockers (Do not claim finished until passed)

| Gate | Acceptance test | Current state |
|---|---|---|
| G1 User typed API | Generate selected command/report handlers; user file stays intact | PARTIAL: typed interfaces generated and compiled for 53 reviewed literal-service contract shapes / 113 official Topic types across six ICDs on Linux; Windows pending same-commit native confirmation; runtime adapters for most services remain OPEN; generated EngineControl user module still ignition-only |
| G2 C++ common protocol | Command status/ack/update/cancel/recovery policy, report, collection tests | PASS for tested prototype paths |
| G3 Independent simulator | Runs external process, rejects invalid traces, verifies expected scenarios | PASS for stdio fixture + independent official UMAA Cyclone DDS BatteryStatus/EngineControl Command/Update cross-process scenarios on Windows and Linux; full conformance still OPEN |
| G4 SDK distribution | Install CMake target and import from external consumer | PASS |
| G5 Official IDL 6.0 | Generate and C-compile all AUVSI 6.0 IDL types | **PASS for Cyclone DDS C on Ubuntu: 352/352 generated and 352/352 compiled**, official ZIP SHA-pinned; not equivalent to all UMAA service codegen
| G6 Service catalog | Every generated topic, role, required QoS validated against official Component Definitions/ICDs | OPEN: 352 official IDL files inventoried; 61 directory candidates (not normative service count); 53 curated service sections / 113 Topic types across EO/MO/MM/SA/SEM/SO; all six ICD literal Topic mappings PASS. 61 IDL folder candidates (not normative services), 8 unreviewed. CO/EXP scope and official Component Definitions Provider/Consumer roles, QoS and all-service runtime behavior OPEN. See docs/ICD_CATALOG_AUDIT.md |
| G7 Full protocol | LargeSet/List full DDS atomic semantics, specializations, startup/recovery, liveliness, all reasons | OPEN |
| G8 Cyclone DDS | Real participant/reader/writer over official IDL incl. keyed/Dispose/liveliness/QoS | **PARTIAL:** official BatteryStatus/EngineControl real DDS PASS on Linux+Windows; Linux 3-node/2-node netns+veth+bridge+tc netem clean, impaired, 100% loss then initial discovery recovery PASS; mid-session partition, full QoS/liveliness OPEN
| G9 RTI Connext 7.7 | Build/run same user handler on RTI + licensing | BLOCKED (missing RTI SDK/license) |
| G10 Cross vendor | Cyclone↔RTI both directions with specified QoS and XTypes | BLOCKED |
| G11 Independent conformance | Full ICD+Compliance Specification test corpus and evidence | OPEN (public Compliance Specification and Component Definitions are available from AUVSI, but entire corpus has not yet been traced and independently executed) |
| G12 Production | Fuzzing, race/leak/resource/timeout/security tests, hardware-level functional safety | NOT STARTED |

## Inputs and environment needed to clear blockers

1. Authoritative UMAA 6.0 IDL ZIP is downloaded at CI build time (not redistributed); review its license/TOU and thoroughly assess the publicly available Compliance Specification, Component Definitions, and all 6.0 ICDs.
2. Cyclone DDS C with idlc is installed in Linux and Windows Actions. Expand to CycloneDDS-CXX when reusing upstream SDK Common, pin versions, and capture upstream regression tests.
3. Install licensed RTI Connext 7.7 with `rtiddsgen` and verify supported Presentation QoS; set up two-way DDS interoperability test processes.
4. 53 six-ICD literal service contracts have generated C++ Handler interfaces using official CycloneDDS TypeSupport on Linux and Windows; bridge additional service handlers into real DDS protocol execution and independently test every required behavior.
5. Add the missing protocol rules, dynamic/distributed lifecycle and exhaustive negative tests; do not silently emulate unsupported QoS.

If these gates cannot be met, the correct delivery status is *partial* rather than success.