#!/usr/bin/env python3
"""Adjudicate §5.3's losers-list ordering, behind the calibration gate.

The spec says 'one winner and N-1 losers' and 'every candidate that set
K with a value other than the winner's'. Both describe a set. Three
ports emit three orders and the spec does not choose between them.

The question is narrow and is NOT 'what order would you pick'. It is
'does the text determine the order', and if it does not, that is the
finding.
"""
import json, os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from judge import jev

STATE = {
  "spec_5_2": ("For each key K, the conflict ledger records the final winner (the setter at the end of the fold) "
               "and all losers (every record that tried to set K with a value other than the winner's). When the "
               "streaming model is used, the conflict ledger has one entry per key, with one winner and N-1 losers."),
  "spec_5_3": ("If 2+ distinct values exist for K across the candidates, emit a conflict: {key, winner, losers} "
               "where losers is every candidate that set K with a value other than the winner's. The losers are "
               "all candidates with a different value, not just the within-class tier."),
  "spec_5_4": ("Order: Kahn's algorithm with smallest-id-first. Ties broken by Unicode code point."),
}

TESTS = {
 "losers-order": {
   "q": "In what ORDER does the conflict ledger list the losers?",
   "A_undetermined": ("The text DOES NOT DETERMINE the order. It says the ledger has "
     "'one winner and N-1 losers' and that losers is 'every candidate that set K with a value other than the "
     "winner's'. That fixes the MEMBERSHIP of the set and says nothing about the order in which they are written."),
   "B_in_order": ("The losers are listed in the fold's ORDER sequence, because the fold already computes a total "
     "order and the streaming model displaces each loser in that sequence."),
 },
 "is-a-ledger-set": {
   "q": "Is the losers field a SET rather than a sequence?",
   "A_yes": ("YES. The text describes it by MEMBERSHIP -- 'every record that tried to set K with a value other "
     "than the winner's' and 'N-1 losers' -- which is set language, and it never says the list is ordered."),
   "B_no": ("NO. A JSON array is inherently ordered, so listing the members implies an order."),
 },
 "does-order-field-govern": {
   "q": "Does the spec's separate `order` field govern the order of the losers list too?",
   "A_yes": ("YES. The fold produces a total order over all records; the conflict ledger is a projection of the "
     "fold, and a projection of an ordered structure is ordered by that structure."),
   "B_no": ("NO. The `order` field describes processing sequence and says nothing about how a ledger entry is "
     "serialised."),
 },
}

if __name__ == "__main__":
    if "--calibrate" in sys.argv:
        ctl = [("true", {"s": "An orphan is a record whose parent id is absent. Example: ghost."}, "The text uses the identifier ghost."),
               ("false", {"s": "An orphan is a record whose parent id is absent. Example: ghost."}, "The text uses the identifier zzz-9."),
               ("scope", {"s": "An orphan is a record whose parent id is absent. Example: ghost."}, "Water boils at 100 C at sea level.")]
        r = {}
        for n, st, cl in ctl:
            r[n] = jev(st, f"{cl}\n\nAnswer true if the text above supports this statement. If the text does not settle it, do not guess.")
            print(f"  {n:<8} {r[n]:.3f}")
        ok = (r["true"] - r["false"]) > 0.3 and r["false"] < r["scope"] < r["true"]
        print(f"  separation {r['true']-r['false']:+.3f}  CALIBRATED: {ok}")
        sys.exit(0 if ok else 1)
    out = {}
    for name, t in TESTS.items():
        out[name] = {}
        for k, claim in t.items():
            if k == "q": continue
            s = jev(STATE, f"{claim}\n\nQuestion: {t['q']}\n\nAnswer true if the specification text above supports this statement. If the text does not settle it, do not guess.")
            out[name][k] = s
            print(f"  {name:<26} {k:<18} {s:.3f}", flush=True)
        print()
    json.dump(out, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "losers-order-verdicts.json"), "w"), indent=1)
