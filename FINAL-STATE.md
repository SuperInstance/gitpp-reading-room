# Verified end state

Every number below was measured against the **merged** trees, rebuilt
from `main`, not against the branches they were developed on.

## Conformance

```
fold conformance    5 real ports, built from merged main
                     29 pass, 0 fail, 0 skip

leaf conformance    20,043 records, from merged main
                     20043 agree, 0 disagree, 0 errors   13.9s

mutant corpus       131,499 mutants, py and js compared
                     131499 agree, 0 disagree, 0 errors

Elixir port         OTP 27.3.4.9 + Elixir 1.17.3, from merged main
                     20 tests, 0 failures
                     4/4 pinned leaves PASS
```

The Elixir port required rebuilding the whole toolchain: `apt-get
update` (the package lists were empty), then OTP 25 from apt, then a
source build of OTP 27 because Elixir 1.17 needs it — 1.14 is what apt
offers and it cannot compile `^decpt` in a runtime-sized binary
segment. The prebuilt binary route does not exist for OTP 27.

## What is merged

| repo | PRs | what |
|---|---|---|
| `gitpp-fold-conformance` | #1, #2 | the comparator that compared nothing; the two blind spots in the harness itself |
| `gitpp-cs` | #1, #3 | `kind: null` threw; conflict ledger's `losers` order |
| `gitpp-js` | #1, #3 | tie-break was not topo order; conflict ledger's `losers` order |
| `gitpp-pl` | #1 | conflict ledger's `losers` order |
| `gitpp-cross-port` | #1, #2 | the C port died above 128 KB; the walker discarded half the corpus |
| `negctl` | #1 | the negative control had never run, and could not report its own failure |
| `gitpp-beam` | #3 | three Elixir canonicalisation defects, found by finally executing the port |

`gitpp-cross-port` also has `feat/mutant-runner` open, adding the runner
the mutation pass never had.

## What is still open, and named

- **`gitpp-c#1`** — `parse_str` drops `\uXXXX` escapes entirely. A hash
  collision, not a formatting difference: `{"c":"ab"}` and
  `{"c":"a\u0001b"}` get the same leaf. **This is the one confirmed
  defect in a maintained port that is still live**, and the mutation
  pass detects it on all 14,611 escape-injection mutants.
- **`gitpp-c#2`** — an absent or `null` `kind` is promoted to `"task"`,
  so an unclassifiable record outranks a `witness`.
- **`gitpp-cs` `L1/signed-zero`** — emits `0.0` where §3 wants `0`. Its
  official runner also ignores `expected_order` and
  `expected_orphans_includes`, so part of the gap is harness laxity.
- **`gitpp-doctor` is 404** — it is `git.pp-doctor`. Every "five ports"
  claim means four.
- **`reader3-c`** — the independently written C reader, deliberately left
  implementing the §5.4 tier reading and the wrong §5.5 orphan rule. It
  is the project's only implementation not authored by the spec's
  author, and conforming it would leave an independent reader that
  agrees with everything.
- **The `losers` order fix** is merged in js/cs/pl, but the C port's
  `gitpp-c` still needs it.

## The four things this session established

1. **A number without a demonstrated failure is not a result.** Four
   separate harnesses produced a green they could not fail, and a fifth
   produced numbers with nothing to judge them.
2. **A fix in a branch is a proposal; a fix in `main` is a fact.** The
   difference is only visible by rebuilding from `main` and re-running —
   and that check found a fifth hardcoded-path defect.
3. **A corpus's reach is a property of its generator.** The base
   corpus has 0 of 20,043 records containing an escape sequence; the
   mutant corpus has 14,611 of 131,499. That single number explains
   why `gitpp-c#1` survived weeks.
4. **Output is not a verdict.** The mutation pass was the most valuable
   check in the fleet and was never wired up, because writing the
   generator felt like the work and writing the runner did not.
