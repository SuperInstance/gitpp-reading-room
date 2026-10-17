# gitpp-reading-room

> Independent readings of the git.pp spec, with **disagreement as the output**.

## The problem this exists to attack

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

## The method

Four readers, each given the spec and **forbidden from seeing any other
implementation** — including implementations in their own language:

| reader | language | brief |
|---|---|---|
| 1 | Python | neutral |
| 2 | JavaScript | **forced to read §5.1–§5.2 ancestry-first** (class as tie-break) |
| 3 | C99 | type-system-forced ambiguities: bounds, overflow, codepoint vs byte |
| 4 | — | no code: an adversarial **ambiguity atlas**, clause by clause |

Reader 2 is deliberately pointed at the other defensible reading of the
precedence clause. The experiment is whether the spec actually distinguishes
the two, and reader 4 is asked independently which clause is riskiest.

Then `compare.py` runs every reader over `corpus/probes.json` — records chosen
because two defensible readings produce **different** output on them — and
reports where they diverge.

## What "disagreement" means here

A probe where readers differ is **not** a bug report. It is a measurement:
the spec does not determine the answer. Those are the clauses where two
careful implementers will ship different systems and neither can say the other
is wrong.

Where readers agree, the clause is load-bearing and unambiguous. That is worth
knowing too.

## Files

| | |
|---|---|
| `corpus/probes.json` | 10 discriminating probes, each with its expected split |
| `compare.py` | runs all readers, reports agreement and located disagreement |
| `readers.json` | reader commands and their brief |
| `result.json` | latest run (written by `compare.py`) |

## The corpus is instruments, not tests

Each probe exists for one reason: two readings diverge on it. A probe that all
readers pass is measuring nothing and should be deleted. The harness reports
agreement *and* disagreement for that reason — a suite of only-hard-cases
would hide the finding.

## What this is not

Not a competing fold. There are already five of those. This is a measurement
of how much of the spec is underdetermined, and therefore of how much of the
conformance work is load-bearing versus how much is everyone agreeing on
something the document never actually said.
