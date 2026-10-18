# CORRECTION — `algebra.md` has two commits, and the second one is the house's own rule

*2026-10-18T15:19Z. Five statements in this record say `algebra.md` has
**exactly one commit, ever**. It had exactly one commit for 27 days and has
had two for **seven hours**, because a merge landed during this very
session. This is the cleanest live example in the record of the rule the
room already holds: **a claim that was true and stopped being true is not a
mistake — it is a timestamp that was never written down.***

---

## 1. The claim, in all five places it appears

| place | text |
|---|---|
| `CORRECTION-cell-history.md:15` | "`algebra.md` — the charter's own formal reference, which has **exactly one commit, ever**" |
| `CORRECTION-address-vs-id.md:12` | "`AI-Writings/algebra.md` — the charter's formal reference, 5,294 bytes, **exactly one commit, ever**" |
| `CORRECTION-address-vs-id.md:182` (rule 95) | "`algebra.md` is 5 KB, one commit, and it is the only document in the fleet that states the model." |
| `cell-history/CELL-HISTORY.md:45` | "`algebra.md` has **exactly one commit, ever**" |
| `cell-history/CELL-HISTORY.md:90` | "`algebra.md` was **never revised** (1 commit, ever). So the corpus simultaneously asserts both." |
| `cell-history/CELL-HISTORY.md:349` | "`algebra.md` has exactly one commit and still lists six A" |

**All six are annotated in place, dated, and none is deleted.** The claim
was true when written — `CORRECTION-cell-history.md` and
`CORRECTION-address-vs-id.md` are both dated 2026-10-18, and the merge
landed at **07:46:49Z** the same day.

## 2. What was measured

```
GET /repos/SuperInstance/AI-Writings/commits?path=algebra.md
  COUNT: 2
  1da6180bb77a  2026-10-18T07:46:49Z  "Make the witness carry the answer, and witness what the tick adopts"
  3807557abd07  2026-09-21T14:46:45Z  "WIP: cellular-first-design + selected canon content"
```

**27 days and 1 day, 17 hours, 6 minutes.** The second commit touched two
files: `algebra.md` **+13 / −0** and `_worker.js` **+48 / −0**.

```
  AI-Writings/algebra.md
  @@ -75,6 +75,19 @@ type Witness = {
     op: 'BIND' | 'LINK' | 'EFFECT' | 'VIEW' | 'TICK' | 'FORGET' | 'PROOF' | 'ROUTE' | 'CRDT' | 'WORLD' | 'TIME';
     cell: string;
     params: Record<string, any>;
+    // What the operation PRODUCED. A witness that records only the request
+    // witnesses an intent, not an outcome -- and an intent is not the
+    // substrate of truth for anything.
+    //
+    // This is the field that makes an ADOPTED system teachable. When a
+    // cell defers to a model or a service, the request is cheap to repeat
+    // and worthless on its own; it is the ANSWER, recorded once and
+    // chained, that lets a later reader -- a smaller model, a rule engine,
+    // no model at all -- reach the same result without paying again.
+    //
+    // Optional, because not every operation produces a value worth
+    // keeping (FORGET produces nothing). Present when there was one.
+    result?: any;
     hash: string;
     prev_hash: string;
     sig: string;  // optional cryptographic
```

**One field, `result?: any`, added to the charter's `Witness` type.** The
author's own commit message calls it *"the formal reference's second
commit, ever"* — **so the person who made the merge updated the record
about itself, and the record in this room was written before or
independently of that note.**

**And the merge touched only two files — which is the useful part.**
Re-measured the same way:

| file | commits after `1da6180` |
|---|---|
| `algebra.md` | **2** (was 1) |
| `_worker.js` | 2+ |
| **`AGENTS.md`** | **still 1** — `3807557abd`, 2026-09-21T14:46:45Z |
| **`ENGINEERING.md`** | **still 1** — `3807557abd`, 2026-09-21T14:46:45Z |

**So `CELL-HISTORY.md:76`'s *"Source: `SuperInstance/AI-Writings/AGENTS.md`,
6,208 B, one commit `3807557a`"* is CORRECT and is not corrected here.** The
charter's *formal reference* was revised; the charter's *statement* documents
were not. `1da6180` added a field to a type. **It did not change what the
charter says about cells, opcodes, kinds or addresses** — which is exactly
why the "three elevens" and "26 kinds" findings survive the merge intact, and
why this correction had to be narrow.

Line `algebra.md:76` in the diff context is also the receipt that the
charter's eleven is **eleven A** — `BIND LINK EFFECT VIEW TICK FORGET
PROOF ROUTE CRDT WORLD TIME` — unchanged, in the very patch that proves the
file was touched. **`algebra.md` still lists six A.** So the "two elevens"
finding survives; the *revision* claim does not.

## 3. Why this one is the most valuable correction in the set

Every other correction in this batch is a **measurement error**: a count
taken in the wrong directory, a branch not searched, a repo that does not
exist. Those are ordinary, and the fix is a better search.

**This one is different: the record was correct and time moved.** It cannot
be fixed by searching harder. The only fixes are (a) write the measurement
date on every count, which this record already does in its datelines, and
(b) accept that a durable record about a moving subject has a shelf life
that has to be stated.

**And it is the only one where the falsifying event happened *inside the
window this very correction pass was measuring*.** The merge at
07:46:49Z; my measurements at 15:19Z. Seven and a half hours apart, same
session, same account. **A reader who took `algebra.md` as one-commit and
built `CORRECTION-address-vs-id` §2 — the ID-versus-address correction —
on the strength of it was building on a claim whose shelf life had already
expired, and did not know.**

This is why the house's rule exists in the form it does:

> **inside a repository, incompleteness is fine. The defect is a sketch
> that misrepresents its state.**

`algebra.md` with one commit did not misrepresent anything on 2026-09-22.
It misrepresents nothing now either — **the claim moved with the subject,
and the record did not move with the claim.** The fix is a date and a
pointer, not a rewrite, and the six original sentences stay in place so the
next reader can see exactly which reasoning was done against which state.

## 4. The consequences that follow the merge

Three things in this record are now out of date *because of* `1da6180`, and
they are annotated in place:

1. **`CORRECTION-address-vs-id.md:73`** — `| 11 opcodes | 11 (two elevens,
   one never revised) |` — "never revised" is now **false**. Eleven A was
   revised (a field was added to its `Witness` type); its **opcode list was
   not**.
2. **`PLAN.md:266`** — `| 3 | 3 witness dialects that disagree; the
   charter's carries no result | **fixed** in AI-Writings PR #84 |` — the
   charter's dialect now **carries a result**, as of 07:46:49Z. The dialect
   count is wrong for a different reason; see
   [CORRECTION-the-elevens-remeasured](CORRECTION-the-elevens-remeasured.md).
3. **`intercontext-map/CLAIMS.md:30`** — *"a witness must carry the answer
   *(new, `algebra.md:88`)* … **2 of 4 dialects**, 2 h old"* — correct, and
   the map is the only record that noticed, because it was written after
   the merge. **That is the whole reason this room missed it too.**

## 5. The rules this correction adds

| # | rule |
|---|---|
| 117 | **A count about a moving subject carries the date it was taken, and the date it stops being true.** `algebra.md` had one commit for 27 days and two for one day. Both are true. Neither is durable without a timestamp. |
| 118 | **"Never revised" and "the thing you care about was not revised" are different claims.** `algebra.md` was revised at 07:46:49Z and **still lists eleven A**. The opcode drift finding survives its own falsification, which is rarer than it sounds. |
| 119 | **When the falsifying event lands inside the measurement window, say so and date both.** The merge is 7h33m older than this measurement. The gap is the finding, not an inconvenience. |
| 120 | **If a newer map caught it and this room did not, the defect is the parallel record, not the miss.** `intercontext-map/CLAIMS.md` caught the second commit; this room did not, because it was written at a different depth. See [CORRECTION-intercontext-map](CORRECTION-intercontext-map.md). |
