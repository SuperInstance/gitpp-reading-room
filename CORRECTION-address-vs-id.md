# CORRECTION — the hash is the cell's ID, not its address, and I had them the wrong way round

*2026-10-18. This corrects the CENTRAL CLAIM of
[FINDING-26](FINDING-26.md) and [CORRECTION-cell-history.md](CORRECTION-cell-history.md).
I read the charter's formal reference too late.*

---

## 1. What I got wrong

`AI-Writings/algebra.md` — the charter's formal reference, 5,294 bytes,
**exactly one commit, ever** — says:

> ```
> ## The Address Is the Data
> The location of a cell IS its data. Two cells with the same
> id and kind ARE the same cell. There is no separate "store".
> ```
> ```
> cell.id = "ack:7"
> cell.kind = "ack"
> cell.payload = { timeoutMs: 3000 }
> ```
> The triple `(kind, id, payload)` IS the cell.

and, under *Engineering Cleverness*:

> ```
> 1. Cell ID = hash of (kind, payload, parents) — no separate ID service
> 2. Witness log = append-only — no DB, just files
> 3. Address = (x, y, t, sig) — 4D navigation without a map
> 4. Op = pure function over Cell × args — easy to test
> 5. Polyformalism = same model, 12 languages — proves it's truly minimal
> 6. Lattice = JSON graph — anyone can read, write, host
```

**The hash is the cell's ID. The ADDRESS is a 4D PLACE — `(x, y, t, sig)` —
"4D navigation without a map".** They are two different properties, and
they are independent.

**I had them the wrong way round.** I wrote that "the game's FNV-1a-64
substrate is the model with the address" and that `@quilt/core`'s string
id and the openjev cell's `Sheet1!C3` were "different addressing
schemes" — as if the hash were *the* address and the place-addresses
were deviations from it.

**The truth is the reverse.** The game's `computeAddress()` computes a
**content hash — the ID — and is misnamed.** The place-address is the
charter's address. The fleet's *older* models implemented the ADDRESS
and not the ID; the *newer* one implemented the ID and not the address.

## 2. The axis, corrected — and it is two axes, not one

| model | when | **ID** (content hash) | **ADDRESS** (a place) |
|---|---|---|---|
| `@quilt/core` | 2026-08-17 | — | **✓** `"compass.heading"` — a bearing |
| `quilt-claw` (agent cell) | 2026-09-17 | — | — |
| the openjev ancestor cell | 2026-09-21 | — (identity is **random**: `f"cell-{uuid4().hex[:8]}"`) | **✓** `Sheet1!C3` — a coordinate |
| the game's substrate | 2026-09-27 | **✓** FNV-1a-64 over `JSON.stringify({s,t,b})` — *named `computeAddress()`, which is the misnomer* | — |
| `qthe`'s lattice cell | 2026-09-27 | — | **✓** slot + resonance — a place in a table |

**So the models are not three portraits of one drifting model.** They sit
at different points on **two independent axes**, and the fleet's
history is a *hand-off*: the place-address was there first, the
content-hash arrived later in one repository, and the agent cell — the
one that does the actual work — got neither.

**And every clause of the charter is under-implemented, each in a
different way:**

| charter clause | documented | implemented |
|---|---|---|
| 11 opcodes | 11 (two elevens, one never revised) | **6** TS / **5** C99 / **1** Python / **0** Rust |
| 26 kinds | 26 | **25** dirs / **25** zoo cards / **24** validator |
| `Address = (x, y, t, sig)` | yes | **nowhere** — no repo has it |
| 4D lattice, **three projections** TOP(x,y) / FRONT(signal) / SIDE(time) | yes | **nowhere** |
| `Cell ID = hash of (kind, payload, parents)` | yes | the game, as `computeAddress()`, hashing `{s,t,b}` instead |
| "Don't store cells in a DB — witness log + derived state" | yes | `quilt-claw` keeps a `store.ts` |

**The 4D address and the three projections are the parts of the charter
with NO implementation at all.** I had spent the session treating the
one clause that IS implemented — the ID hash — as the reference.

## 3. Why the place-address is the one that scales to a device, and the hash is not

This is not a stylistic preference. It is arithmetic.

**An ESP32-S3 has 512 KB of internal SRAM, a dual-core Xtensa LX7 at
240 MHz, and 16 MB of flash.** A 0.5B-parameter Qwen2 has been run
across **seven** of them: one board holds the tokenizer and embeddings,
the other six hold four transformer layers each, using **BitNet
b1.58** — ternary weights, `-1 / 0 / +1` — because *three states carry
log₂(3), about 1.58 bits of information*. Four layers fit one board's
16 MB flash at 3.82 MB per layer, and **only the hidden state crosses
the wire: 896 FP32 values, about 3.5 KB per hop.**

Now apply the two axes to that machine:

- **A content-hash ID is O(payload).** Hashing a cell's whole state to
  be findable is a cost that grows with what the cell knows. On a device
  with 512 KB of RAM this is not a detail; it is the difference between
  addressing a cell and not being able to.
- **A place-address is O(1).** `(x, y, t, sig)` is a coordinate you
  compute and hand over. "4D navigation without a map" is exactly a
  device with no registry, no network and no map: you find things by
  **position, time and signature**.
- **And the charter already says the rest:** *"Don't store cells in a DB.
  Use the witness log + derived state"* and *"Witness log = append-only —
  no DB, just files"*. **Cells of DB and standard algorithms** — a cell
  is a few hundred bytes of standard code plus an append-only log.

**So the hash is the part that does not scale down, and the charter's
place-address is the part that does.** The fleet built the hash in a
game, for teaching, where payloads are small and fixed — and it never
reached the model that would have to run on a $3 chip.

## 4. And the charter already anticipates the whole thing

> ```
> The same cell model in N languages IS a stress test.
> Each language is a *medium*, not a ranking.
> ```
> ```
> quilt-c      # C99, low-level, fast
> quilt-subleq # SUBLEQ, 1-instruction
> quilt-rs     # Rust, no_std
> quilt-py     # Python, type hints
> quilt-ts     # TypeScript, browser
> ```
> **Polyformalism = same model, 12 languages** — proves it's truly minimal.

**`no_std` Rust and 1-instruction SUBLEQ are the device targets, and
they are in the charter's own list.** "Each language is a medium, not a
ranking" is precisely *the same cell, run on a microcontroller, run on
vLLM in parallel, run in a browser* — and it is why the model can be
**"run parallel on vllm easily or another platform"**: the medium
changes, the cell does not.

**And the three projections are the relational dimensions:**

> ```
> Cells form a 4D graph:
> - TOP (x, y) — spatial
> - FRONT (signal) — observed
> - SIDE (time) — temporal
> The same graph, three projections.
> ```

Distance, scale and time; directional-face, trajectory and speed —
**one graph, three projections**, and the address is the place in all
four dimensions at once.

## 5. What this does to the session's findings

**FINDING-26 stands in substance and is corrected in mechanism.** The
charter still describes a cell model that does not exist in any one
repo. But the axis is **not "one model with an address vs two without
it"** — it is **two independent properties, each built somewhere, each
missing somewhere else**, and the largest hole is the one nobody
attempted: **the 4D address and the three projections have no
implementation at all.**

**The canon's own mechanism was right all along and I misdescribed it.**
"Read the hole your cargo names; leave your address in it" is `qthe`'s
**slot + resonance** — a *place* in a table, resolved by what the cell
carries. The canon's recognition is **place-addressed**, and the game's
hash is **content-addressed**. Those are different mechanisms, and the
canon is on the place side.

**And the founder's σ was never a null.** σ = log₂(3) is the *bit
budget of a ternary weight* — the arithmetic that lets a model live on
a microcontroller. The canon's "honest null" says the constant is not
*magical* about bridge formation; it does not say the constant is
useless. It is the number of bits three states carry, and it is
load-bearing everywhere it is actually used: the game, the substrate,
and the device.

## 6. The rules this correction adds

| # | rule |
|---|---|
| 95 | **Read the formal reference before writing about the formal reference.** `algebra.md` is 5 KB, one commit, and it is the only document in the fleet that states the model. I built two findings and a correction on a second-hand summary of a game doc instead. |
| 96 | **A content hash is an ID, not an address.** "Address = (x, y, t, sig) — 4D navigation without a map" is a place. `computeAddress()` in the game computes a hash and is misnamed; the misnomer is what misled me. |
| 97 | **A hash is O(payload) and a place is O(1).** That difference decides which addressing survives on a 512 KB device, and it is the whole reason the charter says "4D navigation without a map." |
| 98 | **When a constant is declared dead, read the null before believing it.** σ = log₂(3) is the bit budget of a ternary weight — the arithmetic that puts a 0.5B model on seven $3 boards. "Not magical about bridge formation" is not "not load-bearing." |
