# Negative controls for every number this repo reports

A number is not evidence until you have watched it fail. This file is
the receipts: each claimed result, and the sabotage that moves it.

The general rule, arrived at three times over in this project:

> **A harness that cannot produce a failure is indistinguishable from a
> harness that is passing.**

The three times it bit:

| harness | defect | what it reported |
|---|---|---|
| `gitpp-fold-conformance/run.mjs` | `cmp` was a 1-arg function, so 25 of 29 cases compared against nothing | `29 pass`, always |
| `gitpp-reading-room/compare.py` | an absent reader (wiped `/tmp`) was not distinguished from agreement | `13/13 agree` |
| `gitpp-cross-port/cleaf.mjs` | C port killed by a 128 KB `argv` limit; `E2BIG` surfaced as a crash | `20043 agree` at some batch sizes, C dead at others |

---

## 1. `compare.py` — the reading-room probe set

**Claim: 14 of 16 probes agree; 2 disagree (both the known reader3 orphan rule).**

| sabotage | result | control valid? |
|---|---|---|
| none | **14/16 agree** | baseline |
| reader1-py Kahn tie-break `min` → `max` | **10/16 agree**, 6 disagree | ✅ **moves** |
| an absent reader (port deleted) | reports `readers ABSENT`, not agreement | ✅ **moves** |
| a reader erroring on every probe | `ERROR`, counted as disagreement | ✅ **moves** |

One character changed in one reader moved the headline by four probes.
The suite is capable of failing.

## 2. `fuzz.mjs` — the 20,043-record leaf corpus

**Claim: 20,043 / 20,043, 0 errors.**

| configuration | result | control valid? |
|---|---|---|
| `BATCH=250` (the default) | 20043 agree, 0 disagree, 69.6s | baseline |
| `BATCH=2000` | 20043 agree, 0 disagree, **14.6s** | ✅ faster after fix |
| `BATCH=2000`, one leaf sabotaged per batch | 20032 agree, **11 disagree** | ✅ **moves by exactly the batch count** |
| `BATCH=250`, one leaf sabotaged per batch | 19962 agree, **81 disagree** | ✅ **moves by exactly the batch count** |

The sabotage count equals the batch count in both configurations
(11 batches at 2000, 81 at 250), which is the signature of a control
that is measuring what it claims to measure rather than failing for
some unrelated reason.

## 3. `run.mjs` — the fold conformance suite

**Claim: 29 / 29.**

| mutation | result | control valid? |
|---|---|---|
| none, all four ports live | **29 pass, 0 fail** | baseline |
| `cmp` reverted to the 1-arg bug, orders fixed | **29/29** | the false green, reproduced |
| `cmp` fixed, d10/d20 orders reverted | 27/29, 2 fail | ✅ **moves** |
| both fixes in | **29/29** | ✅ |
| `d05` expectations corrupted to nonsense | `FAIL d05` | ✅ **moves** |
| C and C# wrapper paths absent | `0/29` + explicit "port errored" | ✅ **moves** |

That first row is the important one: **the original bug reproduces its
original false green on demand.** A fix that cannot be shown to have
removed a specific failure is not a fix.

## 4. Port regression tests

Each port fix ships with a test that was mutation-verified — the fix
reverted, the test fails, the fix restored, the test passes.

| port | test | with fix | fix reverted |
|---|---|---|---|
| `gitpp-cs` | `NullKindRegression` (6 cases) | 6/6 pass | 2 fail, `ArgumentNullException` |
| `gitpp-js` | `v046.test.js` (10 cases) | 10/10 pass | comparator 1 → 1 fail; comparator 2 → 2 fail |
| `gitpp-js` | full suite incl. pinned root/leaf | 22/22 pass | — |

## 5. The RSI-Anything adjudicator

**Claim: the readings score as reported.** A probability is not a test
result, so this is a different kind of control — the question is
whether the instrument can say *no*.

| control | score | expected |
|---|---|---|
| true claim, plainly stated | 0.950–0.990 | high |
| false claim, contradicted by the text | 0.010–0.020 | low |
| true claim, never mentioned | 0.030–0.070 | middle |

Separation +0.93 to +0.98 across every run. An instrument that cannot
return 0.01 cannot adjudicate, and this one can.

---

## What is still unverified

Stated rather than implied:

- **`gitpp-doctor` is 404.** Any "five ports agree" claim covers at
  most four. The C port's comments cite it as having once shipped the
  inverted ancestry test, so this is not hypothetical.
- **The Elixir patch is pushed but never executed.** No BEAM toolchain
  in this sandbox.
- **The cycle-descendant fix** is being applied to three ports; the
  three-clause rule is proven on 13 cases but the port edits are in
  flight.
- **`equivalence.py`'s first two versions reported "0 counterexamples"
  from broken searches.** The version that produced the real result
  states four cases by hand. The history is kept rather than cleaned,
  because the second failure (paraphrased instead of quoted text) is
  the one that changed a verdict.
