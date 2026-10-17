# The controls had no controls

The last step was mechanical once I knew to do it: for every suite in
the fleet, ask **has this badge ever been green, and how many
participants actually ran?**

| repo | CI history |
|---|---|
| `gitpp-fold-conformance` | **zero successful runs, ever** |
| `negctl` | **never ran** — exit 128 at the clone step |
| `gitpp-cs` | **no runs at all** |
| `gitpp-c` | **no runs at all** |
| `gitpp-cross-port` | **no runs at all** |
| `gitpp-pl` | 1 run, failure |
| `gitpp-js` | 1 run, success |

Three ports have **no CI whatsoever**, and the two suites that verify
the others have not run in CI.

## The recursion, stated plainly

`negctl` exists to prove that a conformance suite can fail. It had never
been executed. So the claim *"negctl demonstrated that the suite passes
29/29 with its spec check blinded"* — which I reported — was a **local
observation by hand**, not a property of anything anyone else could
check.

That is not a criticism of the result, which is real and reproducible.
It is a criticism of a **category**: a control that only runs when
someone remembers to run it is not a control, it is a habit.

## And the deeper form

Every finding in this project came from a question of the form *"what
does this do when the input is deliberately wrong?"* — and not one of
the twelve or so defects I have found came from that question being
asked of the **infrastructure**:

- the comparator that could not compare
- the participant that could not fail
- the runner that could not run
- the port that could not load
- the wrapper that was not the port
- the control that could not run
- the control that could not report its own failure

**Ten defects in code, and seven in the things that check the code.**
The ratio is the finding. The code under test was being written with
more care than the means of testing it, because writing code feels like
progress and writing a check feels like bookkeeping.

## What would actually prevent a recurrence

Not vigilance — this has now recurred six times. Something structural:

1. **A badge that has never been green is a failing badge.** Not
   "pending", not "unknown". Any repo whose first CI run is a failure
   should be treated as *never tested* until proven otherwise.
2. **Assert the participant count, not just the pass count.** A run over
   four ports and a run over five print the same number. This one
   assertion would have caught five of the seven infrastructure defects,
   and it is about six lines.
3. **A negative control must run in CI, or it is not a negative
   control.** It must also be able to report its own failure, which
   `negctl` could not until this session.
4. **No hardcoded absolute paths in anything that runs in CI.** Seven of
   the twelve defects trace to `/tmp/...` on one machine.

Points 2 and 4 are cheap, mechanical, and could be applied to every
repo in the fleet this week. Points 1 and 3 are policy, and policy is
the part that has not been written down anywhere — which is why it has
now been discovered seven times by the same accident.
