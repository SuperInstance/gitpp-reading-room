# Spec changes this experiment demands

Four clauses, each with the evidence that forced it. These are proposals,
not applied changes — `git.pp-spec` is unchanged and applying them is a
separate, deliberate act.

---

## 1. §5.2 — "mathematically equivalent" is false as written

The spec says:

> The "gather then filter" model described in v0.2.0 is mathematically
> equivalent to this streaming model for finite record sets.

`equivalence.py` checks this case by case:

| case | result |
|---|---|
| two roots, different class | agree — class decides |
| chain, same class | agree — ancestry decides |
| chain, override deeper | agree — class beats ancestry |
| **two roots, same class** | **gather model is SILENT** |

The last case is the gap. Two unrelated `task` records both setting `x`:
gather-then-filter drops neither (neither is an ancestor of the other),
then says "take the highest class among survivors" — and both survivors
are `task`. The instruction does not select. The streaming model does
select: the first in topo order becomes the setter and the second is
"equal class, not a descendant, the current setter wins."

So the models agree wherever the instruction selects, and the spec calls
them equivalent *unconditionally*. A port author who implements the
v0.2.0 model as described will silently disagree with every port that
implements streaming, on same-class sibling keys — and the spec told them
the two were the same thing.

**Proposed:** either delete the equivalence claim, or complete it —
"…equivalent wherever the survivors' classes differ; where they tie, the
tie is broken by topo order (§5.4), which is what the streaming model
does." The second is better: it converts an unstated tie-break into a
stated one.

### A note on how this was tested, because the first two attempts were wrong

My first version of the search could not generate root records (its
alphabet was 2-tuples and it then rejected every record with more than one
parent), so no chain ever existed. The second admitted self-loops because
the acyclicity check only read the *first* parent. **Both reported "0
counterexamples"** — which is exactly what a broken search reports, and
exactly the result I would have been happy to publish.

The version that produced these numbers states each case by hand. Every
one of the four is checkable by reading two lines, and the failing one
fails for a reason you can see. Enumeration was the wrong tool when the
alphabet is easy to enumerate wrongly.

---

## 2. §1.1 + §5.1 — `kind: null` has no class, but the fold must rank it

§1.1: *"`kind` … One of `task`, `witness`, `override`. Other values are
accepted; their semantics are **out of scope for the fold**."*

§5.1: *"`override > task > witness`."* §5.2 requires every candidate to be
compared by class.

The fold's entire output depends on a ranking the spec declares
out of scope. Three independent readers converged here: reader3 and
reader4 both named it unprompted, and Jev scored "the spec does not
determine it" at **0.840** against 0.120 for the concrete answer.

Nothing catches it. The leaf hashes `kind` as a byte either way. The
closure root never reads the ranking. §6 compares a port against itself
and returns `VERIFIED`. The corpus has no case with a `null` kind.

**Proposed:** state the rank. The minimal rule consistent with §5.1's
direction is that an unrecognised or null `kind` ranks **below
`witness`** — the fold is permissive about what it accepts and strict
about what it can order, and lowest-rank means an unclassifiable record
never displaces a classifiable one. That is one sentence and it closes
the hole; the alternative is making it an error, which is a bigger
change and would reject records the spec currently accepts.

---

## 3. §2.5 — the published "ground truth" digest is not a SHA-256

Step 2 publishes bytes; step 3 publishes a digest for them. The digest is
**65 hex characters** — SHA-256 is 64 — and it is not the hash of the
bytes printed directly above it (the real value is
`eb70b332a71ea6c8aab9a3525ef0906f1875f64778b8f2d2f2ebcf27081c6b03`).

The *bytes* are correct and all four pinned §3.5 leaves verify, so no port
is broken. But this is the one worked example the spec designates as
authoritative, and it is wrong in a section whose purpose is to remove
doubt. A reader checking their work against it gets a false failure and
has no way to know the example, not their port, is at fault.

**Proposed:** replace the digest with the computed one.

---

## 4. §5.1 — the parenthetical invites the reading Jev rejects

§5.1: *"…a `task` outranks a `witness`, **even when the witness is a
descendant of the task**."*

The clause is correct — §5.2 confirms ancestry is a tie-break *within* a
class. But the parenthetical is the only sentence in §5.1 that mentions
ancestry at all, and it is placed as an exception. A reader who takes it
as the shape of the rule generalises it: *class beats ancestry, always* —
which is the ancestry-first reading, and which is wrong.

Jev scored ancestry-first at **0.140** against 0.820 for class-absolute.
Reader 2 in this repo is that reader, and it was written by the spec's own
author.

**Proposed:** add one sentence making the scope explicit — "Ancestry is
consulted only when two records are of equal class (§5.2 step 3); it
never overrides class precedence." The existing text implies this; the
change makes it say so, which is cheaper than an implementation bug.

---

## What is deliberately NOT being changed

- **§2.5's byte example** — the bytes are right. Only the digest is wrong.
- **The four pinned §3.5 leaves** — all four verify.
- **§1 id validation** — Jev scored it 0.680 / 0.310, too close to call.
  An open question stays an open question.
