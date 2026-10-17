# Two ambiguities, found by generating the inputs the corpus lacked

Finding 14 measured that the 20,043-record leaf corpus contains **zero
records with a parent**, so §5.2, §5.4 and §5.5 are unreachable from
it. `gen/foldgen.py` is a generator for the complement: record *sets*,
over eleven shapes chosen from the spec rather than from imagination.

165 sets, three independently written readers, disagreements grouped by
shape:

```
  excluded (ALL readers errored): 0
  partial  (one reader errored):  76      <-- see F15-B
  82 agree, 83 disagree

  missing-parent-descendant   agree  0  split 15  (100%)
  unorderable                 agree  1  split 14  ( 93%)
  cycle                       agree  4  split 11  ( 73%)
  unorderable-id              agree  4  split 11  ( 73%)
  diamond                     agree  7  split  8  ( 53%)
  chain                       agree  9  split  6  ( 40%)
```

## F15-A — the conflict ledger's `losers` list has no specified order

`conflicts` is the field that differs most often (56 of 83 splits).
The disagreement is not *which* records are losers — that agrees — it is
**what order they appear in**:

```
set-00001  reader1-py  losers: ["r0001","r0000","r0003", ...]
           reader3-c  losers: ["r0000","r0001","r0003", ...]
```

§5.2 says:

> one winner and **N-1 losers**

and §5.3 says `losers` is *every candidate that set K with a value other
than the winner's*. Both describe a **set**. Neither says what order it
is written in.

So three ports chose three orders — discovery order, sorted order,
partial order — and all three are defensible readings of the text. The
`order` field is different: §5.4 pins it explicitly, and the two ports
that disagree about `order` disagree for a *substantive* reason (below).

This is invisible to a byte-comparison harness that normalises, and it
is invisible to any case whose losers happen to come out in the same
order — which is every case with one loser, i.e. most of the 29-case
fold corpus.

**It matters.** The conflict ledger is a receipt: "these records tried to
set this key and lost". A consumer diffing two ledgers, or hashing one,
gets different bytes for the same fold. That is the same failure the
`gitpp-c` Unicode bug has, one level up — a canonical form that is not
canonical.

**Proposed:** `losers` is in `order` sequence — the same order as §5.4
emits. It is already a total order over the records, it is the order in
which the losers were displaced, and it needs no new concept.

## F15-B — 76 of 165 sets had a reader error on a single case

Roughly half the corpus killed one reader. That is not a disagreement,
and it is not a flaky test — it is a port failing on **legal input**.

The generator emits `null` inside `effects`, nested objects, arrays, and
the `1e16` band. All of that is inside the spec's pinned domain. A port
that throws there is wrong, and the 29-case corpus does not contain any
of those shapes, so nothing in the current suite would ever have run
those lines.

**This is Finding 14 with a number on it.** The coverage gap is not
theoretical: on inputs a second generator produces freely, an
implementation dies.

## F15-C — §5.4 confirmed at scale, and it is the one that changes results

`unorderable` splits **14 of 15**, and `order` is the differing field
in every one. That shape is built to separate the two readings of
"smallest-id-first":

```
b (root)      ready
a (root)      ready
mm (parent m) becomes ready only after m is emitted
```

A sequential Kahn takes `m`, then re-evaluates and finds `mm < z`, so
`mm` comes next. A tier-based Kahn sealed the tier `{m, z}` first and
emits `mm` last. **The `order` field is a first-class output of
`derive()`**, so this is not a presentation difference — it changes
which record a downstream consumer sees as processed first.

Finding 08 adjudicated this at 0.890 for sequential against 0.470 for
whole-tier, and Jev also confirmed a heap-based Kahn *is*
smallest-id-first at 0.580 against 0.260. What the adjudication could
not tell me is how often it matters, and the answer is: **on one shape
in eleven, 93% of the time.**

The spec needs one sentence. Everything else in §5.4 is already
determined; only this is not.

## What this adds to the picture

Findings 08 and 15 are the same clause found twice, by different means —
once by adjudicating a disagreement between readers, once by generating
the input shape that makes the disagreement inevitable. The second method
is strictly better, because it does not require someone to have already
noticed the ambiguity.

That is the method worth keeping: **generate the shape that separates
the two readings, rather than waiting for two implementations to fall
into it.** Here it turned one adjudicated ambiguity into a rate.
