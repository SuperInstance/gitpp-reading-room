#!/usr/bin/env python3
"""Negative controls for the Jev adjudicator.

An instrument that answers every question confidently measures nothing.
Before any Jev score is used as evidence, the instrument has to be shown
to SEPARATE cases with known answers. Three controls, all with ground
truth that does not depend on anyone's judgement of the spec:

  C1  TRUE-STATED   an unambiguous fact stated plainly in the state.
                    A calibrated reader must score it HIGH.
  C2  FALSE-STATED  a claim directly CONTRADICTED by the state text.
                    Must score LOW. This is the important one: an
                    instrument that cannot say "no" cannot adjudicate.
  C3  OUT-OF-SCOPE  a true claim about something the state never mentions.
                    Must land in the MIDDLE, not high. A reader that
                    scores true-but-unmentioned high is pattern-matching
                    on plausibility, not reading the text.

If C1 and C2 do not separate, every score this tool produces is noise and
the real-clause results are discarded.
"""
import json, sys
from judge import jev

CONTROLS = [
 ("C1 true-stated",
  {"spec": "Every record has an id. An example record is {\"id\": \"a\", \"kind\": \"task\"}."},
  "The specification example above uses the record id \"a\".",
  "high"),
 ("C2 false-stated",
  {"spec": "Every record has an id. An example record is {\"id\": \"a\", \"kind\": \"task\"}."},
  "The specification example above uses the record id \"zzz-9\".",
  "low"),
 ("C3 out-of-scope",
  {"spec": "Every record has an id. An example record is {\"id\": \"a\", \"kind\": \"task\"}."},
  "The moon orbits the sun.",
  "middle"),
]

if __name__ == "__main__":
    res = {}
    print("  control                    score   expect")
    for name, state, claim, expect in CONTROLS:
        s = jev(state, f"{claim}\n\nAnswer true if the text above supports this statement. If the text does not settle it, do not guess.")
        res[name] = {"score": s, "expect": expect, "state": state, "claim": claim}
        print(f"  {name:<24}  {s:.3f}   {expect}")
    json.dump(res, open("calibration.json", "w"), indent=1)

    t = res["C1 true-stated"]["score"]; f = res["C2 false-stated"]["score"]; o = res["C3 out-of-scope"]["score"]
    sep = t - f
    print()
    print(f"  separation true-vs-false : {sep:+.3f}")
    print(f"  out-of-scope sits between: {f < o < t}")
    ok = sep > 0.3 and f < o < t
    print(f"  CALIBRATED: {ok}")
    if not ok:
        print("  -> the instrument does not separate. Real-clause scores are DISCARDED.")
    sys.exit(0 if ok else 1)
