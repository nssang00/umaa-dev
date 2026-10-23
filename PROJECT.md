# UMAA developer SDK and independent verification simulator

Goal: preserve official UMAA IDL and on-wire API; implement protocol plumbing in SDK, exposing typed domain handlers; verify independently of SDK through a separate simulator project. This repository is a self-contained **prototype slice**, not claimed UMAA-compliant production software.

Inputs: user-uploaded UMAA EO ICD v6.0 (2024-06-06), AUVSI SDK Common v1.0.0, AGENTS.md. Official UMAA IDL ZIP and full Compliance Specification not provided; no redistribution of original IDL.

Projects: `sdk/` generates typed C++ skeletons from a strictly supported IDL subset plus service manifest, implements a verifiable Command protocol core. `simulator/` drives an independently compiled process and independently verifies traces over a development-only standard-I/O test protocol (NOT DDS/UMAA wire protocol).

Environment: Python 3.13+, C++17+; no third-party runtime dependencies required for the skeleton/test harness. For official DDS conformance, real DDS adapters, actual IDL, and compliance corpus are required.

Tests: `bash validate.sh` (build, Python unit tests, C++ protocol tests, process-boundary integration tests).  Vendor acceptance: Cyclone DDS and RTI Connext 7.7 as TARGETS only; not validated here.

Safety and controls: no real hardware attached; no release deployment; no unsafe device actuation. Never treat simulated passed traces as certified UMAA conformance.

Added goal (2026-10-23): independent dynamically loadable UMAA **service** modules for K-MOSA-aligned composition, while keeping the UMAA Protocol Core and DDS vendor adapter host-owned. The first proof is a private versioned C ABI loading two synthetic service plugins and rejecting mismatch; NOT a normative K-MOSA interface or hot-swap certification. CI targets GitHub ubuntu-24.04 (GCC/Clang) and windows-2022 (MSVC). Actual GitHub Actions require a repository; none matched the existing local workspace. Windows result is pending.