#!/usr/bin/env python3
"""Run the fold corpus through every reader and group disagreements BY
SHAPE, not by index.

Grouping by shape is the point. An index says "set 0441 disagrees";
a shape says "the two-clause orphan rule is wrong for every
cycle-descendant set", which is a finding about a clause.
"""
import json, subprocess, sys, collections

READERS = {
  "reader1-py": ["python3", "/tmp/rr2/readers/py/cli.py"],
  "reader3-c":  ["python3", "/tmp/rr2/readers/c/cli.py"],
  "reader4-pl": ["perl", "/tmp/rr2/readers/perl/fold.pl", "--cli"],
}
FIELDS = ("state", "order", "conflicts", "orphans")

def run(cmd, recs):
    r = subprocess.run(cmd, input=json.dumps(recs), capture_output=True, text=True, timeout=90)
    if r.returncode != 0: return {"__error__": (r.stderr or "").strip()[:60]}
    try: return json.loads(r.stdout)
    except Exception: return {"__error__": "unparseable"}

def norm(v):
    if isinstance(v, dict): return {k: norm(v[k]) for k in sorted(v)}
    if isinstance(v, list): return [norm(x) for x in v]
    if isinstance(v, float) and v.is_integer(): return int(v)
    return v

def fp(res):
    if "__error__" in res: return "ERR:" + res["__error__"][:40]
    return json.dumps({f: norm(res.get(f)) for f in FIELDS}, sort_keys=True)

if __name__ == "__main__":
    sets = json.load(open(sys.argv[1] if len(sys.argv) > 1 else "/tmp/foldcorpus.json"))
    only = sys.argv[2].split(",") if len(sys.argv) > 2 else list(READERS)
    R = {k: v for k, v in READERS.items() if k in only}
    print(f"  {len(sets)} sets, readers: {', '.join(R)}")
    by_shape = collections.defaultdict(lambda: {"agree": 0, "split": 0, "example": None, "groups": collections.Counter()})
    for s in sets:
        outs = {n: fp(run(cmd, s["records"])) for n, cmd in R.items()}
        sh = s["shape"]
        if len(set(outs.values())) == 1:
            by_shape[sh]["agree"] += 1
        else:
            by_shape[sh]["split"] += 1
            if by_shape[sh]["example"] is None:
                by_shape[sh]["example"] = (s["id"], outs)
            by_shape[sh]["groups"][json.dumps({k: v[:60] for k, v in outs.items()}, sort_keys=True)] += 1
    print()
    tot_a = tot_s = 0
    for sh in sorted(by_shape):
        d = by_shape[sh]; tot_a += d["agree"]; tot_s += d["split"]
        rate = d["split"] / (d["agree"] + d["split"])
        flag = "  <-- DISAGREES" if rate > 0.5 else ("  <-- mixed" if rate > 0 else "")
        print(f"  {sh:<26} agree {d['agree']:>4}  split {d['split']:>4}  ({rate:.0%}){flag}")
    print(f"\n  {tot_a + tot_s} sets: {tot_a} agree, {tot_s} disagree")
    print()
    for sh in sorted(by_shape):
        d = by_shape[sh]
        if d["split"]:
            sid, outs = d["example"]
            print(f"  {sh} (e.g. {sid}):")
            for k, v in outs.items(): print(f"    {k:<12} {v[:120]}")
            print()
