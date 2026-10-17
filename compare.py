#!/usr/bin/env python3
"""Cross-measure independent readings of the git.pp spec.

The claim being tested is NOT "which fold is right". It is:

    for each clause, do independent readings of the SAME TEXT produce
    the same output, and where they do not, which clause is responsible?

Agreement across readers bounds how much of the spec is load-bearing.
Disagreement, located precisely, is the finding.

Each reader is a subprocess emitting one JSON object:
    {"state": {...}, "order": [...], "conflicts": [...], "orphans": [...]}
Normalisation is deliberately MINIMAL. We compare semantics, not bytes --
byte-level agreement is a separate (and already separately-tested) question.
"""
import json, subprocess, sys, os, collections, shutil

HERE = os.path.dirname(os.path.abspath(__file__))
PROBES = json.load(open(f"{HERE}/corpus/probes.json"))["probes"]

# reader name -> command that prints that JSON
READERS = json.load(open(f"{HERE}/readers.json"))

def run_reader(cmd, records):
    # Resolve the reader's own path to absolute. A relative program path
    # works for python3/node (which resolve it) but NOT for a bare C
    # binary invoked through an adapter, and the failure surfaced as
    # ERROR on every probe -- which read as "the readers disagree" when
    # in fact the harness could not launch one of them.
    cmd = [os.path.abspath(c) if (os.path.sep in c and os.path.exists(c)) else c for c in cmd]
    payload = json.dumps(records, ensure_ascii=False)
    try:
        r = subprocess.run(cmd, input=payload, capture_output=True, text=True, timeout=60)
    except subprocess.TimeoutExpired:
        return {"__error__": "timeout"}
    if r.returncode != 0:
        return {"__error__": (r.stderr or r.stdout or "?").strip().splitlines()[-1][:120]}
    try:
        return json.loads(r.stdout)
    except Exception:
        return {"__error__": "unparseable: " + (r.stdout or "")[:80]}

def canon(o):
    """Semantic normalisation: key order is not a fold decision."""
    if isinstance(o, dict):
        return {k: canon(o[k]) for k in sorted(o)}
    if isinstance(o, list):
        return [canon(x) for x in o]
    return o

def fingerprint(res):
    if "__error__" in res:
        return "ERROR"
    return json.dumps({
        "state": canon(res.get("state", {})),
        "order": sorted(res.get("order", [])),
        "conflicts": sorted([canon(c) for c in res.get("conflicts", [])], key=lambda c: json.dumps(c, sort_keys=True)),
        "orphans": sorted(res.get("orphans", [])),
    }, sort_keys=True)

def main():
    names = sorted(READERS)
    present, absent = [], []
    for n in names:
        cmd = READERS[n]["cmd"]
        exe = cmd[0] if not os.path.isabs(cmd[0]) else cmd[0]
        if READERS[n].get("available", True) and (shutil.which(exe) or os.path.exists(exe)):
            present.append(n)
        else:
            absent.append(n)
    print(f"  readers present: {', '.join(present) or '(none)'}")
    if absent:
        # An absent reader is NOT agreement. Counting a missing port as a
        # pass is the false-green class this whole repo exists to kill:
        # /tmp is ephemeral, so a reader can vanish between runs and a
        # naive harness reports unanimity. Absence is reported as absence.
        print(f"  readers ABSENT : {', '.join(absent)}")
        print(f"  -> NOT counted as agreement. A missing reader is missing, not passing.")
    if len(present) < 2:
        print("  need at least two readers to compare"); return

    disagreements = collections.defaultdict(list)
    total = 0
    agree = 0
    for p in PROBES:
        fps = {}
        for n in present:
            fps[n] = fingerprint(run_reader(READERS[n]["cmd"], p["records"]))
        total += 1
        if len(set(fps.values())) == 1:
            agree += 1
        else:
            groups = collections.defaultdict(list)
            for n, f in fps.items():
                groups[f].append(n)
            disagreements[p["id"]] = {
                "ambiguity": p["ambiguity"],
                "declared_split": p.get("split", ""),
                "groups": [{"readers": g, "output": (list(groups.keys())[list(groups.keys()).index(f)] if False else f)[:200]}
                           for f, g in groups.items()],
            }

    print(f"\n  {agree}/{total} probes: all PRESENT readers agree"
          + (f"  ({len(present)}/{len(names)} readers present)" if len(present)!=len(names) else ""))
    if disagreements:
        print(f"  {len(disagreements)} probe(s) DISAGREE — these are the underspecified clauses\n")
        for pid, d in disagreements.items():
            print(f"  {pid}")
            print(f"    ambiguity : {d['ambiguity'][:96]}")
            print(f"    declared  : {d['declared_split'][:96]}")
            for g in d["groups"]:
                print(f"    [{','.join(g['readers'])}]")
                print(f"       {g['output'][:150]}")
            print()
    json.dump({"agree": agree, "total": total,
               "disagreements": {k: {kk: vv for kk, vv in v.items() if kk != "groups"}
                                | {"groups": v["groups"]} for k, v in disagreements.items()}},
              open(f"{HERE}/result.json", "w"), indent=1, default=str)

main()
