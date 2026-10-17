# Four harnesses in this fleet produced a green they could not fail

Every one of these looked exactly like success. Not one was found by
reading the code — each was found by asking a number what it does when
the input is deliberately wrong.

| # | harness | defect | what it reported | found by |
|---|---|---|---|---|
| 1 | `run.mjs` (fold conformance) | `cmp` was a 1-arg function; 25 of 29 cases compared against nothing | `29 pass`, always | a build agent adding *unrelated* cases |
| 2 | `compare.py` (reading-room) | an absent reader counted as agreement | `13/13 agree` with one of three readers deleted | `/tmp` was wiped mid-session |
| 3 | `cleaf.mjs` (leaf fuzz) | a port killed by a 128 KB `argv` limit; `E2BIG` surfaced as a crash | `20043 agree` at some batch sizes, C dead at others | asking whether the number *could* fail |
| 4 | `fuzz.mjs`'s record walker | escape test compared 1 char to a 2-char literal — dead code | half the mutant corpus silently untested | a verification agent asked if the *instrument* was sound |

None of the four is a bug in the thing being tested. All four are bugs
in the thing doing the measuring, and all four produce a number
indistinguishable from a pass.

## The pattern, and why it is systematic

All four share two properties:

- **A participant could be absent** and the run still reported a total.
  (2, 3, and the fourth member of the 3-way case in #1)
- **The comparison could be skipped or short-circuited** without the run
  saying so. (1, 4)

And there is a third property that only became visible at the end, and
it is the one that would have caught all four:

> **Every negative control in this fleet sabotaged an IMPLEMENTATION.**
> None sabotaged the *expectations*.

If all ports are correct and agree, corrupting the corpus is invisible.
And blinding the comparator is invisible too, because the port-to-port
check still passes. The control that was missing is obvious in
hindsight and was never written: **corrupt the expected values and
require a failure.**

That is exactly why fix #1, though real and necessary, was not
sufficient:

```
blind the comparator                  -> 29 pass, 0 fail
ALSO corrupt 10 of 29 expected states -> 29 pass, 0 fail
```

`cmp` feeds two checks — ports-vs-ports and ports-vs-spec. Fixing the
spec check left the port-to-port check satisfied for the wrong reason,
so the corrupted expectations were consulted by nothing that could fail.

## The rule, stated once

> A number without a demonstrated failure is not a result. It is the
> absence of a complaint, and the absence of a complaint is exactly what
> a broken instrument looks like.

The operational form: every number ships with the mutation that moves
it, and the mutation is chosen to be **predictable in magnitude**, so a
control that moves the number by an unpredictable amount is visibly
testing something other than what it claims.

`fuzz.mjs` is the clean example. Sabotaging one leaf per batch should
move the count by exactly the batch count — **11 at `BATCH=2000`, 81
at `BATCH=250`**. It did, in both. That is what a working control looks
like, and it is why that number is now worth something.

## What is now permanent

- `negctl` has two harness-targeted sabotages, `harness-comparison-blinded`
  and `participant-silently-absent`. The first negctl run to report
  anything other than success found two real gaps.
- `run.mjs` asserts that the spec comparison was demonstrably exercised
  and that no participant is missing. Both sabotage classes now produce
  25 and 29 failures respectively, where both previously produced 29/0.
- `NEGATIVE-CONTROLS.md` in this repo lists every reported number with
  the mutation that moves it, **and** the numbers that are still
  unverified.

## The numbers that are still unverified

Stated rather than implied, because a controls file that only lists
passes is a press release:

  **CORRECTED — this was wrong.** `gitpp-doctor` is the clone *directory*
  name in the conformance workflow, not a repository. The five ports are c,
  `git.pp-doctor`, js, cs and pl, and all five are real. See
  `CORRECTION-five-ports.md`.
- The **Elixir** patch is pushed and has **never been executed**.
  *(stale: it WAS executed later this session — 4/4 pinned leaves, 20/20
  tests, 0 failures, on OTP 27 + Elixir 1.17. See `ELIXIR-EXECUTED.md`.)*
- The **cycle-descendant** fix ships to `js`/`cs`/`pl` and is verified
  on 13 cases, but the PRs are open and the mainline result is not yet
  green across all ports.
- `gitpp-cs` fails **`L1/signed-zero`** (emits `0.0`, spec §3 wants
  `0`), and its official `runner.mjs` silently ignores
  `expected_order` and `expected_orphans_includes` — so part of the gap
  is harness laxity, not a worse result. Same shape as #1.
- `equivalence.py`'s first two versions reported **"0 counterexamples"**
  from broken searches. The version that produced the real result states
  four cases by hand. The history is kept rather than cleaned, because
  the second failure — paraphrasing instead of quoting — is the one that
  silently changed a verdict.
