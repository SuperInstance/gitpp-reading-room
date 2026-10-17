#!/usr/bin/env python3
"""Render result.json as a readable disagreement report.

The output is a CLAIM about the spec, not about any implementation:
  "N of M probes are determined by the text; the rest are not."
"""
import json, os, sys
HERE = os.path.dirname(os.path.abspath(__file__))
r = json.load(open(f"{HERE}/result.json"))
print("=" * 72)
print("  gitpp-reading-room — cross-reading report")
print("=" * 72)
print(f"  determined by the spec : {r['agree']} / {r['total']} probes")
print(f"  UNDERDETERMINED        : {len(r['disagreements'])} / {r['total']} probes")
print()
if not r["disagreements"]:
    print("  Every reader produced the same output on every probe.")
    sys.exit(0)
for pid, d in r["disagreements"].items():
    print(f"  {pid}")
    print(f"    clause    : {d['ambiguity']}")
    if d.get("declared_split"):
        print(f"    expected  : {d['declared_split']}")
    for g in d["groups"]:
        print(f"    [{', '.join(g['readers'])}]")
        print(f"       {g['output'][:150]}")
    print()
print("=" * 72)
print("  A disagreement here is not a bug report. It is a clause the text")
print("  does not determine -- two careful readers shipped different systems")
print("  and neither can say the other is wrong.")
print("=" * 72)
