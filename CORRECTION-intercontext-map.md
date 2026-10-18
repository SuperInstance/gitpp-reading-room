# CORRECTION — the map this record was corrected *from* has four errors of its own

*2026-10-18T15:19Z. This correction was commissioned from
`SuperInstance/intercontext-map`, whose `CLAIMS.md` §"Corrections to the
brief, measured" is, receipt for receipt, the list of six claims I was
sent. **Four of its six receipts do not survive re-measurement**, and two
of the corrections I was sent to write are wrong as stated. The map is not
touched — this room only corrects the record — but the receipts were
inherited and the inheritance is the finding.*

---

## 1. Where the brief came from

`SuperInstance/intercontext-map/CLAIMS.md:51-60`, verbatim:

| brief says | measured | evidence |
|---|---|---|
| "implemented 0 Rust" | **9 of 11**, `COUNT = 9`, with an `Engine` and law checks | `quilt-rust/crates/quilt-polyformalism/src/lib.rs:86`, `:195` `ALL_OPS: [Op; 9]`, `:212` `law_bind_idempotent` |
| "`CONTEST MERGER REVOKE DELEGATE WITHDRAW` never implemented anywhere" | **implemented** — `ATTEST:229`, `CONTEST:242`, `WITHDRAW:251` | `cargo-line-tycoon/substrate-bundle/substrate-sdk.js` (11,375 B) |
| "`algebra.md` ONE commit, ever" | **2 commits** | `1da6180bb7`, 2026-10-18T07:46:49Z |
| "charter witness has **no result**" | has `result?: any` — added ~2 h before this map | `algebra.md:88`, from `1da6180bb7` |
| "`PRICED-TRAINING.md`" | **zero commits**; the file is `PRICED-STANDARD.md` | `situations/arch/PRICED-TRAINING.md` — 404 on the commits API |
| "`AI-Writings/…`" | the org is 404; the repo is `SuperInstance/AI-Writings` | `api.github.com/orgs/AI-Writings` |
| "there are **five** witness dialects' worth of disagreements — three" | **four** dialects; openjev's was not listed | `cellular-first-design/code/openjev/cell.py:46-53` |
| "`parse_address` in `openjev/cell.py`" | it is in `code/spreadsheet/spreadsheet_plugin.py:23` | openjev/cell.py has the *identity*, the plugin has the *address* |

**So this file corrects the corrections.** I inherited eight receipts and
re-measured every one.

## 2. What survives, exactly

| receipt | verdict |
|---|---|
| Rust is 9, `ALL_OPS` at `:195`, `law_bind_idempotent` at `:212` | ✅ **verified exactly.** `COUNT = 9` is at **`:85`**, not `:86` — off by one, immaterial |
| `algebra.md` has 2 commits, `1da6180bb7` | ✅ **verified exactly** |
| charter witness has `result?: any` from `1da6180` | ✅ **verified exactly** |
| `parse_address` is in `spreadsheet_plugin.py:23`, not `cell.py` | ✅ **verified exactly** — and **already correct in this room**; see [CORRECTION-phantom-ladder](CORRECTION-phantom-ladder.md) §4 |
| the org `AI-Writings` is 404 | ✅ **verified** — `GET /orgs/AI-Writings` → 404 |
| the 9 ops are eleven A minus `WORLD`/`TIME` | ✅ verified, and the sharpest form of the claim; **not stated in the map**, stated here |

## 3. What does not survive

### 3a. "…never implemented anywhere → **implemented**" — over-claimed by three

The map's row asserts all **five** are implemented and cites **three** line
numbers. Re-measured against all three copies of the blob (`browser/`,
`browser-deploy/`, `substrate-bundle/` — all 11,375 B, all `c94654e3520d`):

| opcode | in the file? |
|---|---|
| `BIND` `:217`, `ATTEST` `:229`, `CONTEST` `:242`, `WITHDRAW` `:251` | ✅ present, real bodies |
| **`MERGER`, `REVOKE`, `DELEGATE`** | ❌ **0 occurrences** |

**The map generalised three receipts to five opcodes.** Had this row been
copied forward uncritically it would have replaced *"3 of 5
unimplemented"* with *"0 of 5"* — **overstating the fleet's implementation
by three opcodes, in a correction.** This is the house's defect in its
purest form: the false claim was load-bearing and it was in the corrective
document.

### 3b. "`PRICED-TRAINING.md` … 404 on the commits API" — wrong twice

- `GET …/commits?path=PRICED-TRAINING.md` returns **HTTP 200 with `[]`**.
  It does not return 404. 404 is not what that endpoint does for a missing
  path, and a receipt that names the wrong status code cannot be checked.
- **The file exists.** `situations/arch/PRICED-TRAINING.md`, 17,693 B,
  blob `68bcaf3d367acebb6562c3f81d0264221ed68344`, one commit
  `6fc5e5b3325f` (2026-09-30T20:08:40Z), on branch `claude/priced-training`.
  The map checked the default branch and a path at the root, and the file is
  at neither.

The same row appears at `CLAIMS.md:29` as *"`situations/arch/PRICED-STANDARD.md:100`
cites `PRICED-TRAINING.md` — **0 commits on that path, ever** … the file it
is quoted from **has never existed in the repository**."* **The accurate
defect is a dangling citation between two files split across a branch
boundary** — see [CORRECTION-phantom-ladder](CORRECTION-phantom-ladder.md)
§3. That is a smaller claim and a real one; "never existed" is neither.

### 3c. "four dialects" vs the map's own `node_kinds: 5`

`MAP.md:78` reads *"The witness dialects — **four, not three**"* and gives a
four-row table; `MAP.md:91` then adds *"And a **fifth** vocabulary"*;
`CLAIMS.md:59` says *"**four** dialects."* But `intercontext.json` says
`"witness_dialect": 5` and enumerates five nodes — the fifth,
`w.reltypes`, being the nine *relations* filed under `witness_dialect`
alongside the four *dialects*.

**Neither is wrong and the map does not say which is which.** Four dialect
*shapes* and one opcode *vocabulary* is the precise statement; the JSON's
count of 5 conflates them. Recorded here rather than resolved in the map,
because this room does not edit the map.

### 3d. "share **zero names** with the charter's eleven" — wrong of eleven A

`MAP.md:91-93`. Measured:

| | overlap with `TASK ROUTE DRAFT DRAW FOLD KEEP DROP MARK OUTCOME` |
|---|---|
| eleven **A** (`algebra.md:13`) | **`ROUTE`** |
| eleven **B** (`wr-obs13`) | zero |
| eleven **C** (`massive_simulation.js:16`) | zero |

The charter's eleven contains `ROUTE`, and so does `quilt-rust`'s
nine-opcode port (`Op::Route => "ROUTE"`, `lib.rs:79`). **"Zero names" is
true of B and C and false of A, and A is the eleven the real ports were
written against** — so the one overlap is the most informative word in the
sentence.

### 3e. Omissions the correction brief did not carry

- **Eleven C has a second home.** `cargo-line-tycoon/POLYFORMALISM.md:58`
  states the same eleven C in prose. The map cites only
  `massive_simulation.js:16`.
- **The fifth Rust port is a `no_std`/`wasm` claim**, and the file's own
  header says `## The 5 laws` while defining six — see
  [CORRECTION-rust-is-nine](CORRECTION-rust-is-nine.md) §3.
- **`wr-obs13:17` says "Five candidates" and names six.**

## 4. What the map got right, and this room did not

**The map is the better record and this room was the stale one.** Stated
plainly, because a correction that only lists faults is not a measurement:

1. **`MAP.md` §1 tabulates all eight cell models** and never states a count.
   This room said three. See [CORRECTION-eight-cell-models](CORRECTION-eight-cell-models.md).
2. **`MAP.md` §1 corroborates `CORRECTION-address-vs-id` §2 exactly** — the
   two-independent-axes structure, and *"no model in the corpus has both"*,
   which is the correction's sharpest sentence. **That correction was right,
   and the map independently confirms it.**
3. **`CLAIMS.md:30` caught the `algebra.md` second commit** — *"a witness must
   carry the answer *(new, `algebra.md:88`)* … 2 of 4 dialects, 2 h old"* —
   which this room missed, because it was written before or independently of
   the merge. The map's own note *"(2 h old)"* is the receipt.
4. **`intercontext.json`'s `m.game` evidence names the misnomer** —
   *"in a method MISNAMED `computeAddress()`"* — which is
   `CORRECTION-address-vs-id`'s central claim, cited to `:65,:68,:69,:74`.

## 5. The rules this correction adds

| # | rule |
|---|---|
| 121 | **Re-measure the receipts you inherited, especially the ones you were told were already verified.** Four of eight receipts in `intercontext-map/CLAIMS.md` did not survive. The map is a scout's report like any other, and this room's own house rule — *"artifacts require independent verification"* — applies to the correcting document with exactly the force it applies to the corrected one. |
| 122 | **Three receipts do not generalise to five findings.** `CLAIMS.md:54` cited `ATTEST:229`, `CONTEST:242`, `WITHDRAW:251` and asserted five opcodes. `MERGER`, `REVOKE` and `DELEGATE` are absent from the file. A citation is not an enumeration; count the ones you actually looked at. |
| 123 | **A corrective document that disagrees with its own data model is the highest-risk kind of citation.** `MAP.md` says four dialects, `intercontext.json` says five, and neither names the distinction. When a map's prose and its JSON disagree, the *distinction* is the finding. |
| 124 | **Two records of a moving fleet drift apart silently.** `intercontext-map` (measured 2026-10-18) and this room (2026-10-18) disagree on cell models, witness dialects and eleven count. Neither knows about the other. **When one corrects the other, the corrected one is the stale one — and the map should be cited by name so the next pass checks both.** |
