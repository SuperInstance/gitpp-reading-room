# An expectation audit that belongs in CI

`EXPECTATION-AUDIT.py` recomputes every conformance case with a reader
that was **not** authored by the spec's author, and diffs the result
against the stored `expect`.

## Why it must run on every change

The suite's green means *the ports agree with the expectations*. It does
not mean *the expectations are correct*. Those are independent, and both
failure modes have now been observed:

- **the expectation was wrong** — `d20` stored an alphabetical `order`
  from a normaliser that had once sorted away topological order
- **the implementation was wrong** — `d12` and `d26`, where reader3-c
  diverged from a correct expectation

An audit that runs once finds a bug. An audit that runs on every change
prevents the *authored* data from silently becoming the ground truth it
was supposed to be checked against.

## What it must assert

1. Every case recomputes without error under the independent reader.
2. Any disagreement names a **clause**, not just a field. "order differs"
   is a symptom; "§5.4 does not say whether Kahn is sequential or
   tier-by-tier" is a finding.
3. A disagreement may not be resolved by **editing the expectation to
   match the reader**. That is the one move that converts an audit into
   a rubber stamp. Resolution must cite a spec sentence and, where the
   text is genuinely silent, an adjudication.
4. The reader used for the audit is **pinned**, and its own tests pass.
   An audit performed with a broken reader is worse than no audit: it
   produces confident false alarms that train people to ignore it.

## The one reader problem

There is exactly **one** implementation in this fleet not authored by
the spec's author: reader3, the C fold. So the audit currently has a
sample size of one, and a single wrong reader produces a wall of false
mismatches.

That is the constraint to fix next, and it is the same constraint the
reading-room project has been hitting all along: **independence is the
scarce resource, not agreement.** Five ports agreeing is cheap. Two ports
written by different people is expensive, and it is the only thing that
has ever found a real defect — every port bug this session was found by
divergence from something the spec's author did not write.
