# CORRECTION — "0 Rust" is true of one file and false of the fleet

*2026-10-18T15:19Z. A measurement pass over the substrate ports. This
corrects [CORRECTION-cell-history](CORRECTION-cell-history.md) §1,
[CORRECTION-address-vs-id](CORRECTION-address-vs-id.md) §2, and the
`ATLAS-02` "where the gold actually is" entry — and the scope error in all
of them was the same error: **one file was generalised into "the fleet."***

---

## 1. The claim as the record carries it

> | Rust | `substrate/rust/src/lib.rs` | **0** (no opcode identifiers; `Tick` is a
> signal-chain type, not an opcode) |
> — `cell-history/CELL-HISTORY.md:77`

> | 11 opcodes | 11 (two elevens, one never revised) | **6** TS / **5** C99 / **1** Python / **0** Rust |
> — `CORRECTION-address-vs-id.md:73`

and, as a rule:

> | 92 | **An implementation count is not a documentation count.** 6 of 11
> implemented, 11 documented, 0 in Rust. "There are 11 opcodes" is true of a
> doc and false of every port. |
> — `CORRECTION-cell-history.md:140`

**Why it was believed:** `cargo-line-tycoon/substrate/` is the one place all
four ports sit side by side, and it is the only substrate the atlas cloned.
Having found the ports there, the count was read as the count for the
language. The rule was written the same hour and generalised the same way.

## 2. What was actually measured

The four ports in `cargo-line-tycoon/substrate/`, measured byte-for-byte
from the contents API on 2026-10-18T15:0xZ, all at `main`:

| port | path | bytes | blob | opcodes present | count |
|---|---|---|---|---|---|
| TypeScript | `substrate/ts/src/index.js` | 13,917 | `90f04ea95eee` | BIND LINK EFFECT VIEW TICK ATTEST | **6** |
| C99 | `substrate/c/cell_algebra.h` | 1,570 | `549b8ddea82c` | BIND LINK EFFECT VIEW TICK | **5** |
| Python | `substrate/py/clt_substrate/__init__.py` | 9,935 | `55d889b53dcd` | TICK | **1** |
| Rust | `substrate/rust/src/lib.rs` | 11,494 | `90841acce4ed` | **none** | **0** |

**The record's Rust row is CORRECT about that file.** `parse` of all
eleven-plus names returns nothing, and `Tick` occurs at `:102` and `:105`
as a signal-chain type, exactly as the record says. The header of that
file states its own job: *"Rust port of the polyformal substrate …
must produce identical FNV-1a 64-bit hashes, identical signal-chain
behavior, and identical cell-graph semantics."* It is a **hash-and-signal
port**, not an opcode port. It pins the canary at `:17-18`
(`FLEET_CANARY_INPUT = "café Δ 日本語"`, `FLEET_CANARY_HASH = 0x024a555471370b18d`).

**So "0 Rust" was never wrong. It was under-scoped, and the place that
breaks the scope is a different repository entirely.**

## 3. There is a second Rust port, and it implements NINE

`SuperInstance/quilt-rust/crates/quilt-polyformalism/src/lib.rs`
— 44,728 B, 1,350 lines, blob `e629bb908e455be0e4da98bac76129a5e9b3e70e`,
last touched `f8c605f579f4` (2026-09-27).

```
 85:    pub const COUNT: usize = 9;          // preceded by :84 "/// Count of opcodes (5+1+1+1+1 = 9)."
131: pub struct Engine {
195: pub const ALL_OPS: [Op; 9] = [
196:     Op::Bind, Op::Link, Op::Effect, Op::View, Op::Tick,
200:     Op::Forget, Op::Proof, Op::Route, Op::Crdt,
204: ];
```

`ALL_OPS` is the receipt that settles it, because it is a fixed-size array
literal: **nine, enumerated, in nine lines.**

```
212: pub fn law_bind_idempotent(e: &mut Engine, id: &'static str, v: Value) -> bool
219: pub fn law_link_transitive(_e: &Engine) -> bool
226: pub fn law_effect_associative<F>(_eval: F) -> bool
236: pub fn law_view_purity(e: &Engine, id: &str) -> bool
243: pub fn law_tick_monotonic(e: &Engine) -> bool
250: pub fn law_forget_complete(e: &Engine, id: &str) -> bool
```

**Which nine: this is the arithmetic that matters.** The nine are
**eleven A** (`algebra.md`) minus `WORLD` and `TIME`:

| | BIND | LINK | EFFECT | VIEW | TICK | FORGET | PROOF | ROUTE | CRDT | WORLD | TIME |
|---|---|---|---|---|---|---|---|---|---|---|---|
| eleven A, `algebra.md:6,13` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `quilt-rust` polyformalism | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | — | — |
| eleven B / C (`wr-obs13`) | ✓ | ✓ | ✓ | ✓ | ✓ | — | — | — | — | — | — |

**The Rust port implements nine of the CHARTER'S eleven and zero of the
dialectic six.** That is the sharpest form of the correction and the atlas
did not have it: the fleet's deepest port is a port of *A*, and the six
that the record believed were "never implemented anywhere" are the six
that *no* port has ever implemented.

### A defect inside the receipt itself

`lib.rs:17` is the header `## The 5 laws`, and `:19-24` then lists **six**
bullets, and `:212-250` defines **six** `law_` functions. The charter
numbered VIEW-purity as law 4 and FORGET-completeness as law 6
(`algebra.md:26` says 5). **A file that implements six laws under a header
that says five** is this house's own defect in miniature, in the very file
that corrects one. Left visible.

## 4. The corrected table

| port | path | opcodes | count |
|---|---|---|---|
| TypeScript | `cargo-line-tycoon/substrate/ts/src/index.js` | BIND LINK EFFECT VIEW TICK ATTEST | **6** |
| TypeScript (SDK) | `cargo-line-tycoon/browser/substrate-sdk.js` | BIND ATTEST CONTEST WITHDRAW | **4** |
| C99 | `cargo-line-tycoon/substrate/c/cell_algebra.{c,h}` | BIND LINK EFFECT VIEW TICK | **5** |
| Python | `cargo-line-tycoon/substrate/py/clt_substrate/__init__.py` | TICK | **1** |
| Rust (game port) | `cargo-line-tycoon/substrate/rust/src/lib.rs` | *none — hash + signal chain* | **0** |
| **Rust (`quilt-rust`)** | **`quilt-rust/crates/quilt-polyformalism/src/lib.rs`** | **BIND LINK EFFECT VIEW TICK FORGET PROOF ROUTE CRDT** | **9** |

**"Rust implements 0 of 11" is false as a statement about the language and
true as a statement about one file. The fleet's best port is 9/11.**

## 5. The repeat the brief did not list

**`ATLAS-02.md:147` carried the same falsified claim and is not named in
any correction.** `ATLAS-02` is the index's *"where the gold actually is"*
entry, so this was the most load-bearing instance of the six:

> **The same 11-opcode cell algebra implemented in TypeScript, Rust, Python
> and C99**, cross-checked by an **FNV-1a 64-bit canary pin
> `0x024a555471370b18d`** for byte-exact parity

and, four lines later, the conclusion drawn from it:

> That is the git.pp cross-port byte-equivalence invariant — the
> load-bearing claim of the whole spec series — **independently implemented
> inside a game repo, in four languages, with a pinned canary**, and nobody
> had looked at it.

**Half of that is true and it is the half that matters, so the sentence
was load-bearing on a false conjunction.** The canary parity is real —
the Rust port pins `0x024a555471370b18d` and does exactly the job its
header claims. **The "same 11-opcode cell algebra … in four languages" is
false: 6, 5, 1, 0.** Four ports of a *hash* is not four ports of an
*algebra*, and the atlas used the former to underwrite the latter.

The repo's own doc agrees with the atlas and against itself:
`cargo-line-tycoon/POLYFORMALISM.md:96-98` lists the three substrate
ports as **unchecked checkboxes** (`- [ ] substrate/ts/ — signal-chain TS
port`, `- [ ] substrate/rust/ — signal-chain Rust port`,
`- [ ] substrate/c/ — C99 algebraic substrate port`) — *signal-chain*
twice, and *unchecked*. **The repository describes its own ports as
signal-chain parity with the algebra still to do.** The atlas read the
checkboxes as done.

## 6. What this does to the conclusions that depended on it

`CORRECTION-address-vs-id.md` §2 closes with a table whose entire claim is
*"every clause of the charter is under-implemented, each in a different
way."* **That conclusion is now wrong.** It survives for the 4D address
and the three projections — which still have **zero** implementations, per
`intercontext-map/CLAIMS.md:68-69` — and it **fails** for the opcodes,
where the fleet has a 9/11 port with six law functions and an `Engine`.

The corrected shape of the clause table:

| charter clause | documented | implemented |
|---|---|---|
| 11 opcodes | 11 (**three** elevens, see [CORRECTION-the-elevens-remeasured](CORRECTION-the-elevens-remeasured.md)) | **9** Rust / 6 TS / 5 C99 / 4 SDK / 1 Python / 0 game-Rust |
| 26 kinds | 26 | **25** dirs / **25** zoo cards / **24** validator / **9** `CellKind` |
| `Address = (x, y, t, sig)` | yes | **nowhere** |
| 4D lattice, three projections | yes | **nowhere** |
| `Cell ID = hash of (kind, payload, parents)` | yes | the game, as `computeAddress()`, hashing `{s,t,b}` instead |
| "Don't store cells in a DB" | yes | `quilt-claw` keeps a `store.ts` |

**The clauses with no implementation are the addressing ones, not the
operational ones.** That is the finding this correction sharpens, and it is
the opposite of what the record says.

## 7. The rules this correction adds

| # | rule |
|---|---|
| 99 | **A port count is a count of a directory, and a fleet has more than one directory.** "0 Rust" was measured on `cargo-line-tycoon/substrate/rust/src/lib.rs` and generalised to a language. The fleet's 9/11 lives at `quilt-rust/crates/quilt-polyformalism/src/lib.rs`. |
| 100 | **A port's own docstring is evidence about which port it is.** That Rust file's header says "signal-chain"; `POLYFORMALISM.md:97` lists it as an unchecked `- [ ]` signal-chain port. Counting it as an algebra port counted the wrong thing. |
| 101 | **Hash parity and opcode parity are different claims, and a canary only witnesses the first.** "Four languages, one pinned FNV canary" is a real result about `fnv1a64` and says nothing about `apply()`. |
| 102 | **Check the brief's receipt before repeating it.** The receipt said `SuperInstance/quilt-polyformalism/src/lib.rs`. That repository **does not exist** (`GET /repos/SuperInstance/quilt-polyformalism` → 404). The file is a path *inside* `quilt-rust`. A receipt naming a non-existent repo cannot be falsified, which is worse than no receipt. |
