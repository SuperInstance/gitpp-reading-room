# A shared negative-control harness

## The problem this would have prevented

Three separate harnesses in this fleet reported a green they could not
fail:

| harness | defect | what it said |
|---|---|---|
| `gitpp-fold-conformance/run.mjs` | `cmp` was a 1-arg function; 25 of 29 cases compared against nothing | `29 pass` |
| `gitpp-reading-room/compare.py` | an absent reader counted as agreement | `13/13 agree` |
| `gitpp-cross-port/cleaf.mjs` | a port killed by a 128 KB `argv` limit, surfaced as a crash | `20043 agree` |

None of them was a bug in the thing being tested. All three were bugs
in the instrument, and all three produced a number that looked exactly
like success.

## What they had in common

- A participant could be **absent** and the run still reported a total.
- The comparison could be **skipped or short-circuited** without the
  run saying so.
- The reported number was **not paired with a demonstrated failure**.

`negctl` already exists for sabotage-based negative control on a single
port. What is missing is a *fleet-wide* assertion that any harness which
reports a pass count can also report a lower one.

## The shape of it

A harness declares what it means to be runnable, and the fleet checks
the declaration before believing any number:

```
required:
  participants:  [py, js, cs, pl, c]
  must_all_report: true        # an absent participant is a FAIL, not a pass
  comparison:    "reported"    # a comparison that cannot fail is a FAIL
  min_participants: 2
```

Then two runs are mandatory, not optional:

1. **the honest run** — whatever number comes out;
2. **the sabotaged run** — one participant perturbed by a known,
   reversible amount, and the number must move by a predicted amount.

A harness that cannot pass check 2 is not reporting a result. It is
reporting an absence of complaints, and those are different things.

## Why the predicted amount matters

"Something went down" is weak. `fuzz.mjs` sabotaging one leaf per batch
should move the count by *exactly the number of batches* — 11 at
`BATCH=2000`, 81 at `BATCH=250`. A control that moves the number by an
unpredictable amount is testing something other than what it claims.

That is the check that would have caught all three defects. Each of them
failed to move when it should have moved:

- `run.mjs`: corrupting d05's expectations changed nothing
- `compare.py`: deleting a reader changed nothing
- `cleaf.mjs`: raising `BATCH` past 128 KB silently removed a participant

## The general rule

> A number without a demonstrated failure is not a result. It is the
> absence of a complaint, and the absence of a complaint is what a
> broken instrument looks like.

The three defects were found by asking each number *"what do you do when
the input is deliberately wrong?"* — not by reading any of the
comparisons. Reading is how all three got shipped.
