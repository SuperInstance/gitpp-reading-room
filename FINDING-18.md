# The mutation pass was a generator with no runner, and it is the only check that reaches the escape path

The mutant corpus has been the source of this project's best finding
(the C port's `\uXXXX` collision, `gitpp-c#1`). It has never been
*executed* as a check.

`mutate/mutate.py` writes 131,499 mutants to a file. **Nothing consumes
that file.** There is no runner in the repo, no CI step, no script. The
README describes what the pass found, and the finding came from a manual
run that no longer exists.

So the mutation pass — the one mechanism in this fleet that was
designed to be independent of the corpus author's imagination — was a
**generator with no consumer**, which is the same class of defect as a
harness with no negative control. It could not fail, because it could
not run.

## The runner, and what it costs to build

`mutate/run_mutants.mjs` is about sixty lines: apply each port's leaf
function to each mutant, group by transformation class, and report where
two ports disagree.

Running it on the merged tree:

```
131,499 mutants, ports py and js compared
    0 disagreements
```

**Zero.** Every mutant that both ports handle, they handle identically.

The C port is excluded from the verdict, and that exclusion is a
finding rather than a convenience: `gitpp-c#1` is still open, so the C
port disagrees on **every** escape-injection mutant by design. Comparing
against it would report a known defect as a new one on 14,611 mutants
and bury the signal. The runner says so in its output rather than
silently dropping a port.

## The negative control, measured rather than asserted

The claim this project keeps making is that mutation reaches code paths
the corpus never executes. It is measurable, and the measurement is
stark:

```
base corpus records containing an escape sequence :      0 of 20,043
mutant corpus records containing an escape sequence: 14,611 of 131,499
```

**Zero.** The float64 corpus never puts an escape in a string value, by
construction — which is exactly why it never found `gitpp-c#1`.

And a planted defect confirms the difference is real, not theoretical.
Planting a control-character escape defect in the JS port — the same
class as the C port's actual bug:

| corpus | result |
|---|---|
| base, 400 records | **0 of 400 differ — MISSED** |
| escape-injection mutants, 100 | **100 of 100 differ — CAUGHT** |

The base corpus does not merely fail to catch it; it cannot, because it
contains no input that reaches the path. The mutation pass catches every
one.

## A methodological trap, and what it taught me

My first plant was **inert** and I nearly recorded a false result. It
reversed the key sort inside a function that builds an *object*, and
object key order does not affect the hash — the canonicaliser sorts
again later. The base corpus caught that plant on 200 of 200 records,
so I could have concluded "both corpora catch it" and moved on.

What actually happened is more useful: the plant was caught by the base
corpus, so the interesting question became *why*, and the answer was
that the plant never affected the output at all. A second, real plant
was needed to test the claim.

**A negative control that passes for the wrong reason is not a
negative control.** The test for a planted defect is not "does it fail?"
but "does it fail *for the reason I planted it*?" A defect in a
no-op code path is not a defect; it is a decoration.

## The generalised rule

> **A corpus's reach is a property of its generator.** A base corpus
> exercises the paths its author imagined; a mutant corpus exercises
> the paths its author could think to *break*.

And the corollary that made this project slow: the mutation pass was
the most valuable check in the fleet and it was never wired up, because
*writing the generator felt like the work and writing the runner did
not.* The generator is the part that produces something visible. The
runner is the part that produces a verdict.

That is now the fourth instance of the same shape in this project — a
thing that produces output without producing a judgement:

| | produces | judges? |
|---|---|---|
| `mutate.py` | 131,499 mutants | **no runner existed** |
| `run.mjs` (before) | 29 pass | compared against nothing |
| `compare.py` (before) | 13/13 agree | counted an absent reader as agreement |
| `fuzz.mjs` (before) | 20,043 agree | a port could die and be reported as a crash |

The pattern is consistent enough to be a rule:

> **Output is not a verdict.** Anything that produces a number must be
> paired with something that can say the number is wrong.
