#!/usr/bin/env python3
"""Are the conformance corpus's expected values CORRECT?

negctl now proves the suite *reads* its expectations. That is a
different question from whether they are RIGHT.

Every `expect` in cases.json was hand-authored by the same person who
wrote the spec. If the author made a systematic error, every port would
be marked wrong to agree with it, and the suite would be green and
incorrect together. Nothing in the current setup can detect that,
because the ports were never an independent source for `expect`.

The one source in this project that IS independent of the spec author
is the reading-room readers -- particularly reader3, an independently
written C fold. So: recompute every case with an independent
implementation and diff against the stored expectation.

A MISMATCH is not automatically a defect in the expectation. It is a
place where the spec author and an independent author disagree, and
those places are exactly where the interesting defects live.
"""
import json, os, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
CASES = sys.argv[1] if len(sys.argv) > 1 else "/tmp/fc2/cases.json"

# the three readers available in-repo; reader3 is the independent one
READERS = {
    "reader1-py": ["python3", f"{HERE}/readers/py/cli.py"],
    "reader2-js": ["node", f"{HERE}/readers/js/cli.mjs"],
    "reader3-c":  ["python3", f"{HERE}/readers/c/cli.py"],
}
FIELDS = ("state", "order", "conflicts", "orphans")

def run(cmd, records):
    r = subprocess.run(cmd, input=json.dumps(records), capture_output=True, text=True, timeout=90)
    if r.returncode != 0: return None
    try: return json.loads(r.stdout)
    except Exception: return None

def norm(v):
    if isinstance(v, dict): return {k: norm(v[k]) for k in sorted(v)}
    if isinstance(v, list): return [norm(x) for x in v]
    if isinstance(v, float) and v.is_integer(): return int(v)
    return v

def main():
    cases = json.load(open(CASES))["cases"]
    only = sys.argv[2] if len(sys.argv) > 2 else "reader3-c"
    print(f"  auditing {len(cases)} expectations against {only}")
    print()
    mismatches = []
    for c in cases:
        exp = c.get("expect") or {}
        got = run(READERS[only], c["records"])
        if got is None:
            print(f"  {c['name']:<34} ERROR"); continue
        diffs = [f for f in FIELDS if f in exp and norm(exp[f]) != norm(got.get(f))]
        if diffs:
            mismatches.append((c["name"], diffs, exp, got))
            print(f"  {c['name']:<34} MISMATCH on {','.join(diffs)}")
            for f in diffs:
                print(f"      expected {f}: {json.dumps(norm(exp[f]))[:90]}")
                print(f"      actual   {f}: {json.dumps(norm(got.get(f)))[:90]}")
        else:
            print(f"  {c['name']:<34} agrees")
    print()
    print(f"  {len(mismatches)} of {len(cases)} stored expectations disagree with {only}")
    json.dump([{"name": n, "fields": d} for n, d, _, _ in mismatches],
              open(f"{HERE}/expectation-mismatches.json", "w"), indent=1)
    return mismatches

if __name__ == "__main__":
    main()
