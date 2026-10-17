#!/usr/bin/env python3
"""Jev as a FOURTH reader -- one with no stake in any implementation here.

Readers 1 and 2 share an author, so their agreement proves nothing. Jev
did not write any of the ports, is not the corpus author, and cannot
pattern-match to a codebase.

It is a strange reader in a useful way: it is not a code generator. It
returns a CALIBRATED PROBABILITY that a framed claim is supported, and it
is told to decline rather than guess. So it cannot be asked "what does
the spec say?" in prose -- but for a claim the spec either supports or
does not, it can say how confident it is that the text supports it.

Framing is symmetric: identical state, competing instructions, one call
each. A difference in score is then a property of the CLAUSE rather than
an artefact of how one side happened to be worded.
"""
import json, subprocess, os

def jev(state, question, tries=4):
    body = json.dumps({"state": state, "model": "jev-latest",
                       "questions": {"q": {"type": "noul", "instructions": question}}})
    last = ""
    for i in range(tries):
        r = subprocess.run(["curl", "-sk", "-m", "60", "--fail-with-body",
            "https://api.typesafe.ai/v1/systemone",
            "-H", f"Authorization: Bearer {os.environ['TYPESAFE_AI_TOKEN']}",
            "-H", "Content-Type: application/json", "--data-binary", body],
            capture_output=True, text=True, timeout=90)
        last = (r.stdout or "") + (r.stderr or "")
        if r.returncode == 0:
            try:
                d = json.loads(r.stdout)
                if "answers" in d: return d["answers"]["q"]["noul"]
            except Exception: last = r.stdout[:200]
        if i < tries - 1: subprocess.run(["sleep", "2"])
    raise RuntimeError(f"Jev unreachable: {last[:160]}")

CLAUSES = {
  "p01-class-vs-ancestry": {
    "state": {
      "spec_5_1": "Class precedence is absolute: override > task > witness. An override outranks a task, a task outranks a witness, EVEN WHEN THE WITNESS IS A DESCENDANT OF THE TASK.",
      "spec_5_2": "Per-key resolution. Within a class, if two records contest a key, a record that DESCENDS from another is applied after it and therefore wins.",
      "question": "When an OVERRIDE record is an ANCESTOR of a TASK record and both set the same key, which one sets the final value?"},
    "A": "The OVERRIDE wins, because 5.1 states class precedence is absolute and explicitly says it holds even against a descendant.",
    "B": "The TASK wins, because 5.2 says a descendant is applied after its ancestor and therefore wins.",
  },
  "p07-orphan-descendant": {
    "state": {
      "spec_5_5": "An orphan is a record with a parent id that is not present. The record is excluded from the fold. An orphan's DESCENDANTS ARE NOT AUTO-ORPHANED - being descended from an orphan does not by itself make a record an orphan.",
      "question": "A record K descends from an orphan O. K is not itself an orphan. Do K's own effects enter the final state?"},
    "A": "YES, K's effects apply. K is not an orphan, and 5.5 says explicitly that an orphan's descendants are not auto-orphaned.",
    "B": "NO, K's effects are excluded because its ancestry is broken.",
  },
  "p10-id-validation": {
    "state": {
      "spec_1": "A record id must be free of newline and colon characters.",
      "question": "If a record id contains a colon, is that a hard error the fold rejects, or merely documented?"},
    "A": "It is a HARD ERROR: the spec says an id MUST be free of colon, so an id containing one is not a valid record and the fold rejects it.",
    "B": "It is DOCUMENTED BUT NOT ENFORCED: nothing in the spec's fold sections says the fold validates ids, so such a record is processed normally.",
  },
  "kind-null": {
    "state": {
      "spec_1_1": "The five content fields are id, kind, parents, effects, reason. kind is optional, defaulting to null.",
      "spec_out_of_scope": "Values of kind other than the three named classes have semantics that are OUT OF SCOPE for the fold.",
      "spec_5_1": "Class precedence is absolute: override > task > witness. Per-key resolution ranks records by class.",
      "question": "The fold must rank every record by class. What class does a record with kind=null have, and what does the fold do with it?"},
    "A": "The spec DOES NOT DETERMINE IT. kind=null has no class, and the spec defers its semantics to 'out of scope', yet 5.1 must rank every record. Nothing in the text settles the gap.",
    "B": "null INHERITS THE LOWEST RANK (witness), so it loses every contest.",
  },
}

if __name__ == "__main__":
    out = {}
    for cid, spec in CLAUSES.items():
        row = {"state": spec["state"], "scores": {}}
        for side in ("A", "B"):
            q = f"{spec[side]}\n\nAnswer true if the specification text above supports this statement. If the text does not settle it, do not guess."
            row["scores"][side] = jev(spec["state"], q)
            print(f"  {cid:<26} {side} = {row['scores'][side]:.3f}", flush=True)
        out[cid] = row
    json.dump(out, open(os.path.join(os.path.dirname(__file__), "verdicts.json"), "w"), indent=1)
    print("  wrote verdicts.json")
