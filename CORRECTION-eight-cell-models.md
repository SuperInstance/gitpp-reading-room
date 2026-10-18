# CORRECTION — there are eight cell models, not three, four, or five

*2026-10-18T15:19Z. `SuperInstance/intercontext-map` measured the corpus on
2026-10-18 and drew the graph. The reading room's cell model — which is
FINDING-26 and `cell-history/CELL-HISTORY.md`, the two documents the whole
series stands on — was never updated from it. This carries the count across,
and it is **eight**, with every node cited to a file:line.*

---

## 1. The claims as the record carries them

> # FINDING-26 — three cell models, one charter  (`FINDING-26.md:1`)

> # CELL-HISTORY — a trace of the three (and four) cell models in `SuperInstance`
> (`cell-history/CELL-HISTORY.md:1`)

> is a *rule about how to build*, and the fleet has three cell models
> (`FINDING-26.md:116`)

> | 86 | **A charter that says "don't fork, port" is a claim about a model,
> and a model is only one model if it has an address.** Three cell models with
> three addressing schemes is three models, whatever the vocabulary shares.
> (`FINDING-26.md:161`)

Plus `PLAN.md:36` and `CORRECTION-cell-history.md:3`.

**There is no "five cell models" claim anywhere in this record.** Three,
or "three (and four)" in the title. The count is understated, and the
brief that prompted this pass named a fifth that was never written down.

**Why it was believed:** three cell models is what you get from the repos
the atlas had cloned. `SuperInstance/intercontext-map` was built later, from
a wider corpus, and the reading room was not revised against it. **Two
records of the same fleet, measured on the same day, disagreeing, with
neither knowing about the other.** That is the defect worth naming; the
count is just the visible part.

## 2. What the map measured

`SuperInstance/intercontext-map/intercontext.json`, blob
`6622a8fe1f16e34df7bd1bfe1bfe44dae440eea2`, 74,253 B, `measured_at:
2026-10-18`:

```
counts:    { "nodes": 48, "edges": 70 }
node_kinds: { "cell_model": 8, "witness_dialect": 5, "chain": 3, "media": 5,
              "charter_claim": 11, "organization": 8, "npm_scope": 4,
              "npm_packages": 1, "device_feature": 3 }
```

**48 nodes / 70 edges, and `cell_model: 8`.** All eight carry
`verified: true` and per-node `evidence` arrays. Here they are, in full:

| # | node | repo | ID | ADDRESS | evidence |
|---|---|---|---|---|---|
| 1 | `m.spreadsheet` | `quilt` | — | ✓ `"compass.heading"` | `packages/core/src/types.ts:43,:47`, `package.json:2`, `src/cells/` — **8 kinds** |
| 2 | `m.agent` | `quilt-claw` | — | — | `src/types.ts:21` `witness_hash`, `src/cells/distiller.ts:113`, `src/store.ts:19` |
| 3 | `m.openjev` | `AI-Writings` | — (random `cell-{uuid4[:8]}`) | ✓ `A1` / `Sheet1!C3` | `openjev/cell.py:34`, `spreadsheet_plugin.py:23`, `:4` |
| 4 | `m.game` | `cargo-line-tycoon` | ✓ FNV-1a-64 | — | `substrate/ts/src/index.js:65,:68,:69,:74` |
| 5 | `m.qthe` | `qthe` | — | ✓ `(slot, x, y, resonance)` | `qthe-core/kernel/qthe.mjs:102,:121,:123`, `cloudflare/src/worker.js:198` |
| 6 | `m.receipt` | `AI-Writings/labs` | ✓ content hash | — | `labs/quilt-kernel/quilt_kernel.py:221,:257,:259` |
| 7 | `m.quilt_cells` | `quilt-cells` | — | — | `QUILT.md:48,:145,:153` — **docs only, no code** |
| 8 | `m.corpus` | `AI-Writings` | — | — | `cellular-first-design/` 109 entries, `demos.md`, `test_openjev.py:48` |

**The three the record names are #1, #2 and #4** — spreadsheet, agent,
game. **Five were missing: `m.openjev` (#3), `m.qthe` (#5), `m.receipt`
(#6), `m.quilt_cells` (#7), `m.corpus` (#8).**

Three of those five are not new models at all — **they were already in this
record, filed under other headings**:

- **`m.openjev`** is `CELL-HISTORY.md` row 10, *"5. the ancestor `Cell`"*,
  and row 9, the `00_MASTER_VISION.md` cell. `CORRECTION-address-vs-id.md`
  has it in its addressing table as *"the openjev ancestor cell"*.
- **`m.qthe`** is in `CORRECTION-address-vs-id.md` §1 and §2, as
  *"`qthe`'s lattice cell"*, and is the whole basis of that correction's
  claim that the canon's recognition mechanism is `qthe`'s wormhole table.
- **`m.receipt`** is `CELL-HISTORY.md`'s `AI-Writings labs` reference and
  the corpus the PRICED ladder runs in.

**So the record already knew five cell models and had filed them under
three headings.** The count was never wrong so much as **un-tabulated** —
and `FINDING-26`'s conclusion ("three models, three addressing schemes,
therefore three models") was drawn from a list that had been silently
partitioned by which document a model appeared in rather than by which
model it is.

**#7 and #8 are new and they are different in kind.** `m.quilt_cells` is
**docs only, no code** — a cell contract described in `QUILT.md` and never
built. `m.corpus` is **not a model but 109 demos that change under
themselves.** Counting a corpus as a cell model is arguable; **counting a
document that specifies a cell and has no implementation is exactly the
error `FINDING-26` was written to catch**, and it is the reason the eight
deserve to be on the list rather than quietly rounded to six.

## 3. The corrected axes

`CORRECTION-address-vs-id.md` §2 got this right and it survives: the
models sit on **two independent axes**, and the counting error hid the
structure because it counted documents instead of models.

| model | ID (content hash) | ADDRESS (a place) |
|---|---|---|
| `m.spreadsheet` | — | ✓ a chosen string |
| `m.agent` | — | — |
| `m.openjev` | — (identity is **random**) | ✓ a spreadsheet coordinate |
| `m.game` | ✓ FNV-1a-64 — *in a method misnamed `computeAddress()`* | — |
| `m.qthe` | — | ✓ slot + resonance |
| `m.receipt` | ✓ content hash + activation | — |
| `m.quilt_cells` | — (docs only) | — |
| `m.corpus` | — | — |

**Five addressing schemes** (`CORRECTION-cell-history.md` §2, unchanged) —
and **two models with neither**, which the three-model table could not show
because one of them did not exist in the table.

## 4. The consequence that changes a conclusion

`FINDING-26.md:161`, rule 86, is the load-bearing sentence of the whole
series:

> Three cell models with three addressing schemes is three models, whatever
> the vocabulary shares.

**The rule survives and its arithmetic changes.** The corrected form:

> **Eight cell models with five addressing schemes is eight models, and two
> of the eight have neither axis.**

That is a *stronger* finding, not a weaker one. The original said the
charter describes a model the fleet does not have. The corrected version
adds: **four of the eight never had an address and were never going to get
one from the charter, because the charter's address is a place and four of
the eight are places already or are documents.** And `m.quilt_cells` is the
proof that the failure mode is *reachable*: a cell contract can be written,
specified in prose, and never implemented, and the record's three-model
frame had no way to hold that.

`FINDING-26.md:116`'s conclusion — *"the fleet has three cell models built
by different routes, with the parity harness on the one that was easiest to
make four copies of"* — survives verbatim. **The asymmetry finding, which is
the actual point of FINDING-26, is untouched by this correction.** What
changes is the denominator, and the denominator is what made "three"
feel complete.

## 5. Where else the record depends on the count

| place | text | status |
|---|---|---|
| `FINDING-26.md:1` | title: "three cell models, one charter" | corrected in place |
| `FINDING-26.md:116` | "the fleet has three cell models" | corrected in place |
| `FINDING-26.md:161` | rule 86 | corrected in place; rule survives |
| `cell-history/CELL-HISTORY.md:1` | title: "three (and four)" | corrected in place |
| `PLAN.md:36` | index row | corrected in place |
| `CORRECTION-cell-history.md:3` | dateline | corrected in place |
| `CORRECTION-cell-history.md:120` | "the three models met" — the weld | **survives**: `_worker.js` still welds agent kinds to canon opcodes |
| `CORRECTION-address-vs-id.md` | two-axis tables | **survives** — the axes were never in dispute |

## 6. The rules this correction adds

| # | rule |
|---|---|
| 108 | **Two records of one fleet, measured the same day, will disagree, and neither will know about the other.** `intercontext-map` (2026-10-18) and this room (2026-10-18) counted cells differently for eight months' worth of models. Name the other record or you will re-derive it. |
| 109 | **Count models, not documents.** The three-model list was partitioned by which document each model appeared in. `openjev`, `qthe` and the receipt cell were all in this record already — filed under other headings. |
| 110 | **A model specified in prose and never built is a cell model.** `quilt_cells` has `QUILT.md:48,:145,:153` and no code. Excluding it because it has no implementation is the error FINDING-26 exists to catch. |
| 111 | **When a later map revises an earlier finding, the earlier finding is not wrong — it is under-scoped.** FINDING-26's asymmetry claim survives intact. Only its denominator moved. Say which parts survive, or you will throw away the part that was right. |
