#!/usr/bin/env python3
"""Run the fold corpus through three independently written readers and
group disagreements BY SHAPE.

Three things this must get right, all learned the hard way in this repo:

  1. Group by SHAPE. An index says "set 0441 disagrees"; a shape says
     "the orphan rule is wrong for every cycle-descendant set", which is
     a finding about a clause.

  2. A set where EVERY reader errored is a HARNESS failure, not a
     disagreement. Counting it as a split is the exact false-green
     pattern this project keeps finding: an absent result read as a
     result. Those sets are excluded and counted separately.

  3. A set where ONE reader errored while others succeeded is a real
     finding, and is reported as such.
"""
import json, subprocess, sys, collections

READERS = {
  "reader1-py": ["python3", "/tmp/rr2/readers/py/cli.py"],
  "reader3-c":  ["python3", "/tmp/rr2/readers/c/cli.py"],
  "reader4-pl": ["perl", "/tmp/rr2/readers/perl/fold.pl", "--cli"],
}
FIELDS = ("state", "order", "conflicts", "orphans")

def norm(v):
    if isinstance(v, dict): return {k: norm(v[k]) for k in sorted(v)}
    if isinstance(v, list): return [norm(x) for x in v]
    if isinstance(v, float) and v.is_integer(): return int(v)
    return v

def run(cmd, recs):
    try:
        r = subprocess.run(cmd, input=json.dumps(recs), capture_output=True,
                           text=True, timeout=60)
        if r.returncode != 0: return {"__e": 1}
        return json.loads(r.stdout)
    except Exception:
        return {"__e": 1}

def main():
    sets = json.load(open(sys.argv[1]))
    st = collections.defaultdict(lambda: {"agree": 0, "split": 0, "ex": None})
    allerr = collections.Counter()
    fieldcount = collections.Counter()
    partial = 0

    for s in sets:
        outs = {n: run(c, s["records"]) for n, c in READERS.items()}
        errs = [n for n, o in outs.items() if "__e" in o]
        if len(errs) == len(outs):
            allerr[s["shape"]] += 1
            continue
        if errs:
            partial += 1
        fps = {}
        for n, o in outs.items():
            if "__e" in o: continue
            fps[n] = json.dumps({f: norm(o.get(f)) for f in FIELDS}, sort_keys=True)
        for f in FIELDS:
            vals = {json.dumps(norm(o.get(f)), sort_keys=True)
                    for o in outs.values() if "__e" not in o}
            if len(vals) > 1: fieldcount[f] += 1
        if len(set(fps.values())) == 1:
            st[s["shape"]]["agree"] += 1
        else:
            st[s["shape"]]["split"] += 1
            if st[s["shape"]]["ex"] is None:
                st[s["shape"]]["ex"] = (s["id"], fps)

    print(f"  {len(sets)} sets x {len(READERS)} readers")
    print(f"  excluded (ALL readers errored): {sum(allerr.values())}")
    print(f"  partial (one reader errored):    {partial}")
    print()
    A = S = 0
    for sh in sorted(st):
        d = st[sh]; A += d["agree"]; S += d["split"]
        tot = d["agree"] + d["split"]
        rate = d["split"] / tot if tot else 0
        flag = "   <-- DISAGREES" if rate > 0.5 else ""
        print(f"  {sh:<26} agree {d['agree']:>3}  split {d['split']:>3}  ({rate:>3.0%}){flag}")
    print(f"\n  {A} agree, {S} disagree")
    print()
    print("  which FIELD differs across the splits:")
    for f, c in fieldcount.most_common(): print(f"    {f:<12} {c}")
    print()
    for sh in sorted(st):
        if st[sh]["ex"]:
            sid, fps = st[sh]["ex"]
            print(f"  {sh}  (e.g. {sid}):")
            for k, v in fps.items(): print(f"    {k:<12} {v[:130]}")
            print()

if __name__ == "__main__":
    main()
