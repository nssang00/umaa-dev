# UMAA 6.0 G6 service contract inventory — incremental proof (2026-10-25)

**Decision: G6 remains OPEN.** Do not infer UMAA service compliance from IDL
directory names, matching Topic type names, or a compiling Handler contract.

## Two independent sources, two distinct kinds of evidence

1. **AUVSI official UMAA 6.0 IDL:** ephemeral ZIP (SHA-256
   `9674ef84b92cc296a544d001d58149efbb4650f4fafe3a40819636df49c42411`)
   fetched on the Linux GitHub runner. `tools/audit_official_idl_catalog.py`
   enumerates every file, computes per-file hashes and highlights candidate
   directory groups. Only manually reviewed entries of
   `catalog/umaa6-eo-reviewed-subset.json` enter the service Handler generator.
2. **Six public 6.0 ICD Markdown transcriptions** from RTI Community commit
   [ecb923646e7f3fe569b773c3946444e793d8a8be](https://github.com/rticommunity/rticonnextdds-usecases-umaa/commit/ecb923646e7f3fe569b773c3946444e793d8a8be),
   consumed ephemerally. `tools/extract_icd_topic_ledger.py` reads explicit
   Section 5.1 / 6.1 operation headers plus literal Namespace / Topic / Data Type
   declarations; every extracted Topic is matched against the official IDL ZIP.
   The resulting `official-icd-topic-ledger.json` keeps ICD clause references
   and SHA-256 of each transcription. An independent EO-only completeness check requires 14 service sections and 21 literal Topic IDs to match the curated catalog. These transcriptions are secondary sources;
   the AUVSI ICD, Component Definitions and Compliance Specification are the
   authoritative acceptance inputs.

## Observed quantitative results (GitHub Actions Linux)

| Independent measure | Verified value |
|---|---:|
| UMAA official IDL files enumerated | 352 |
| IDL directory groups that resemble service folders | 61 (not normative service count) |
| Manually reviewed EO services in SDK catalog | 14 |
| Types present in official IDL and selected for those services | 21 |
| Not-yet-reviewed IDL directory groups | 47 |
| Public ICD texts processed | 6: EO, MO, MM, SA, SEM, SO |
| Explicit service sections found in those ICDs | 53 |
| Explicit operation/Topic/IDL triples in those ICDs | 113 |
| Pre-reviewed catalog Topic IDs cross-checked against ICD records | 21 |
| Full G6 service/QoS/Component acceptance | **FAIL / OPEN** |

The 14 reviewed EO services are AnchorControl, AnchorSpecs, AnchorStatus,
BatterySpecs, BatteryStatus, EngineControl, EngineStatus, FuelTankSpecs,
FuelTankStatus, GeneratorSpecs, GeneratorStatus, MastControl, MastStatus, and
UVPlatformSpecs. All 14 explicit EO 6.1 service sections and all 21 of their
Topic IDs now have curated Type/role mappings. This is a **literal Topic mapping**
milestone, not QoS or runtime-complete certification. Both single- and **multi-Report** service shapes are
supported by the **typed interface generator**; this is not runtime proof of
every service protocol. Official TypeSupport for all 21 scoped Topics generated
on Ubuntu and Windows; generated C++ service contracts also compile on both.

## Proof and fail-closed controls

- [Linux official IDL, compiled typed APIs, independent netns+netem DDS tests](https://github.com/nssang00/umaa-dev/actions/runs/40112466454).
  Evidence artifact `linux-dds-namespace-integration` includes
  `official-idl-catalog-inventory.json`,
  `official-idl-catalog-strict-evidence.json`,
  `official-icd-topic-ledger.json` and DDS process logs.
- `--strict-g6` must return **3** while Component Definitions, full service
  catalog, all QoS and runtime behavior remain unverified. CI asserts this exact
  nonzero status instead of silently marking compliance.
- Schema drift (352 file count), mismatched ICD namespace/Topic/data type,
  nonexistent official IDL, incomplete source documents, missing manually
  reviewed Topic, and unsafe duplicate topic contracts fail the audit.
- Linux/Windows baseline SDK and native DDS proof are separate from full G6.

## Next audit tasks / release blockers

- **CO ICD** is not present in the pinned six-document transcription batch.
  Experimental (EXP) services are explicitly out of this nonexperimental
  catalog scope. Complete the authoritative public CO ICD trace separately.
- Read the complete [AUVSI UMAA release](https://www.auvsi.org/advocacy/advocacy-initiatives/unmanned-maritime-autonomy-architecture/)
  **Component Definitions v1.0**, all applicable 6.0 ICDs and
  **Compliance Specification**; resolve provider/consumer direction,
  query/cancel operations, real DDS Topic strings, QoS, entity keys,
  LargeSet/List, startup, dispose, liveliness and session recovery.
- Expand the reviewed catalog one service at a time without assuming one
  service equals one Topic or every report-shaped IDL is a DDS Report Topic.
  Use `official-icd-topic-ledger.json` as a *review queue* only.
- Tie codegen, generated DLL/SO adapters, native cross-process DDS verification,
  negative cases and vendor interop to every reviewed service.
- RTI Connext 7.7 SDK/license and vendor interoperability remain unverified.

**No official IDL ZIP, transcribed full ICD text, or restricted files are
committed to this repository.** Only code, manually curated facts and runtime
audit metadata are retained.

## Latest verified Windows and Linux service API gates

- [Ubuntu network/IDL/ICD audit](https://github.com/nssang00/umaa-dev/actions/runs/40112466454): explicit EO 14/21 Topic mapping PASS, generated reviewed service C++ contracts 14/21, 352/352 file inventory, DDS CTest 4/4 and 8 isolated process scenarios PASS.
- [Windows native official DDS + reviewed Handler APIs](https://github.com/nssang00/umaa-dev/actions/runs/40112466468): official reviewed C++ TypeSupport generation 17 additional Topics, 14-service/21-Topic API compilation and CTest 1/1, official DDS Battery/Engine CTest 3/3 PASS.
- [Linux GCC/Clang and Windows MSVC baseline tests](https://github.com/nssang00/umaa-dev/actions/runs/40112466451): all three jobs PASS. **No release/compliance claim** is implied by this scoped CI success.
