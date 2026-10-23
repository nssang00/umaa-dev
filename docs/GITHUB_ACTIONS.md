# GitHub Actions — verified cross-platform prototype

Workflow: [`.github/workflows/ci.yml`](../.github/workflows/ci.yml)

- Runs on every push, pull request, or manual dispatch.
- Matrix: `ubuntu-24.04` GCC, `ubuntu-24.04` Clang, `windows-2022` MSVC.
- Runs `python tools/ci_verify.py --compiler <default|clang>`, with Python 3.13, CMake and CTest.
- Verifies native service DLL/SO module loader, Command/Report/Collection unit behavior, subset-IDL generator, CMake install and external consumer, and **separate independent Simulator** subprocess scenarios.
- Uploads per-run JSON and detailed logs in Actions artifacts.

**Latest proof:** https://github.com/nssang00/umaa-dev/actions/runs/39962717654 — all three jobs SUCCESS, CTest 4/4 and Simulator 4 scenarios in each environment.

**This baseline workflow alone** is synthetic and does not cover DDS. Separate native DDS workflows now test official EngineControl and BatteryStatus; Cyclone↔RTI interop, full ICD compliance, K-MOSA certification and safe active-command hot swap remain unverified.

These gaps are documented in `docs/DELIVERY_GATES.md`. **Do not mark blocked acceptance gates as pass.**

## True DDS integration jobs

- [`.github/workflows/real-dds.yml`](../.github/workflows/real-dds.yml): Ubuntu native Cyclone DDS C TypeSupport with synthetic IDL. [Passing source run](https://github.com/nssang00/umaa-dev/actions/runs/39950750899).
- [`.github/workflows/official-idl-check.yml`](../.github/workflows/official-idl-check.yml): ephemeral AUVSI official 6.0 IDL archive, recursive TypeSupport generation, Engine Command/Status type compilation, **official BatteryReportType Linux wire CTest**. [Passing run](https://github.com/nssang00/umaa-dev/actions/runs/39952552116).
- [`.github/workflows/real-dds-windows.yml`](../.github/workflows/real-dds-windows.yml): vcpkg Cyclone DDS 0.10.5 + MSVC, both synthetic and official UMAA BatteryReportType DDS CTest with native DLL module. [Passing run](https://github.com/nssang00/umaa-dev/actions/runs/39953508112).

Both projects are distinct: the Simulator connects via DDS from an independent process and never links SDK Command state machine code. Official IDL is downloaded into CI runner temp, and must never be redistributed with the prototype. See [REAL_DDS_EVIDENCE.md](REAL_DDS_EVIDENCE.md) for limitations.

## Milestone 2 official live DDS and complete IDL coverage

- [Linux official-IDL integration](https://github.com/nssang00/umaa-dev/actions/runs/39962602416): BatteryStatus and EngineControl Command/Update CTest **3/3**, generated ignition-only Handler module loaded as native SO.
- [Windows official-IDL integration](https://github.com/nssang00/umaa-dev/actions/runs/39962717265): same official UMAA DDS **3/3** on MSVC, generated DLL.
- [Full official IDL TypeSupport coverage](https://github.com/nssang00/umaa-dev/actions/runs/39963083783): **352/352** official idlc generation and **352/352** generated C compilation on Ubuntu; this does not establish all service APIs or compliance.

See [ENGINECONTROL_EVIDENCE.md](ENGINECONTROL_EVIDENCE.md) for correct scope limits and links.