# Windows + Linux CI for the UMAA prototype

Workflow: `.github/workflows/ci.yml`, fixed runners `ubuntu-24.04` and `windows-2022`, Python 3.13, CMake C++17. The single cross-platform command is `python tools/ci_verify.py`.

Actions executes: generator unit tests, independent simulator oracle tests, CMake build and CTest (Command, Report/LargeCollection, per-service shared module load/unload, generated Report smoke), `cmake --install`, independent `find_package(UMAAPrototype)` consumer smoke, and 4 out-of-process simulator scenarios. CTest runs in Debug configuration to keep C++ assertions active on MSVC. Evidence uploaded even on failure via `actions/upload-artifact`.

**Status:** Full local workflow has been tested **on Linux only**; the smaller smoke workflow has been tested **on GitHub Linux and Windows**. GitHub repository nssang00/umaa-dev now exists. A **reduced service-module smoke** is committed and actually passed on Ubuntu GCC, Ubuntu Clang and Windows MSVC: https://github.com/nssang00/umaa-dev/actions/runs/39930834209 . The complete local SDK + Simulator workflow shown in this document is **not committed to GitHub and has not run on Windows**. Never report `windows-2022` as passing until an actual job URL and run logs are available.

To enable it: commit the workspace to the intended GitHub repository and push a branch; Actions runs on push/pull_request or manually via workflow_dispatch. Require both `build-test` matrix jobs to pass before merging.

Real DDS validation is a separate CI gate. This prototype has no official UMAA 6.0 IDL TypeSupport, no Cyclone DDS-CXX or RTI Connext backend, and no independently verified on-wire simulator. Current CI must not be called UMAA conformance or multi-DDS interoperability.