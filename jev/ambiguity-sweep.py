#!/usr/bin/env python3
"""Sweep the remaining DISAGREEING clauses through Jev, plus a control.

Only clauses where readers actually diverge are worth an adjudication --
agreeing clauses need no judge, and asking about them would be asking
Jev to grade homework. So this reads result.json and scores exactly the
probes that split, with the calibrated instrument from calibrate.py.

Each disputed clause gets:
  - one call per candidate reading, same state, symmetric framing
  - a verdict: which reading the TEXT supports, or 'undetermined'
  - a confidence in that verdict

The undetermined verdict is the important output. It names clauses that
need a rule written rather than a reading chosen.
"""
import json, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from judge import jev

# The clause text as it actually appears, not a paraphrase.
CLAUSE_TEXT = {
 "p01-class-vs-ancestry": {
   "spec_5_1": "Class precedence is absolute: override > task > witness. An override outranks a task, a task outranks a witness, EVEN WHEN THE WITNESS IS A DESCENDANT OF THE TASK.",
   "spec_5_2": "Per-key resolution, streaming model. Step 2: compare the candidate's class to the current setter's class using class precedence. Step 3: IF R's class is EQUAL, then R is a descendant of the current setter iff R is reachable from the current setter via the parent chain; if so R wins, if not the current setter wins.",
   "q": "An override record is an ANCESTOR of a task record and both set the same key. Which one sets the final value?"},
 "p07-orphan-descendant": {
   "spec_5_5": "An orphan is a record with a parent id that is not present. The record is excluded from the fold. An orphan's DESCENDANTS ARE NOT AUTO-ORPHANED: being descended from an orphan does not by itself make a record an orphan.",
   "q": "A record K descends from an orphan O. K is not itself an orphan. Do K's own effects enter the final state?"},
 "p10-id-validation": {
   "spec_1_1": "The id field: 'The unique identifier. Must be free of \\n and : (the closure root format uses these as delimiters).'",
   "spec_1_1_malformed": "A record missing any of id, parents, kind, effects, reason is malformed; the fold excludes malformed records from the applied set.",
   "q": "A record's id is 'a:b' -- it contains a colon, which section 1.1 says the id must be free of. Is that record malformed and excluded from the fold?"},
 "p12-kind-null": {
   "spec_1_1": "The kind field: 'One of task, witness, override. Other values are accepted; their semantics are out of scope for the fold.' kind is optional, defaulting to null.",
   "spec_5_1": "Class precedence is absolute: override > task > witness.",
   "spec_5_2": "Per-key resolution compares the candidate's class to the current setter's class. Every record that sets a key is a candidate.",
   "q": "A record has kind: null and sets a key that an override record also sets. What class does the null-kind record have, and which one wins?"},
}

READINGS = {
 "p01-class-vs-ancestry": {
   "A_class_absolute": "The OVERRIDE wins. Section 5.1 states class precedence is absolute and explicitly says it holds even against a descendant, and 5.2 step 3 applies ancestry ONLY when classes are equal.",
   "B_ancestry_first": "The TASK wins, because 5.2 says a descendant is applied after its ancestor and therefore wins, and 5.1's mention of descendants is a special case that generalises."},
 "p07-orphan-descendant": {
   "A_descendant_applies": "YES, K's effects enter the state. K is not an orphan, and 5.5 says explicitly that an orphan's descendants are not auto-orphaned.",
   "B_branch_excluded": "NO, K's effects are excluded. K's ancestry is broken, so the whole branch is dropped with the orphan."},
 "p10-id-validation": {
   "A_malformed_excluded": "YES, the record is malformed and excluded. Section 1.1 says an id MUST be free of colon, and says malformed records are excluded from the applied set.",
   "B_accepted_normally": "NO, the record is processed normally. The colon constraint is a documented convention for the closure-root delimiter; the exclusion rule in 1.1 lists only MISSING fields, not malformed ones."},
 "p12-kind-null": {
   "A_undetermined": "The spec DOES NOT DETERMINE it. kind defaults to null and other values are declared out of scope for the fold, yet 5.2 must rank every candidate by class. Nothing in the text says where an unclassifiable record ranks, so the text does not settle which wins.",
   "B_ranks_lowest": "The OVERRIDE wins, because a null kind has no class and therefore the lowest rank: the fold is permissive about what it accepts and strict about what it can order, so an unclassifiable record never displaces a classifiable one."},
}

if __name__ == "__main__":
    here = os.path.dirname(os.path.abspath(__file__))
    res = json.load(open(os.path.join(here, "..", "result.json")))
    disputed = set(res["disagreements"].keys())

    # map probe id -> clause key
    probe_to_clause = {
      "p01-ancestry-vs-class": "p01-class-vs-ancestry",
      "p02-witness-descendant-of-task": "p01-class-vs-ancestry",
      "p13-class-vs-ancestry-inversion": "p01-class-vs-ancestry",
      "p07-orphan-descendant": "p07-orphan-descendant",
      "p10-id-whitespace-and-separators": "p10-id-validation",
      "p12-kind-null": "p12-kind-null",
    }
    todo = sorted({probe_to_clause[p] for p in disputed if p in probe_to_clause})
    print(f"  disputed probes: {len(disputed)}  ->  distinct clauses to adjudicate: {len(todo)}")
    print()
    out = {}
    for cid in todo:
        c = CLAUSE_TEXT[cid]
        row = {"state": c, "scores": {}}
        for name, claim in READINGS[cid].items():
            q = f"{claim}\n\nQuestion: {c['q']}\n\nAnswer true if the specification text above supports this statement. If the text does not settle it, do not guess."
            row["scores"][name] = jev(c, q)
        best = max(row["scores"], key=lambda k: row["scores"][k])
        spread = row["scores"][best] - min(row["scores"].values())
        row["verdict"] = best if spread >= 0.25 else "UNDETERMINED"
        out[cid] = row
        for k, v in sorted(row["scores"].items(), key=lambda kv: -kv[1]):
            print(f"  {cid:<26} {k:<22} {v:.3f}")
        print(f"  {'':<26} -> {row['verdict']}  (spread {spread:+.3f})\n")
    json.dump(out, open(os.path.join(here, "sweep.json"), "w"), indent=1)
    print("  wrote sweep.json")
