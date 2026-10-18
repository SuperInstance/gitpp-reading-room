# CORRECTION — three elevens, four dialects, and two of the five "unimplemented" opcodes

*2026-10-18T15:19Z. This corrects the claim that CONTEST / MERGER / REVOKE /
DELEGATE / WITHDRAW are **never implemented anywhere**, the claim that there
are **two** elevens, and the count of witness dialects. Two of the five are
confirmed implemented; **three are not**, and the brief that prompted this
pass over-claimed. Both directions are recorded here.*

---

## 1. The claim as the record carries it

> `CONTEST`, `MERGER`, `REVOKE`, `DELEGATE`, `WITHDRAW` are **never
> implemented** — they appear in `docs/DIALECTIC_IN_11_OPCODES.md` as "6
> proposed (R11 derivative)" and nowhere else in code.
> — `cell-history/CELL-HISTORY.md:79`

> **Worse: the canon's eleven is not stable.** There are **two different
> elevens**, seven hours apart
> — `cell-history/CELL-HISTORY.md:82`

> "There are 11 opcodes" — there are **two** elevens seven hours apart
> — `README.md:58`

and the inventory carries both: `PLAN.md:268`.

**Why it was believed:** the search that produced it was
`grep 'ATTEST CONTEST MERGER'` and `grep 'ATTEST CONTEST'` over the *three
repos the atlas had cloned*. `substrate-sdk.js` is in `cargo-line-tycoon`,
and its `browser/` copy had not been cloned at the time the search was
written — the tree under `substrate/` was measured, the tree beside it was
not. **A search over `substrate/` cannot find a file in `browser/`.**

## 2. What is actually implemented — and what is not

`SuperInstance/cargo-line-tycoon/browser/substrate-sdk.js`, 11,375 B, 371
lines, blob `c94654e3520d`. **Three copies exist and all three are that one
blob** — `browser/`, `browser-deploy/`, `substrate-bundle/`, all 11,375 B,
all `c94654e3520d`. The map cited `substrate-bundle/`; all three verify.

```
217:   BIND(aId, bId) {
229:   ATTEST(cellId, evidence) {
242:   CONTEST(cellId, reason) {
251:   WITHDRAW(cellId) {
```

Each is a real method with real state, not a stub:

| line | opcode | body |
|---|---|---|
| 217 | `BIND` | mutual `bound_to` push, `this.set` both sides, idempotent via `.includes()` |
| **229** | **`ATTEST`** | pushes `{...evidence, hash: '0x'+fnv1a64(...).padStart(16,'0'), timestamp: Date.now()}` onto `cell.state.attestations` |
| **242** | **`CONTEST`** | pushes `{reason, timestamp}` onto `cell.state.contestations` |
| **251** | **`WITHDRAW`** | sets `cell.state.withdrawn = true`, `withdrawnAt` |

**The claim is FALSIFIED — for two of the five.**

| opcode | verdict | receipt |
|---|---|---|
| `CONTEST` | **falsified — it IS implemented** | `browser/substrate-sdk.js:242` |
| `WITHDRAW` | **falsified — it IS implemented** | `browser/substrate-sdk.js:251` |
| `MERGER` | **the claim SURVIVES** | 0 occurrences of the string in the file |
| `REVOKE` | **the claim SURVIVES** | 0 occurrences of the string in the file |
| `DELEGATE` | **the claim SURVIVES** | 0 occurrences of the string in the file |

**The brief that prompted this correction said all five "ARE implemented,
in `substrate-sdk.js`", and cited line numbers for three of them. Two of
those three line numbers are exact and the fifth, sixth and seventh are
absent from the file.** The receipt I was handed for claim 2 is therefore
**half right**, and the half it got wrong is in the direction that matters:
it would have replaced "3 of 5 unimplemented" with "0 of 5", overstating
the fleet's implementation by three opcodes.

### The correct narrow claim

> `CONTEST` and `WITHDRAW` are implemented, in
> `cargo-line-tycoon/browser/substrate-sdk.js:242,251` — a file the original
> search never looked in, because it sat in `browser/` and not `substrate/`.
> **`MERGER`, `REVOKE` and `DELEGATE` remain unimplemented in every port
> measured**, and no port implements them under any spelling.

Scoping note, measured and not over-claimed: a word-level fleet search
does return `MERGER` 102 files, `REVOKE` 301, `DELEGATE` 400 — but every
one in the top ten per term is unrelated English use (`capability_tokens.py`,
a2a delegation, papers, lyrics). **As cell-opcode identifiers they remain
unimplemented.** I did not enumerate all 5,290 repositories; the honest
form of this claim is "in every port measured", not "nowhere in the fleet".

## 3. There are THREE elevens, not two

The record says two. There are three, and the third is the only one in
**running code**.

| | where | the eleven | written |
|---|---|---|---|
| **A** | `AI-Writings/algebra.md:6,13` | BIND LINK EFFECT VIEW TICK **FORGET PROOF ROUTE CRDT WORLD TIME** | `3807557abd` — 2026-09-21T**14:46:45**Z |
| **B** | `AI-Writings/cellular-first-design/reports/wr-obs13-missing-opcodes.md:9,11,13,15,17` (3,583 B) | BIND LINK EFFECT VIEW TICK **ATTEST CONTEST REVOKE WITHDRAW MERGE DELEGATE** | `46766b5605` — 2026-09-21T**22:01:18**Z |
| **C** | **`cargo-line-tycoon/tests/sim/massive_simulation.js:16`** | **ATTEST CONTEST MERGER REVOKE WITHDRAW DELEGATE** BIND LINK EFFECT VIEW TICK | `595f6f32` — 2026-10-03 |

A and B are 7 h 14 m 33 s apart. **C is three weeks later and is not B:**
B says `MERGE`, C says **`MERGER`** — one letter, two vocabularies. And C
is not A either: it merges B's six into A's five and drops A's five,
producing an eleven that is **neither A nor B**.

```
  cargo-line-tycoon/tests/sim/massive_simulation.js
  5: const sub = require('../../substrate/ts/src/index.js');
 16: const OPCODES = ['ATTEST','CONTEST','MERGER','REVOKE','WITHDRAW','DELEGATE','BIND','LINK','EFFECT','VIEW','TICK'];
```

**This is worse than a third prose list, because it is executable.** Line
16 is the array the simulation iterates. The fleet's simulation **dispatches
an eleven that no document in the canon ever asserts** — and `:5` shows it
dispatching into the 6-opcode TypeScript port, where `apply()`'s
`default:` throws `Unknown opcode`.

And C is stated a **second time**, in prose, in the same repo:

> `cargo-line-tycoon/POLYFORMALISM.md:58`
> "Every action is one of the 11 opcodes (BIND, LINK, EFFECT, VIEW, TICK +
> ATTEST, CONTEST, **MERGER**, REVOKE, DELEGATE, WITHDRAW)"

**So eleven C has two homes — a test array and a design doc — and no
adoption commit.** Still: **no adoption commit exists**, exactly as
`CELL-HISTORY.md:351` says. What changes is the count: three, not two.

## 4. There are FOUR witness dialects — and the map's own data says five

`PLAN.md:266` says **3**. The record has always said three.

`cell-history/CELL-HISTORY.md:42` records the fourth in passing and never
promotes it: *"Opcodes observed as `opcode:` strings `WITNESS`, `PROOF`."*
That is a dialect and it was sitting in the record the whole time, in a
table row, unremarked.

| # | dialect | where | chained? | carries the result? |
|---|---|---|---|---|
| 1 | the charter's | `algebra.md:72` | **yes** (`prev_hash`) | `result?` — **added 2026-10-18T07:46:49Z** |
| 2 | the live canon's | `_worker.js:181` | **no** (0 hits for `prev_hash`) | only on TICK (`:447`) |
| 3 | the labs' | `recorder.py:102` | **yes** | **yes** (`DRAFT{prompt, response, params}`) |
| 4 | **openjev's** | `cell.py:46` | **no** | **yes** (`value`) |

```
  AI-Writings/cellular-first-design/code/openjev/cell.py
  46:     def witness(self, value: Any, source: str = "self") -> Dict:
  48:         entry = {
  49:             "ts": datetime.now(timezone.utc).isoformat(),
  50:             "opcode": "WITNESS",
  51:             "value": value,
  52:             "source": source,
  53:             "jev_confidence": self.jev_confidence,
  54:         }
```

**No hash field. No chain. And `opcode` is `"WITNESS"` — which is not a
charter opcode.** `proof()` at `:58` emits `PROOF` the same way; the
charter's `PROOF` at `algebra.md:76` is a member of eleven A. **The word
collides and the shape does not.** That is the most dangerous kind of
dialect: it will pass any grep that looks for the word.

**And a FIFTH node, which is a vocabulary and not a dialect:**

```
  AI-Writings/labs/situation-recorder/recorder.py
  21:     rel ∈ TASK ROUTE DRAFT DRAW FOLD KEEP DROP MARK OUTCOME
  38: REL_TYPES = {"TASK", "ROUTE", "DRAFT", "DRAW", "FOLD", "KEEP", "DROP", "MARK", "OUTCOME"}
```

**A defect in the map that cited it.** `intercontext-map/MAP.md:78` reads
*"The witness dialects — **four, not three**"* with a four-row table, and
then `MAP.md:91` adds *"And a **fifth** vocabulary."* But
`intercontext.json`'s own `node_kinds` counts
**`"witness_dialect": 5`** — it files the nine *relations* under
`witness_dialect` alongside the four *dialects*. So the map's prose says
four and the map's data says five, and **the map does not say which of the
two is right.** It is worth being precise, because it is the only place in
this record where the correction and the correcting document disagree:

| | dialects (a witness entry's *shape*) | vocabularies (its *opcode words*) |
|---|---|---|
| count | **4** | **5** (`node_kinds` counts the vocabulary as a dialect) |
| `PLAN.md:266`'s "3" | falsified | — |

## 5. The nine relations — "zero names" is right of B and wrong of A

The map says:

> And a **fifth** vocabulary: `recorder.py:38` defines nine relations —
> `TASK ROUTE DRAFT DRAW FOLD KEEP DROP MARK OUTCOME` — which share **zero
> names** with the charter's eleven. (`MAP.md:91-93`)

**Measured, and this is wrong of A:**

| | overlap with the nine relations |
|---|---|
| eleven **A** (`algebra.md`) | **`ROUTE`** — one shared name |
| eleven **B** (`wr-obs13`) | **zero** |
| eleven **C** (`massive_simulation.js`) | **zero** |

`ROUTE` is `algebra.md:13`, in FORGET PROOF **ROUTE** CRDT WORLD TIME. So
"zero names" holds against B and C and **fails against the charter's own
eleven by exactly one word** — and that one word is load-bearing, because
it is the same word `quilt-rust`'s nine-opcode port also implements
(`Op::Route => "ROUTE"`, `lib.rs:79`).

**The accurate claim:** the recorder's nine relations share **zero names
with eleven B and eleven C, and exactly one (`ROUTE`) with eleven A** —
which is itself the evidence that A is the eleven the fleet's real ports
were written against.

## 6. What this does to the conclusions that depended on it

1. **"6 of 11 implemented, max"** — now **9** in Rust, and **4 more** in the
   SDK the search missed. See [CORRECTION-rust-is-nine](CORRECTION-rust-is-nine.md).
2. **"there are two elevens"** — **three**, and the third is executable.
3. **"3 witness dialects"** (`PLAN.md:266`) — **four dialects**, and the
   charter's carries `result` only because of a merge two hours before the
   map was written. `PLAN.md:266` also says this is **"fixed in `AI-Writings`
   PR #84"** — the charter half is fixed; the count is still wrong.
4. **The `wr-obs13` count is wrong in its own text:** `:17` says *"Five
   candidates"* and then names six. The record noted this; it is restated
   here because eleven C inherited "five" nowhere and "six" everywhere, and
   that inconsistency is still load-bearing.

## 7. The rules this correction adds

| # | rule |
|---|---|
| 103 | **A search scoped to a directory is not a search of a repository.** `substrate-sdk.js` lives in `browser/`. Grepping `substrate/` returned "unimplemented anywhere" for two opcodes that run. |
| 104 | **Check the opcodes you could NOT find with the same rigour as the ones you could.** "Never implemented" is a claim about absence, and absence claims need the wider search that a presence claim does not. The receipt I was handed asserted five found and cited three. |
| 105 | **A list in running code outranks a list in prose.** `massive_simulation.js:16` is an array a program iterates. `wr-obs13` is a report. When they disagree by one letter, the executable one is the finding. |
| 106 | **A word collision across dialects is worse than a word mismatch.** openjev emits `opcode: "WITNESS"`; the charter has no `WITNESS`. It will pass every grep for the string and fail every test of the shape. |
| 107 | **When the correcting document disagrees with its own data, say which is which.** `MAP.md` says four dialects, `intercontext.json` counts five. Neither is wrong; the record must not pick silently. |
