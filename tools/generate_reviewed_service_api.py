#!/usr/bin/env python3
"""Generate reviewed UMAA EO C++ service contracts using official Cyclone IDL C types.

No guessed wire methods, QoS, or application logic. The official generated
TypeSupport headers supply every payload field (including optional fields).
Reviewing the service catalog is a human ICD task, not a filename heuristic.
"""
import argparse
import json
from pathlib import Path
import sys
from verify_official_service_types import inspect, type_idl_path


def generate(catalog, generated_roots, dest):
    # A catalog is only a scoped subset; never silently imply complete service coverage.
    check = inspect(catalog)
    if check["status"] != "PASS":
        raise ValueError("service manifest invalid: " + repr(check["errors"]))
    if catalog.get("complete") is True:
        raise ValueError("complete catalog must use a separately audited generation gate")
    required = []
    source = ["// GENERATED: reviewed UMAA 6.0 service slice; NOT full conformance.",
              "#pragma once", "#include <cstdint>", "#include <memory>"]
    chunks = []
    for svc in catalog["services"]:
        name = svc["name"].split("::")[-1]
        cname = name + "Api"
        role_types = {}
        for item in svc["provider_topics"] + svc["consumer_topics"]:
            role, type_name = item["role"], item["type"]
            header = type_idl_path(type_name).with_suffix(".h").as_posix()
            found = next((root / header for root in generated_roots if (root / header).is_file()), None)
            if not found:
                raise ValueError("official generated header missing for " + type_name)
            role_types[role] = (header, type_name.replace("::", "_"))
            required.append(header)
        if svc["pattern"] == "command_response":
            if set(role_types) != {"command", "ack", "status"}:
                raise ValueError(name + " missing validated protocol roles")
            lines = [f"struct {cname} {{",
                     f"  using Command = {role_types['command'][1]};",
                     f"  using Ack = {role_types['ack'][1]};",
                     f"  using Status = {role_types['status'][1]};",
                     "  class Handler {",
                     "   public:",
                     "    virtual ~Handler() = default;",
                     "    virtual bool validate(const Command&) = 0;",
                     "    virtual bool execute(const Command&) = 0;",
                     "    virtual bool cancel(const Command&) = 0;",
                     "  };",
                     "};"]
        elif svc["pattern"] == "request_reply_without_query_data":
            if set(role_types) != {"report"}:
                raise ValueError(name + " must use report subscription only")
            lines = [f"struct {cname} {{",
                     f"  using Report = {role_types['report'][1]};",
                     "  class Handler {",
                     "   public:",
                     "    virtual ~Handler() = default;",
                     "    virtual bool read_report(Report&) const = 0;",
                     "  };",
                     "};"]
        else:
            raise ValueError("unsupported binding pattern: " + svc["pattern"])
        chunks.extend(lines)
    source.extend('#include "' + header + '"' for header in sorted(set(required)))
    source.append("namespace umaa::generated {")
    source.extend(chunks)
    source.append("} // namespace umaa::generated")
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text("\n".join(source) + "\n", encoding="utf-8")
    return len(catalog["services"]), len(set(required))


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--catalog", required=True, type=Path)
    ap.add_argument("--generated-root", required=True, action="append", type=Path)
    ap.add_argument("--out", required=True, type=Path)
    args = ap.parse_args(argv)
    try:
        doc = json.loads(args.catalog.read_text(encoding="utf-8"))
        count, types = generate(doc, args.generated_root, args.out)
    except (ValueError, OSError) as exc:
        print("OFFICIAL_BINDINGS_FAIL: " + str(exc), file=sys.stderr)
        return 2
    print(f"OFFICIAL_BINDINGS_GENERATED: reviewed_services={count} official_types={types} path={args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
