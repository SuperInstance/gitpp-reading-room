# Reader 4 — a fourth implementation, written to attack §5.4

## Why a fourth

The expectation audit has a **sample size of one**: `reader3-c` is the
only implementation in this fleet not authored by the spec's author. A
single wrong reader produces a wall of false mismatches, and a reader
that agrees with everything is not evidence of much.

**Independence is the scarce resource here, not agreement.** Five ports
agreeing is cheap. A second implementation written by someone who had
not read the spec author's is expensive, and it is the only thing that
has ever found a real defect.

## What it was built to attack

Two clauses nobody had audited:

**§5.4 order.** *"Kahn's algorithm with smallest-id-first"* admits a
sequential reading (take the minimum of the current ready set,
re-evaluate after each emission) and a whole-tier reading (release and
order an entire tier, then start the next). They differ whenever
emitting a node makes another ready whose id sorts before an
already-ready peer — `m`, `z`, `mm(m)` gives `[m, mm, z]` sequentially
and `[m, z, mm]` tier-by-tier.

Jev scored sequential at **0.890** against 0.470, and confirmed a
heap-based Kahn *is* smallest-id-first at 0.580 vs 0.260. This reader
implements the sequential reading and independently produces
`[a, b, only]` on `d26` — **agreeing with the stored expectation** where
`reader3-c` does not.

**§5.5 orphans.** Two clauses, not three: a missing parent, or the
record cannot be topologically ordered. Clause 2 is transitive, so a
cycle's whole downstream cone is covered and no third clause is needed.
I wrote a third clause first and a mutation deleting it changed
nothing — which is how I know it was redundant.

## Result

| check | result |
|---|---|
| 12 cycle-orphan cases | **12/12** on the first run |
| 19-probe suite | **14/19 agree, and it adds no new disagreement** |
| 29 corpus expectations | **27/29** — and both mismatches are the stale `d10`/`d20` order already fixed elsewhere |

That the new reader adds **no new disagreements** is the strongest
result available: four implementations written by different hands agree
on every probe, and the two that dissent are the two already known to be
wrong.

## A bug it found in itself

`d25-numeric-string-vs-number` initially disagreed: the reader emitted
no conflict for `{"x": 1.5e20}` versus `{"x": "1.5e20"}`.

Cause: `JSON::PP` hands Perl two **bare scalars** whose stringifications
are identical, so any numeric test collides them and the conflict ledger
silently loses an entry. The fix is a type-aware `canon` that only treats
a scalar as a number when it round-trips as one.

Worth recording as a general hazard: **a language whose JSON decoder
erases the number/string distinction cannot implement a spec that pins
it.** That is a language property, not a bug, and it is invisible until
a case exists that contains both forms in one key.
