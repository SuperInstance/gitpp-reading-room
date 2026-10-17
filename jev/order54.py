#!/usr/bin/env python3
"""Adjudicate SPEC 5.4's order clause behind the calibration gate.

'Kahn's algorithm with smallest-id-first.' Two implementations both
satisfy that phrase:

  SEQUENTIAL  repeatedly take the minimum of the CURRENT ready set and
              re-evaluate readiness after each emission. This is what a
              heap-based Kahn does.
  WHOLE-TIER  compute a tier, release and order the entire tier, then
              start the next tier.

They differ whenever emitting a node makes another ready whose id
sorts before an already-ready peer.
"""
import json, os, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from judge import jev

STATE = {
 "spec_5_4": ("SPEC 5.4 Order: Kahn's algorithm with smallest-id-first. Ties broken by "
   "Unicode code point. Orphan records (those with any missing parent) and cycle-affected "
   "records are excluded from the order."),
}

TESTS = {
 "which-implementation": {
   "q": ("Records m (no parents), z (no parents), and mm (parent m). "
         "Which order does the spec require?"),
   "A_sequential": ("[m, mm, z]. Taking the minimum of the current ready set and "
     "re-evaluating after each emission IS 'smallest-id-first': at the second step mm is "
     "ready and mm < z, so mm is the smallest-first remaining record."),
   "B_wholetier": ("[m, z, mm]. The first tier is {m, z} and a tier is ordered as a whole "
     "before the next tier begins, so mm is in the second tier regardless of its id."),
 },
 "is-a-heap-kahn-smallest-first": {
   "q": "Does a heap-based Kahn's algorithm satisfy 'smallest-id-first'?",
   "A_yes": ("YES. A heap-based Kahn always removes the minimum of the current ready set, "
     "so every emission is the smallest-id-first available record."),
   "B_no": ("NO. A heap-based Kahn interleaves tiers, so its output is not ordered "
     "tier-by-tier, and 'smallest-id-first' is intended to mean tier-by-tier."),
 },
}

if __name__ == "__main__":
    if "--calibrate" in sys.argv:
        ctl = [("true", {"s": "An orphan is a record whose parent id is absent. Example: ghost."},
                "The text uses the identifier ghost."),
               ("false", {"s": "An orphan is a record whose parent id is absent. Example: ghost."},
                "The text uses the identifier zzz-9."),
               ("scope", {"s": "An orphan is a record whose parent id is absent. Example: ghost."},
                "Water boils at 100 C at sea level.")]
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
        for k in ("A_sequential", "B_wholetier", "A_yes", "B_no"):
            if k not in t: continue
            s = jev(STATE, f"{t[k]}\n\nQuestion: {t['q']}\n\nAnswer true if the specification text above supports this statement. If the text does not settle it, do not guess.")
            out[name][k] = s
            print(f"  {name:<32} {k:<16} {s:.3f}", flush=True)
        print()
    json.dump(out, open("order54-verdicts.json", "w"), indent=1)
