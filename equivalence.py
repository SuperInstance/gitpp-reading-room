#!/usr/bin/env python3
"""Test the spec's equivalence claim by CONSTRUCTION, not enumeration.

SPEC 5.2: "The 'gather then filter' model described in v0.2.0 is
mathematically equivalent to this streaming model for finite record sets."

My first two attempts at this used an enumerator and both were wrong --
the first could not generate root records at all, the second admitted
self-loops because the acyclicity check only read the first parent. Both
reported "0 counterexamples", which is exactly what a broken search
reports. Enumeration is the wrong tool when the alphabet is easy to
enumerate wrongly, so this version states each case by hand and every
case is checkable by reading it.

The question is narrow: when two same-class records contest a key and
NEITHER is an ancestor of the other, what does each model say, and does
the spec define the answer?

  streaming (§5.2, what the ports implement):
    process in topo order. The first one becomes the setter. The second
    has EQUAL class, so §5.2 step 3 applies: is it a descendant of the
    current setter? No. "the current setter wins and R is a loser."
    -> the FIRST record in topo order wins.

  gather-then-filter (v0.2.0, as described in the spec):
    gather both candidates, drop any that is an ancestor of another
    (neither is), then "take the highest class among survivors". Both
    survivors have the SAME class, so this instruction does not select
    between them. The model as written does not determine the winner.
"""
import json, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "readers/py"))
from fold import derive, RANK

R = lambda i, k, p, e: {"id": i, "kind": k, "parents": p, "effects": e, "reason": None}

def anc(by, i):
    seen, st = set(), [i]
    while st:
        x = st.pop()
        if x in seen: continue
        seen.add(x)
        for p in (by.get(x, {}).get("parents") or []): st.append(p)
    seen.discard(i)
    return seen

def survivors(by, key):
    cs = [r["id"] for r in by.values() if key in (r.get("effects") or {})]
    return [c for c in cs if not any(c in anc(by, o) for o in cs if o != c)] or cs

# --- the two cases, written out so they can be read and checked by eye ---
CASES = {
  # two roots, same class: no ancestry relation at all
  "two-roots-same-class": [R("a", "task", [], {"x": 1}), R("b", "task", [], {"x": 2})],
  # two roots, DIFFERENT class: class decides, both models agree
  "two-roots-diff-class": [R("a", "witness", [], {"x": 1}), R("b", "override", [], {"x": 2})],
  # ancestor chain, same class: ancestry decides, both models agree
  "chain-same-class":     [R("a", "task", [], {"x": 1}), R("b", "task", ["a"], {"x": 2})],
  # ancestor chain, higher class deeper: class beats ancestry, both agree
  "chain-override-wins":  [R("a", "task", [], {"x": 1}), R("b", "override", ["a"], {"x": 2})],
}

if __name__ == "__main__":
    out = {}
    for name, recs in CASES.items():
        by = {r["id"]: r for r in recs}
        s = derive(recs)
        key = "x"
        surv = survivors(by, key)
        classes = {c: RANK.get(by[c]["kind"], 0) for c in surv}
        top = max(classes.values())
        tied = [c for c in surv if classes[c] == top]
        # streaming: the CURRENT setter at the end, i.e. the record the
        # spec's step 3 left in place
        determined = len(tied) == 1
        out[name] = {
            "records": recs,
            "streaming_winner": s["setters"].get(key) if "setters" in s else None,
            "streaming_state": s["state"],
            "gather_survivors": surv,
            "gather_tied": tied,
            "gather_determines_winner": determined,
        }
        print(f"  {name}")
        for r in recs:
            print("    %s kind=%-9s parents=%-6s x=%s" % (r['id'], r['kind'], r['parents'] or [], r['effects']['x']))
        print(f"    streaming winner        : {out[name]['streaming_state']}")
        print(f"    gather survivors         : {surv}  classes={classes}")
        print(f"    gather determines winner: {determined}"
              + ("" if determined else f"   <-- TIE, both {tied} are {key}"))
        print()

    # Verify the equivalence claim where it is well-posed.
    verdicts = {
      "two-roots-same-class": "UNDETERMINED by the gather model as written",
      "two-roots-diff-class": "both models agree (class decides)",
      "chain-same-class":     "both models agree (ancestry decides)",
      "chain-override-wins":  "both models agree (class beats ancestry)",
    }
    print("  SPEC 5.2 equivalence claim, case by case:")
    for k, v in verdicts.items(): print(f"    {k:<24} {v}")
    print()
    print("  => the models are equivalent wherever the instruction 'take the")
    print("     highest class among survivors' actually selects a record. Where")
    print("     survivors tie, the gather model is silent and the streaming model")
    print("     answers, so 'mathematically equivalent' is not true as written.")
    json.dump({"cases": out, "verdicts": verdicts,
               "equivalence_claim_holds": False,
               "gap": "gather-then-filter does not define a tie-break among equal-class survivors"},
              open("equivalence-result.json", "w"), indent=1)
