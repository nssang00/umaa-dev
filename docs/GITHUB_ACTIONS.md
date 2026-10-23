# GitHub Actions — verified cross-platform prototype

Workflow: [`.github/workflows/ci.yml`](../.github/workflows/ci.yml)

- Runs on every push, pull request, or manual dispatch.
- Matrix: `ubuntu-24.04` GCC, `ubuntu-24.04` Clang, `windows-2022` MSVC.
- Runs `python tools/ci_verify.py --compiler <default|clang>`, with Python 3.13, CMake and CTest.
- Verifies native service DLL/SO module loader, Command/Report/Collection unit behavior, subset-IDL generator, CMake install and external consumer, and **separate independent Simulator** subprocess scenarios.
- Uploads per-run JSON and detailed logs in Actions artifacts.

**Latest proof:** https://github.com/nssang00/umaa-dev/actions/runs/39941404598 — all three jobs SUCCESS, CTest 4/4 and Simulator 4 scenarios in each environment.

**Not covered:** official UMAA 6.0 IDL, real DDS network/RTPS transport, Cyclone↔RTI on-wire interoperability, full ICD compliance, K-MOSA certification, safe active-command hot swap.

These gaps are documented in `docs/DELIVERY_GATES.md`. **Do not mark blocked acceptance gates as pass.**
