# The unexecuted component was not the risk. Being unexecuted was the *symptom*.

The Elixir port sat in this project for a long time labelled
"explicitly unverified", with a commit message saying so, an open issue,
and a summary entry repeating it. That labelling read as responsible
bookkeeping. It was not.

## What executing it actually found

| | |
|---|---|
| pinned leaves | **3 of 4** — the non-BMP case was wrong |
| its own test suite | **15 tests, 1 failure** — CI red |
| a test that **asserted the wrong answer** | `canon_test.exs:30` pinned the behaviour the spec forbids |
| a second stale threshold | `exp >= 17` in a *different function* from the one the previous fix touched |
| a crash on any explicit JSON null | `:json.decode` gives `:null`; `encode/1` had no clause |
| a **locale dependency** | without `+fnu` and UTF-8, the leaf is wrong |

Six findings, and **not one of them was visible from the label**. "Not
executed" said what was true; it did not say what was true *instead*.

## The pattern across everything in this project

| component | labelled | actually |
|---|---|---|
| Elixir port | unverified | 3/4 leaves, red CI, a test protecting a defect |
| `run.mjs` | 29/29 pass | 25 of 29 cases compared against nothing |
| `fuzz.mjs` | 20,043/20,043 | half the mutant corpus never tested |
| `gitpp-c` | conformant | promotes an absent `kind` above a `witness` |
| `gitpp-js` | conformant | tie-break contradicted the spec; 4-cycle orphan bug |
| `gitpp-cs` | conformant | threw on two `kind: null` records |
| `gitpp-cross-port` README:48 | "the Elixir port also produces it" | unverified, and now doubtful |

**Every defect in this project was a claim that had never been tested
by someone other than the claimant.**

The labels were not lies. `gitpp-c` *was* conformant on everything the
corpus contained. `run.mjs` *did* pass. But conformance is a claim about
a *set of cases*, and in every case above the set was missing exactly
the record shape that would have exposed the defect.

## Why the set was always missing the right case

Because the cases were written by the person who wrote the code, from
the same mental model. This is not carelessness — it is what happens
when the author is also the test author. The case that would catch the
bug requires *imagining the bug*, and you can only imagine bugs you
suspect.

The cases that DID find things were always written by someone, or some
thing, working from the outside:

- `reader3-c` — an independently written C fold → found the cycle-orphan divergence
- `EXPECTATION-AUDIT.py` — recomputing with a reader that did not come from the spec's author → found a wrong expectation *and* a §5.4 ambiguity nobody had audited
- `negctl` with a harness-targeted sabotage → found that the suite passes 29/29 with its spec check blinded
- the verification agent asked "is the instrument sound?" → found the record walker discarding half the corpus
- the Elixir agent asked "can this run at all?" → found six defects in a component that was labelled rather than tested

None of those is a better test. They are tests written by something
that did not already believe the answer.

## The rule, in its final form

> **Conformance is a claim about a set of cases, and the set is always
> smaller than the space.** The only reliable way to find out whether
> the space is missing something is to ask a question the author could
> not have guessed the answer to.

Concretely, for this project, in priority order:

1. **Never mark a component "unverified" and move on.** Either execute it,
   or state *what executing it would check* and why that has not been
   done. "Unverified" is where defects go to be permanent.
2. **Every claim needs a case that was written to be able to fail.**
   A case that passes against every plausible reading is not a test.
3. **Any test whose expectation is authored by the same person as the
   code needs an outside recomputation.** `EXPECTATION-AUDIT.py` is that
   step, and it is now part of the routine.
4. **The most valuable agent is the one asked to distrust something**,
   not the one asked to build something. Every finding in the table
   above came from a task phrased as "verify" or "is this sound", not
   "implement".

## What I would do differently

Six of these findings were available for weeks. The Elixir port could
have been executed the day the toolchain question arose — the blocker
turned out to be `apt-get update`, not a missing capability.

The cost of "explicitly unverified" is that it reads as *careful*. It
is the opposite: it is a decision not to look, dressed as a decision to
defer. A label that documents a gap is worth keeping; a label used *in
place of* the check is the failure mode this whole project has been
studying, applied to itself.
