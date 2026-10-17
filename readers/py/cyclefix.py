#!/usr/bin/env python3
"""Correct orphan rule: cycles AND their descendants, but NOT descendants
of a MISSING parent.

First attempt at this file proposed "orphan = any parent missing, OR any
parent is itself an orphan" and it was WRONG: a cycle member's parents
are all present, so the rule never fires on a cycle at all. It scored
U==A in 11/12 cases, which is the tell -- a rule that agrees with the
rule it was supposed to improve on is not an improvement.

The correct rule needs three clauses, not two:

  1. any parent id is missing from the applied set        (missing-parent)
  2. the record cannot be topologically ordered           (cycle member)
  3. any parent cannot be topologically ordered           (cycle descendant)

Clause 3 is what distinguishes a cycle descendant from a missing-parent
descendant: the former's parent EXISTS but is unorderable, the latter's
parent does not exist. A rule keyed on existence alone cannot tell them
apart, which is why every existing port gets exactly one of the two
cases wrong.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fold import derive, cp

def _orderable(by):
    """Kahn, in-degree counting only PARENTS THAT EXIST. Returns the set of
    records that can be topologically ordered."""
    emitted, pool = set(), set(by)
    while True:
        ready = sorted([k for k in pool
                        if all(p in emitted for p in (by[k].get("parents") or [])
                               if p in by)], key=cp)
        if not ready:
            break
        emitted.add(ready[0]); pool.discard(ready[0])
    return emitted

def rule_A(recs):
    """any parent missing from the applied set"""
    by = {r["id"]: r for r in recs}
    return {r["id"] for r in recs if any(p not in by for p in (r.get("parents") or []))}

def rule_B(recs):
    """anything Kahn never emitted, in-degree counting ALL parents"""
    by = {r["id"]: r for r in recs}
    emitted, pool = set(), set(by)
    while True:
        ready = sorted([k for k in pool
                        if all((p not in by) or (p in emitted)
                               for p in (by[k].get("parents") or []))], key=cp)
        if not ready: break
        emitted.add(ready[0]); pool.discard(ready[0])
    return set(by) - emitted

def rule_U3(recs):
    """the three-clause rule"""
    by = {r["id"]: r for r in recs}
    ordered = _orderable(by)
    out = set()
    for k, r in by.items():
        ps = r.get("parents") or []
        if any(p not in by for p in ps):        # 1. missing parent
            out.add(k)
        elif k not in ordered:                  # 2. cycle member
            out.add(k)
        elif any(p not in ordered for p in ps if p in by):   # 3. cycle descendant
            out.add(k)
    return out

R = lambda i, p, e: {"id": i, "kind": "task", "parents": p, "effects": e, "reason": None}

CASES = {
 "clean chain":         [R("a",[],{}), R("b",["a"],{"x":1})],
 "missing parent":      [R("a",["ghost"],{"x":1})],
 "desc of missing":     [R("orph",["ghost"],{"x":1}), R("kid",["orph"],{"y":2})],
 "2-cycle":             [R("ca",["cb"],{}), R("cb",["ca"],{})],
 "desc of 2-cycle":     [R("ca",["cb"],{}), R("cb",["ca"],{}), R("down",["cb"],{"y":3})],
 "desc^2 of 2-cycle":   [R("ca",["cb"],{}), R("cb",["ca"],{}), R("d1",["cb"],{}), R("d2",["d1"],{"y":4})],
 "cycle + root":        [R("ca",["cb"],{}), R("cb",["ca"],{}), R("ok",[],{"z":5})],
 "two cycles":          [R("a",["b"],{}), R("b",["a"],{}), R("c",["d"],{}), R("d",["c"],{})],
 "self-loop":           [R("a",["a"],{"x":6})],
 "desc of self-loop":   [R("a",["a"],{}), R("k",["a"],{"y":7})],
 "diamond":             [R("r",[],{}), R("l",["r"],{}), R("m",["r"],{}), R("j",["l","m"],{"x":8})],
 "cycle + branch":      [R("a",["b"],{}), R("b",["a"],{}), R("t",["r"],{}), R("r",["t"],{}), R("leaf",["r"],{"x":9})],
 "missing AND cycle":   [R("ca",["cb"],{}), R("cb",["ca"],{}), R("m",["ghost"],{"q":1}), R("mk",["m"],{"w":2})],
}

if __name__ == "__main__":
    print("  %-22s %-18s %-22s %-24s %s" % ("case","A missing","B kahn","U3 three-clause","note"))
    print("  " + "-"*112)
    ua = ub = 0
    for name, recs in CASES.items():
        a, b, u = sorted(rule_A(recs)), sorted(rule_B(recs)), sorted(rule_U3(recs))
        note = []
        if a == u: ua += 1
        if b == u: ub += 1
        if a != b: note.append("A/B split")
        print("  %-22s %-18s %-22s %-24s %s" % (name, a, b, u, "; ".join(note)))
    print()
    print("  U3 == A in %d/%d, U3 == B in %d/%d" % (ua,len(CASES),ub,len(CASES)))
    print("  The target is: agree with A on 'desc of missing' ([orph] only),")
    print("  and with B on every cycle case. Both counts should be far from 100%%.")
    print()
    ok = (sorted(rule_U3(CASES["desc of missing"])) == ["orph"]
          and sorted(rule_U3(CASES["desc of 2-cycle"])) == ["ca","cb","down"]
          and sorted(rule_U3(CASES["desc^2 of 2-cycle"])) == ["ca","cb","d1","d2"]
          and sorted(rule_U3(CASES["cycle + branch"])) == ["a","b","leaf","r","t"])
    print("  U3 satisfies the spec on every discriminating case: %s" % ok)
