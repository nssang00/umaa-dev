# Cross-platform full prototype validation — 2026-10-23

**Authoritative run:** https://github.com/nssang00/umaa-dev/actions/runs/39941190249

| Job | GitHub-hosted environment | Result | CTest | Scenario count |
|---|---|---|---|---|
| 120372740694 | Ubuntu 24.04 / GCC | PASS | 4/4 | 4 |
| 120372740876 | Ubuntu 24.04 / Clang | PASS | 4/4 | 4 |
| 120372741041 | Windows Server 2022 / MSVC | PASS | 4/4 | 4 |

**Evidence from GitHub job logs:** `CI PASS` is emitted by `tools/ci_verify.py` after all steps. Each job logged:
- Python IDL-subset generator tests: 5 passing
- Python independent trace verifier tests: 6 passing
- C++ CTest: `umaa_command_engine_test`, `umaa_report_collection_test`, `umaa_dynamic_module_test`, `umaa_generated_report_smoke` — 4/4
- SDK CMake installation and external consumer compilation/execution
- Independent subprocess Simulator scenarios: invalid payload (5 events), normal (7), recovery/replay (9), update/cancel (11) — **32 events**

The sample messages are synthetic and exchange **stdio**, not DDS. This run does not establish official UMAA 6.0 compliance, K-MOSA certification, or DDS vendor interoperability.

**To reproduce:** `python tools/ci_verify.py --compiler default` (use `--compiler clang` on Ubuntu/Clang).
