# gitpp-reading-room

> Independent readings of the git.pp spec, with **disagreement as the output** —
> and, since 2026-09, the map of the sketchbook the readings came from.

This repository is where the fleet's *evidence* lives. Not the code — the
code is in the `gitpp-*` repos and the 5,000-odd sketches. This is the
record of what was measured, what was found, and **what was wrong in the
findings themselves**.

---

## Why it exists

Conformance testing answers *"do these implementations agree?"* It does not
answer *"is the spec saying anything?"*

Five fold ports agree on 20,043 records. That is a real result and it is
bounded. It says the spec is *self-consistent* and that five implementations
can hit it. It does not say the spec is *right*, because the spec, the corpus,
the reference and the test runner were written by the same person. Every bug
found so far was in code that person wrote or in code written to that person's
reading.

The one genuinely outside judgment in the project was a sixth fold, written by
someone else, which **disagreed** with the spec's reading of §5.1–§5.2. That
disagreement was the most valuable thing that happened all week.

That is the method this room follows: **find something, write it down, then
try to break your own finding.** Four of the corrections in here were written
by the agent whose own work they correct.

---

## Start here

| if you want | read |
|---|---|
| **the state of everything, and what is left** | **[PLAN.md](PLAN.md)** — the inventory, the critical path, the refined order |
| the sketchbook, mapped | [ATLAS-01.md](ATLAS-01.md) |
| where the gold actually is | [ATLAS-02.md](ATLAS-02.md) |
| the ladder: sketchbook -> production -> hardened -> shelf | [ATLAS-03.md](ATLAS-03.md) |
| the substrate's own shape, and where it drifted | [FINDING-26.md](FINDING-26.md) |
| two ports of one idea, and no equivalence check | [FINDING-25.md](FINDING-25.md) |
| the npm layer, and the publish trap | [FINDING-24.md](FINDING-24.md) |

---

## The corrections

Kept in the room on purpose. **A finding left visible and labelled is worth
more than a finding quietly repaired** — the error is the receipt that the
check works.

| | what it corrects |
|---|---|
| [CORRECTION-canon-shape.md](CORRECTION-canon-shape.md) | I claimed the Live Canon service changed its response shape. **It never did.** The client was written against a shape the service never had — four days before the service existed. |
| [CORRECTION-cell-history.md](CORRECTION-cell-history.md) | "There are 11 opcodes" — there are **two** elevens seven hours apart, and 6/5/1/0 are implemented. "26 kinds" is 25/25/24. |
| [CORRECTION-address-vs-id.md](CORRECTION-address-vs-id.md) | I had the **ID and the ADDRESS the wrong way round.** The hash is the cell's ID; the address is a 4D place — `(x, y, t, sig)`, "4D navigation without a map." |
| [CORRECTION-quilt-namespace.md](CORRECTION-quilt-namespace.md) | The name "quilt" is held **three times over**, and the package I thought was ours is a third party's. |
| [CORRECTION-third-party-packages.md](CORRECTION-third-party-packages.md) | 36 of the published packages are **other people's**, with correct provenance — and `motifcode` is `taewooopark`'s. |
| [CORRECTION-signed-zero.md](CORRECTION-signed-zero.md) | An earlier one: a defect I reported **was not a defect.** |

---

## The method, in the house's terms

- **Measure, don't judge.** If it can be run, run it. If it cannot, say so
  and name where you looked.
- **A claim about a score has to be checkable.** Every number here carries
  the command or the file that produced it.
- **The broken artifact is not the witness.** When a client disagrees with a
  service, the client is under suspicion until history says otherwise — and
  history is a commit log, not a vibe.
- **A number in a charter is a claim about a count, and counts drift.**
- **A witness that records only the request witnesses an intent, not an
  outcome** — and an intent is not the substrate of truth for anything.
- **Confidence is a minimum, never a mean**, and a weight never enters the
  confidence number.
- **Green over nothing is the signature defect.** A badge is not a result
  until the thing it claims has been asserted.

---

## What is here

```
ATLAS-*.md            the map
FINDING-*.md           what was found
CORRECTION-*.md        what was found WRONG, and by whom
adr/                   decisions, options, and the graph
chronicle/             where things came from
```
