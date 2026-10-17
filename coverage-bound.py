#!/usr/bin/env python3
"""How much of the RECORD SPACE does the corpus actually cover?

FINDING-13 established that the ports are independent, so their
agreement is real evidence. It also named the limit: agreement is only
evidence about the cases EXERCISED, and the corpus comes from ONE
generator. Independence of implementations does nothing about
independence of inputs.

This measures the input side. The record space is not a continuum, it
is a product of a few factors, and each factor has regimes the generator
either covers deliberately or misses. For each factor this reports what
the corpus contains and what the spec makes legal, so the gap is a
number rather than a feeling.

The factors come from the spec itself:
  kind       three classes + null + unrecognised
  effects    scalars, nested objects, arrays, mixed
  values     numbers across the 3 encoding bands, strings, bools
  ids        ASCII, non-BMP, punctuation, empty
  parents    none, chains, diamonds, cycles, MISSING
  depth      1 .. n
"""
import json, re, collections, os, sys

CORPUS = sys.argv[1] if len(sys.argv) > 1 else "/tmp/gitpp-fuzz/records.json"

def load():
    d = json.load(open(CORPUS))
    return d if isinstance(d, list) else d.get("records") or d.get("cases")

def classify(r):
    tags = set()
    k = r.get("kind", None)
    tags.add("kind:" + ("null" if k is None else
                        "known" if k in ("override","task","witness") else "unrecognised"))
    fx = r.get("effects") or {}
    for v in fx.values():
        if isinstance(v, bool): tags.add("value:bool")
        elif isinstance(v, (int, float)):
            a = abs(v)
            tags.add("value:int-band" if a < 1e-4 else
                     "value:sci-band" if a >= 1e16 else "value:plain")
        elif isinstance(v, str): tags.add("value:string")
        elif isinstance(v, list): tags.add("value:array")
        elif isinstance(v, dict): tags.add("value:object")
        else: tags.add("value:" + type(v).__name__)
    if not fx: tags.add("effects:EMPTY")
    if isinstance(fx, dict) and any(isinstance(v, dict) for v in fx.values()): tags.add("effects:NESTED")
    i = r.get("id", "")
    if not i: tags.add("id:EMPTY")
    elif max((ord(c) for c in i), default=0) > 0x7e: tags.add("id:NON-ASCII")
    elif max((ord(c) for c in i), default=0) > 0xffff: tags.add("id:NON-BMP")
    ps = r.get("parents") or []
    tags.add("parents:none" if not ps else f"parents:{min(len(ps),4)}{'+' if len(ps)>4 else ''}")
    return tags

if __name__ == "__main__":
    recs = load()
    print(f"  corpus: {len(recs)} records from {os.path.basename(CORPUS)}")
    counts = collections.Counter()
    for r in recs: counts.update(classify(r))
    by = collections.defaultdict(dict)
    for k, v in sorted(counts.items()):
        fam, _, val = k.partition(":")
        by[fam][val] = v
    # what the spec makes LEGAL
    LEGAL = {
      "kind": {"known", "null", "unrecognised"},
      "value": {"bool", "int-band", "sci-band", "plain", "string", "array", "object"},
      "parents": {"none", "parents:1", "parents:2", "parents:3", "parents:4+"},
      "depth": {"1", "2-3", "4+"},
      "id": {"EMPTY", "NON-ASCII", "NON-BMP"},
    }
    total_missing = 0
    for fam, vals in by.items():
        print(f"\n  {fam}:")
        legal = LEGAL.get(fam)
        for v, n in sorted(vals.items(), key=lambda kv: -kv[1]):
            mark = ""
            if legal is not None and v not in legal: mark = "   <-- not a regime the spec enumerates"
            print(f"    {v:<12} {n:>7}{mark}")
        if legal:
            missing = legal - set(vals)
            if missing:
                total_missing += len(missing)
                print(f"    MISSING: {sorted(missing)}")
    print()
    print(f"  regimes the spec makes legal and the corpus never generates: {total_missing}")
