# §5.4 has an ambiguity nobody had looked at, and the corpus caught it

## How this surfaced

`negctl` now proves the conformance suite *reads* its expectations. That
is a different question from whether they are **right**.

Every `expect` in `cases.json` was hand-authored by the same person who
wrote the spec. If that person made a systematic error, every port would
be marked wrong for agreeing with it, and the suite would be green and
incorrect together. Nothing in the setup could detect it, because the
ports were never an independent source for `expect`.

So: recompute all 29 cases with **reader3-c** — the independently
written C fold, the only implementation in this project not authored by
the spec's author — and diff against the stored expectations.

`EXPECTATION-AUDIT.py` in this repo does it. **4 of 29 disagree.**

| case | disagreement | verdict |
|---|---|---|
| `d12-orphan-descendant` | orphans, state, order | **reader3 is wrong** (the known §5.5 bug, `gitpp-rr-reader3#1`) |
| `d20-wide-fanout` | order | **expectation is wrong** — the stale alphabetical order, already fixed elsewhere, missed here |
| `d26-setter-provenance` | order | **reader3 is wrong** — and this one is interesting |
| all others | — | agree |

## `d26` is a new finding: §5.4 is ambiguous

```json
[{"id":"only","parents":[],"effects":{"status":"wip"}},
 {"id":"a","parents":[],"effects":{"x":1}},
 {"id":"b","parents":["a"],"effects":{"x":2}}]
```

Stored expectation: `[a, b, only]`. reader3-c: `[a, only, b]`.

The spec says, in full:

> Kahn's algorithm with smallest-id-first. Ties broken by Unicode code
> point.

**Two implementations both satisfy that sentence.**

| reading | algorithm | d26 | discriminator `m, z, mm` |
|---|---|---|---|
| **sequential** | take the min of the current ready set, re-evaluate after each emission | `[a, b, only]` | `[m, mm, z]` |
| **whole-tier** | release and order an entire tier, then start the next | `[a, only, b]` | `[m, z, mm]` |

The discriminator: `m` and `z` are both roots, and `mm` is a child of
`m`. After emitting `m`, `mm` becomes ready and `mm < z`. Sequential
takes it; whole-tier does not, because the tier `{m, z}` was already
sealed.

A heap-based Kahn is the **sequential** one — it always removes the
minimum of the current ready set. So "smallest-id-first" favours
sequential, and the whole-tier reading requires reading the clause as
"tier-by-tier", which the words do not say.

**Adjudicated** behind the calibration gate (0.960 / 0.020 / 0.070,
separation +0.940):

| claim | score |
|---|---|
| `[m, mm, z]` — sequential is required | **0.890** |
| `[m, z, mm]` — whole-tier is required | 0.470 |
| a heap-based Kahn satisfies "smallest-id-first" | **0.580** |
| it does not | 0.260 |

So the stored `d26` expectation is **correct** and reader3-c is wrong.
The clause is readable — but it was only readable because two
implementations disagreed and someone had to adjudicate. That is the
fourth distinct reader/implementation, and the only one that surfaced a
question about a clause nobody had audited.

## The one that is genuinely mine

`d20`'s stored order is the alphabetical `[c01…c10, root]`, and Kahn
gives `[root, c01…c10]`. I found and fixed exactly this in the
`gitpp-fold-conformance` PR — but **not in this copy of the corpus**,
because the two clones had diverged.

That is its own small finding: the expectation audit only works against
one checkout, and the audit itself is therefore only as current as the
tree it ran on. A corpus that exists in two places with two states is a
corpus that will be wrong in one of them.

## What this changes

Nothing in the ports — `d26`'s expectation was already right, and the
ports already agree with it. What this changes is the *method*:

**Authored expectations are unverified data.** The suite's green says
the ports agree with the expectations. It does not say the expectations
are correct, and the two failures are independent: I have now seen a
case where the expectation was wrong (`d20`) and a case where the
implementation was (`d12`, `d26`).

The only independent check available is a reader that did not come from
the spec's author, and there is exactly one. Making that a *required*
input rather than a lucky accident is the obvious next step, and it is
the same shape as the retention gate in RSI-Anything: a component
cannot be accepted on its own quality, because quality measured against
yourself is not quality.
