# Typed C++ SDK prototype (independent Project A)

**Status:** Verifiable core slice, not full UMAA 6.0 compliance.

## Public workflow

From the repository root:

```bash
python3 sdk/tools/umaa_codegen.py inspect --idl sdk/fixtures/demo.idl
python3 sdk/tools/umaa_codegen.py generate \
  --idl sdk/fixtures/demo.idl --catalog sdk/fixtures/services.json \
  --service Demo::EO::EngineControl --out build/generated
python3 sdk/tools/umaa_codegen.py generate \
  --idl sdk/fixtures/demo.idl --catalog sdk/fixtures/services.json \
  --service Demo::EO::BatteryStatus --out build/generated
```

The generator writes `/include/umaa/generated/*.hpp` (safe to regenerate) and `/user/My*.hpp` (written once, never overwritten). The example DTOs are **synthetic**, not wire-compatible UMAA IDL artifacts. A real DDS IDL compiler and generated TypeSupport are required when integrating official UMAA IDL.

## User-side contract

- For command Provider, implement `execute(typed_payload)` and `cancel(typed_payload)`; optionally override `validate`. The SDK owns session identity, transition sequencing and response event production.
- For report Provider, implement `read_report()`. The SDK provides source/timestamp bookkeeping, cache and dispose primitives.
- Use the SDK-generated API, not low-level Writer/Reader, in your device/business code.

## Current unsupported IDL syntax

The generator deliberately accepts **only** nested `module`, primitive `struct`, `@key`, strings, and simple signed/unsigned numeric fields. Actual UMAA 6.0 IDL uses other constructs such as imports, typedefs, enums, user-defined nested types, sequences, and optional members. The generator **fails closed** rather than silently corrupting types. It is NOT a substitute for official IDL code generation.

Service definitions are not inferable unambiguously from IDL names, so an explicit Service Catalog is mandatory. The catalog in `fixtures/` is deliberately marked "demo-only".

## CMake and tests

```bash
cmake -S sdk -B build/sdk-cmake -G Ninja
cmake --build build/sdk-cmake
ctest --test-dir build/sdk-cmake --output-on-failure
cmake --install build/sdk-cmake --prefix build/sdk-install
```

An external app may `find_package(UMAAPrototype CONFIG REQUIRED)` and `target_link_libraries(app PRIVATE UMAA::Core)`.

## Vendor integration contract (planned; NOT implemented)

- Cyclone DDS-CXX: primary adapter candidate because upstream AUVSI Common uses it.
- RTI Connext 7.7: second independently testable adapter candidate, requires valid SDK/license.
- Keep generated vendor-specific DDS C++ types outside the public user Handler API.
- Reject a vendor that cannot meet the mandatory LargeCollection `PRESENTATION` QoS, rather than silently downgrading compliance.
