# The generated corpus rediscovers a known bug, and separates three that looked like one

The 165-set corpus produced 93 disagreements. Most were the *same* three
ambiguities, and separating them needed a normalisation I had not
applied: **sorting each `losers` list before comparing**, because the
losers-ORDERING difference (Finding 15) is not a semantic difference.

With that normalisation:

| split profile | count |
|---|---|
| `order` only | **28** |
| `conflicts` + `order` + `orphans` | 9 |
| `conflicts` + `order` + `orphans` + `state` | 6 |
| `conflicts` + `order` + `state` | 3 |
| `conflicts` only | 2 |
| `order` + `orphans` + `state` | 2 |
| `conflicts` + `order` | 1 |

## The `state` differences are the known reader3-c bug, found a third time

Six splits have `state` differences, and the minimal witness is two
records:

```json
[{"id":"r0000","kind":"weird-kind","parents":["ghost-r0000"],"effects":{"k0":0}},
 {"id":"r0001","kind":"override",   "parents":["r0000"],      "effects":{"k0":"0","k1":null}}]
```

`r0000` has a missing parent, so it is an orphan. `r0001` is its
descendant. §5.5 says the orphan's descendants are **not**
auto-orphaned, and v0.4.6 says a non-orphan descendant's effects
**do** enter the fold.

| reader | orphans | state | |
|---|---|---|---|
| reader1-py | `[r0000]` | `{k0:"0", k1:null}` | correct |
| reader4-pl | `[r0000]` | `{k0:"0", k1:null}` | correct |
| **reader3-c** | `[r0000, r0001]` | `{}` | **wrong** |

This is the defect already filed as
[gitpp-rr-reader3#1](https://github.com/SuperInstance/gitpp-rr-reader3/issues/1):
reader3-c defines orphans as "anything Kahn never emitted", which
catches the descendant of a *missing* parent as well as a cycle's.

**It has now been found three independent ways:** by reading the code,
by the reading-room probe `p07`, and by a generated corpus that was
built for a different purpose. A defect found three ways is a defect
that is really in the specification's reach, and the third finding is
the cheapest — it needs no inspection of the port at all, just a
generator that emits the shape.

## The 28 order-only splits are §5.4, and they are the REAL remaining ambiguity

`order` is a first-class output of `derive()`. A consumer reads it to
know which records were processed first, and the two readings of
"smallest-id-first" give different answers:

```
m (root)   z (root)   mm (parent m)

sequential Kahn:  m, then mm becomes ready and mm < z, so -> m, mm, z
tier Kahn:        the tier {m, z} is sealed, so               -> m, z, mm
```

28 of 165 sets — **17%** — differ only in `order`, with identical
`state`, `conflicts` and `orphans`. That is the most common
disagreement in the corpus, and it is the one thing about §5.4 that
three separate attempts have failed to settle.

## What this says about the method

The corpus did three things at once, and separating them needed a
**normalisation** that reflects a decision about what counts as a
difference:

- `order` differing — a real ambiguity in §5.4
- `conflicts` differing — the losers ORDERING, which is not semantic
- `state` differing — a real defect in one reader

Comparing raw JSON conflates all three. The normalisation is not a
convenience; it is the statement of what "the same fold" means. And
once it is made, each class can be handled on its own terms: two need a
spec sentence, one needs a port fix.

## The rule, refined

Earlier this session I wrote:

> Generate the shape that separates the two readings, rather than
> waiting for two implementations to fall into it.

The refinement, from this run:

> **Generate the shape, and normalise away the differences you have
> already explained.** A generator produces disagreements of every kind
> at once, and without a normalisation that says which differences are
> semantic, the output is a pile rather than a diagnosis.

Without the losers normalisation, this corpus reported 93 disagreements
and looked like a wall of new findings. With it, 93 becomes 28 of one
kind, 15 of another, and 2 of a third — one of which is a bug already
filed, and one of which is a spec clause that needs a sentence.

The normalisation is the analysis. The generator just supplies the
material.
