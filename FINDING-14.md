# The 20,043-record corpus contains no records with parents

Finding 13 established the ports are independent, so their agreement is
real evidence. It also named the limit: **agreement is only evidence
about the cases exercised**, and the corpus comes from one generator.

Here is what that generator actually produced.

```
records with >= 1 parent  :     0   of 20043
records with kind: null  :     0
records with unknown kind:     0
```

**Every one of the 20,043 records is a root with a recognised class.**

## Why that is the whole story

`fuzz.mjs` is a *leaf* harness. It computes one thing per record — the
SHA-256 of the canonical form — and compares across ports. That is a
correct and valuable harness for what it covers, and it covers the
encoding thoroughly: four value regimes, nested objects, arrays, bools,
signed zero, the `1e16` boundary, non-BMP ids.

But three of the fold's four clauses are about records **with parents**:

| clause | subject | records needed |
|---|---|---|
| §5.2 per-key resolution | class vs ancestry | at least two related records |
| §5.4 order | Kahn over the parent DAG | parents, and shared parents |
| §5.5 orphans | missing parents, cycles | parents that are absent |

**A corpus of roots cannot exercise any of them.** Not "rarely" —
*not at all*, and not incidentally: the generator has no code path that
emits a parent, so the absence is structural rather than statistical.

This is the same shape as every other finding in this series, one level
up. The 20,043/20,043 result is real, and it is a claim about a
generator that only ever emits leaves. Nobody was deceived: the number
was true about what it measured. What was missing was the question
*what does it measure*.

## It explains a historical accident

The four fold bugs found this session all live in clauses a root-only
corpus cannot reach:

- `gitpp-cs` promoting an absent `kind` to `task`
- `gitpp-js`'s tie-break ignoring topo order
- three ports not orphaning cycle descendants
- `reader3-c`'s §5.5 derivation

Every one of them was found by the **29-case fold corpus** or by a
hand-built probe, not by the 131k mutant pass and not by the leaf fuzz.
The two harnesses are not redundant and they do not overlap: one covers
encoding, the other covers the fold, and the leaf harness covers only
the half of the system where a record is alone.

## What would close it

The leaf harness's scope is correct and should not be widened — a leaf
harness that also tried to fold would blur what it proves. What is
missing is a **generator that emits parents**, so the fold has a
property-based counterpart to the leaf corpus.

The factors, and what a generator must cover to be interesting:

```
kind     : override, task, witness, null, unrecognised string
parents  : none, single, chains, diamonds, fan-out, cycle, MISSING
values   : the four number bands, string twins, bool, nested, array
ids      : ASCII, non-ASCII, non-BMP, empty
```

Note the last row of the missing list from the coverage scan: `NoneType`
appears 338 times in the corpus — a `null` **value** inside `effects`,
which is legal and covered — while `kind: null` appears zero times. The
generator knows about JSON null and applies it in exactly one of the two
places the spec allows it. That is not a random gap; it is a generator
with a mental model of where nulls belong, and the model does not match
the spec.

## The generalisation

> A harness's coverage is a property of its **generator**, not of its
> size. Twenty thousand records from a generator that only emits leaves
> is one data point examined twenty thousand times.

The number that matters is not how many records were tested but how many
**distinct shapes** they occupy. `coverage-bound.py` in this repo reports
that as a number, which is the first time in this project that the size
of a corpus has been compared against anything at all.
