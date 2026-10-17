# The fix I shipped is necessary and NOT sufficient — proven by sabotaging it

I fixed `run.mjs`'s blinded comparator last session, mutation-verified
it, opened a PR, and reported it as closing the "25 of 29 cases
asserted nothing" defect.

**negctl has now demonstrated that the suite still passes 29/29 with
the comparator blinded.** The fix was real. It was also not enough.

## The demonstration

```
1. blind the comparator        -> 29 pass, 0 fail
2. ALSO corrupt 10 of 29       -> 29 pass, 0 fail
   expected `state` values to {"CORRUPT": 1}
```

Both sabotages applied together, and the suite reports a perfect run.

## Why the first fix could not have caught this

`cmp` is used by **two** checks:

```js
const agreeWithEachOther = live.every(p => ... && cmp(results[p], results[live[0]]));
const matchSpec         = live.every(p => ... && cmp(results[p], expected));
```

With `cmp` blinded, `matchSpec` becomes constant-true — which is what
I fixed. But `agreeWithEachOther` compares **the ports against each
other**, and when the ports all agree, that check is *satisfied on its
own terms* regardless of what the spec says. The suite then needs both
to be true to pass, and one of them is true for the wrong reason.

So: the ports agreeing made the spec check redundant. The corrupted
expectations were never consulted by anything that could fail.

## The uncomfortable part

This is the third instance in this project of the same shape, and each
one has been found by a different mechanism:

| # | defect | why the previous control missed it |
|---|---|---|
| 1 | `cmp` blinded the spec check | nothing tested `matchSpec` in isolation |
| 2 | a participant could vanish | the denominator was never asserted |
| 3 | **ports agree ⇒ spec check redundant** | every prior control sabotaged a PORT, never the EXPECTATIONS |

**Every negative control in this fleet so far has sabotaged an
implementation.** That is a systematic blind spot: if all ports are
correct and agree, corrupting the corpus is invisible, and corrupting
the comparator is invisible too, because the port-to-port check still
passes.

The control that was missing is therefore obvious in hindsight and was
never written: **corrupt the expected values and require the suite to
fail.** Any suite whose cases are all satisfied by port agreement
alone cannot detect it.

## What this means for the earlier claim

The `run.mjs` fix stands on its own merits — `cmp` was genuinely
broken, and 25 cases were genuinely comparing against nothing. But
"25 of 29 cases now actually compare" overstates what the suite
*demonstrates*, because the suite cannot currently distinguish
"matches the spec" from "all ports happen to agree."

The honest statement is: the comparator bug was fixed, and a second,
independent defect remains — the suite has no control that requires the
**expected** values to be consulted.

## The fix, in one line

The sabotages must include corpus mutations, and negctl should treat a
suite that survives one as untrustworthy. That is now implemented as
two sabotages:

```
harness-comparison-blinded   DETECTED? NO   -> 29 pass
participant-silently-absent DETECTED? NO   -> 29 pass
```

Both currently **MISSED**, which is the correct result: negctl is now
reporting that the suite cannot be trusted to pass, and it is right.
