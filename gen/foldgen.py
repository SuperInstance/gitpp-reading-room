#!/usr/bin/env python3
"""A corpus generator for the FOLD, covering the record space the leaf
corpus structurally cannot.

FINDING-14 measured the gap: the 20,043-record leaf corpus contains
ZERO records with a parent, zero with kind:null, and zero with an
unrecognised kind. Its generator emits leaves. Three of the fold's four
clauses -- 5.2 per-key resolution, 5.4 order, 5.5 orphans -- are about
records that HAVE parents, so none of them is reachable from that corpus.

This generator is deliberately the complement rather than a replacement.
The leaf harness stays a leaf harness; widening it would blur what it
proves. This produces record SETS for the fold, where the interesting
behaviour is a property of the set and not of any single record.

The shapes, chosen from the spec rather than from imagination:

  kind       override | task | witness | null | unrecognised
  parents    none | single | chain | diamond | fan-out | CYCLE | MISSING
  values     the four number bands, string twins (1e20 vs "1e20"),
             bool, nested object, array
  ids        ASCII | non-ASCII | non-BMP | empty-ish

Every generated set is emitted with the shape it is meant to exercise,
so a failure names its own class rather than an index.
"""
import json, random, sys

KINDS = ["override", "task", "witness", None, "weird-kind"]
KINDS_SPEC = {"override", "task", "witness"}

def rval(rng):
    """A value, weighted toward the regimes the leaf corpus already
    covers, with the ones it misses represented too."""
    k = rng.randrange(10)
    if k < 3:  return rng.choice([1, 2, -3, 0, 1.5, 0.1])
    if k == 3: return rng.choice([1e-5, 2.5e-7, -1e-5])       # sub-1e-4 band
    if k == 4: return rng.choice([1e16, 2e16, 1.0000000000000002e16, 1e20])  # sci band
    if k == 5: return rng.choice(["1e20", "1e16", "0", "true"])   # string twins
    if k == 6: return rng.choice([True, False])
    if k == 7: return {"nested": rng.randrange(3), "deep": {"z": 1}}
    if k == 8: return [rng.randrange(4) for _ in range(rng.randrange(4))]
    return None

def rec(i, kind, parents, effects, rid=None):
    return {"id": rid if rid is not None else f"r{i:04d}", "kind": kind,
            "parents": parents, "effects": effects, "reason": None}

def gen_set(rng, n, shape):
    """One record set of the requested SHAPE. Returns (records, label)."""
    ids = [f"r{i:04d}" for i in range(n)]
    kinds = [rng.choice(KINDS) for _ in range(n)]
    pars = [[] for _ in range(n)]

    if shape == "chain":
        for i in range(1, n): pars[i] = [ids[i-1]]
    elif shape == "diamond":
        mid = n // 2
        for i in range(1, mid): pars[i] = [ids[i-1]]
        if mid + 1 < n: pars[mid+1] = [ids[1], ids[mid-1]]
        for i in range(mid+2, n): pars[i] = [ids[mid+1]]
    elif shape == "fanout":
        for i in range(1, n): pars[i] = [ids[0]]
    elif shape == "cycle":
        if n >= 2: pars[0] = [ids[-1]]; pars[1] = [ids[0]]
    elif shape == "cycle-descendant":
        if n >= 3: pars[0] = [ids[1]]; pars[1] = [ids[0]]; pars[2] = [ids[0]]
    elif shape == "missing-parent":
        pars[0] = ["ghost-" + ids[0]]
    elif shape == "missing-parent-descendant":
        pars[0] = ["ghost-" + ids[0]]
        if n > 1: pars[1] = [ids[0]]
    elif shape == "deep":
        for i in range(1, n): pars[i] = [ids[i-1]]
    elif shape == "roots":
        pass
    elif shape == "unorderable":
        # two Kahn tiers where a later-tier id sorts BEFORE a ready peer:
        # the only shape that separates 5.4's two readings
        pass

    recs = [rec(i, kinds[i], pars[i], {"k%d" % j: rval(rng)
                                       for j in range(rng.randrange(1, 4))})
            for i in range(n)]

    if shape == "unorderable" and n >= 3:
        # ids chosen so 'b' is ready and 'a' becomes ready only after b
        recs = [rec(0, "task", [],     {"x": 1}, rid="b"),
                rec(1, "task", ["b"], {"x": 2}, rid="a"),
                rec(2, "task", [],     {},       rid="c")]
    if shape == "unorderable-id" and n >= 3:
        recs = [rec(0, "task", [],   {"x": 1}, rid="m"),
                rec(1, "task", [],   {"x": 2}, rid="z"),
                rec(2, "task", ["m"], {"x": 3}, rid="mm")]
    return recs, shape

SHAPES = ["roots", "chain", "deep", "diamond", "fanout", "cycle",
          "cycle-descendant", "missing-parent", "missing-parent-descendant",
          "unorderable", "unorderable-id"]

def main():
    seed = int(sys.argv[1]) if len(sys.argv) > 1 else 20261017
    count = int(sys.argv[2]) if len(sys.argv) > 2 else 2000
    rng = random.Random(seed)
    out = []
    for i in range(count):
        shape = SHAPES[i % len(SHAPES)]
        n = rng.randrange(2, 9)
        recs, sh = gen_set(rng, n, shape)
        out.append({"id": f"set-{i:05d}", "shape": sh, "records": recs})
    json.dump(out, open(sys.argv[3] if len(sys.argv) > 3 else "/tmp/foldcorpus.json", "w"))
    from collections import Counter
    c = Counter(s["shape"] for s in out)
    print(f"  {len(out)} record sets, seed {seed}")
    for k, v in c.most_common(): print(f"    {k:<26} {v}")

if __name__ == "__main__":
    main()
