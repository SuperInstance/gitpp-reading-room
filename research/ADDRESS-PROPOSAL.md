# PROPOSAL: kill the four-tuple, ship the pair

**Status:** design proposal. Nothing was pushed, modified, or created on GitHub. Read-only throughout.
**Author:** Snowball (Scout). **Date:** 2026-10-18.
**Subject:** `SuperInstance/AI-Writings/algebra.md` — clause 3 of "Engineering Cleverness" (`Address = (x, y, t, sig)`), and its conflict with "The Address Is the Data".

---

## 0. What I verified before arguing

`algebra.md` was read through the contents API with `?ref=main` (default branch read from the API first: it is `main`).

| check | result |
|---|---|
| size | 5,987 B — matches the brief |
| blob sha | `a95433d05e826c44568fddd6335f2567cfe8bca8` |
| md5 of API bytes | `9b3b2db2d9d72306e65454bdb85c42a2` |
| commits touching it | **2** — `3807557abd` (2026-09-21T14:46:45Z, created), `1da6180bb7` (2026-10-18T07:46:49Z) |

Every cell-model citation below was re-fetched from the API and read at the stated line. Citations taken from the intercontext map **without** re-verifying are marked `[map]`.

**One process finding, because it invalidates a premise:** the local `/tmp/intercontext.json` in my working directory is **stale** — 53,493 B vs **74,253 B** on `main` right now. `MAP.md` and `CLAIMS.md` were byte-identical; only the JSON had moved. Anyone reasoning from a cached copy of that file is reasoning about a different map.

---

## 1. RECOMMENDATION

> **Strike clause 3 and do not implement a four-tuple. Implement a two-field pair, and keep the content hash — but demote it from *address* to *digest*.** The durable identity of a cell becomes `CellId { site: u32, seq: u64 }` — 12 bytes packed, 16 bytes aligned, 5–13 bytes on the wire as a LEB128 varint pair — where `site` is a stable per-vessel integer provisioned once and never reused, and `seq` is a per-site counter incremented by exactly one on every state change and never reused. This makes the address **O(1), computable without reading the payload, and stable under mutation**. Alongside it, every cell carries `Digest = BLAKE3-256(0x01 || len(kind) || kind || len(payload) || payload || parents)` — 32 bytes, domain-separated per RFC 6962 §2.1 — which is **not an address**: it is used for deduplication, integrity, and witness chaining, and it changes whenever the payload changes. This is the pair the fleet's own most-complete implementation already has (`quilt-rust` `Cell { id, value, reads, version }`), it is the pair Yjs ships, and it is the pair OpenXR, Tahoe-LAFS and qthe each arrived at independently. The spatial axes `x, y` are **not** deleted from the substrate; they are demoted from *identity* to *projection* — exactly what qthe already does and exactly what `@quilt/core` already says out loud.

Concretely:

```rust
// no_std, Copy, no allocation, no serde, Ord for free.
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[repr(C)]
pub struct CellId {
    /// Stable per-vessel. Provisioned once (eFuse or config). Never changes. Never reused.
    pub site: u32,
    /// Monotonic counter within `site`. +1 on every state change. Never reused, never decremented.
    pub seq:  u64,
}
```

Wire form (identical to Yjs `src/utils/ID.js:49-52`, two `writeVarUint` calls):

```
varint(site) || varint(seq)
```

**Semantics, stated as laws:**

1. **Uniqueness.** `(site, seq)` is globally unique iff `site` values are unique across the fleet. This is a *provisioning* obligation, not a runtime one — the same obligation Yjs discharges with `random.uint53` and a collision check.
2. **Stability.** `CellId` never changes for the life of the cell. Payload changes, `Digest` changes, `CellId` does not.
3. **Monotonicity.** For fixed `site`, `seq` is strictly increasing. This is `TICK is monotonic` (law 5, `algebra.md:32`) stated at the addressing layer rather than the engine layer.
4. **Total order.** Compare `site`, then `seq`. This is a **causal/Lamport order**, not a spatial order — see §2, the strongest argument against me.
5. **No registry.** The address is derivable from the address. Nothing has to be looked up to know where a cell is; the update log *is* the directory. This preserves the "no separate ID service" half of clause 1 and the "without a map" spirit of clause 3.

**Repair to clause 1.** `algebra.md:150` reads *"Cell ID = hash of (kind, payload, parents) — no separate ID service."* Split it in two. Keep the second half (right, and load-bearing). Reject the first half (it makes a cell un-updatable in place, and it is O(payload) — see §5):

```
1a. Cell Digest = BLAKE3-256(domain_sep(kind) || payload || parents)   -- integrity + dedup
1b. Cell ID     = (site, seq)                                          -- identity; no ID service
```

---

## 2. The strongest argument AGAINST this recommendation

Steelmanned, because the house rule is that a decision without a recorded counter-argument is not a decision.

**The pair has no geometry, and the charter's central promise is geometric.** `algebra.md:60-70` promises a **4D graph** with three projections — `TOP (x, y)`, `FRONT (signal)`, `SIDE (time)` — and one of the eleven adopted opcodes is `ROUTE : (Cell × Path) → Cell // "Find me the way"` (`algebra.md:20`). To answer *that* you must ask **"what is near me?"** `(site, seq)` cannot answer it. There is no distance function, no locality, no "adjacent". Answering a spatial query against a pair-addressed store requires a **full scan** — O(n) over the entire struct store, in RAM, on a device whose entire SRAM is 512 KB. I am proposing an addressing scheme that makes the charter's headline capability (`cl.lattice`, the one node in the intercontext map with *zero* edges) **strictly harder**, and I am doing it by deleting the only fields that could have supported it.

The four-tuple's `x, y` are not decoration. They are the entire reason a four-tuple was ever proposed. My proposal keeps `t` (as `seq`) and throws away the two fields that carry the actual idea.

Three further points the recommendation does not answer:

- **It concedes the fleet's only unclaimed idea.** The survey's phrase is blunt and correct as a description: the four-tuple is unclaimed. Recommending the Yjs pair means the fleet ships a 2015 CRDT identifier under its own name. Originality has value in a charter that says *"an algebra that grows when more opcodes are proved in"* — and my answer to that is `algebra.md:162`, *"Don't invent new abstractions,"* which cuts against me as much as for me.
- **It assumes `site` can be provisioned on a vessel with no network and no filesystem.** See §7 gap 1 — I could not find prior art for this. If a vessel cannot be given a unique `site`, the pair is not globally unique and the scheme degrades to "unique per session", which is Yjs's actual guarantee, not a fleet-wide one.
- **It makes `x, y` a runtime cost rather than deleting them.** I say "demote to projection" — but a projection is *RAM you pay for on every device*, and the device test (§5) is precisely the argument that the fleet is short on RAM. I have moved the problem, not solved it.

**Why I still recommend the pair — honestly stated.** The four-tuple's two spatial fields cannot be specified without answering questions the fleet has never answered: who allocates `x`? What happens to every `LINK` edge when a cell is inserted at `(3,7)` and everything after it shifts? What is the resolution — millimetres? lattice sites? The charter says *"without a map"*, which means no allocator, and **without an allocator there is no `x`.** `@quilt/core` — the only model in the corpus with a published npm version — already answered this in a comment block titled `KEY DESIGN DECISIONS`, decision 1, at `packages/core/src/types.ts:42-45`:

> *"STABLE IDENTITY OVER COORDINATES — A cell is addressed by its `id` (e.g. "compass.heading"), not by A1/B2-style coordinates. **Coordinates are display-only.** This means inserting/reordering cells never breaks references."*

The fleet's most-shipped model has already rejected coordinate addressing, for the correct reason, in prose — and the charter still carries the tuple. A spatial index is a real requirement and deserves real work. But it is a **separate, additive, rebuildable** problem, and conflating it with identity is what produced four incompatible tuples in the first place.

---

## 3. What the fourth field is FOR

**I cannot justify it. That is the finding, and it is why the four-tuple is wrong.**

Four candidate justifications for `sig` as a fourth address field, and why each fails:

| proposed job for `sig` | why it fails |
|---|---|
| Proves authorship of the address | A signature authenticates a *message*. It does not describe a *place*. Putting it in the address conflates identity with authenticity — two problems with two different lifetimes. |
| Disambiguates concurrent cells at the same `(x, y, t)` | `t` already does that if `t` is monotonic per site (law 5). A signature is not a tiebreaker and is non-deterministic, so it cannot participate in an ordering. |
| Binds the address to the payload | Precisely what the content hash does, better. The charter **already has this field twice**: `Cell.hex` (`algebra.md:48`) and `Witness.sig` (`algebra.md:93`). |
| Authenticates the cell without a network | A real problem with the wrong owner. It belongs to the witness chain — which `quilt-rust/crates/quilt-core/src/ledger.rs:402` already implements in sha256 with `expected`/`imbalance` fields (`ledger.rs:421,426`), the only chain in the graph that witnesses a prediction `[map]`. |

**And it is not merely redundant — on constrained hardware it is infeasible.** This is the strongest single piece of evidence in the whole question, and the survey cited the document without finding it.

RFC 9153 §5.4 (`rfc9153.txt:1266-1296`) works through exactly the problem of binding an identifier to its cryptographic authentication data in a small broadcast frame, under a hard 25-byte budget (`:1266` — *"this limits the RID message to 25 bytes"*). Its conclusion, verbatim (`:1286-1289`):

> *"it is infeasible to bundle information related to the UAS ID and corresponding authentication data elements in a single Bluetooth 4.x frame; yet, somehow all these must be securely bound together."*

The IETF's answer was to **page the authentication message out of the address entirely and correlate it out-of-band by MAC address** (`rfc9153.txt:1290-1296`; the paged `Authentication` message type at `:1328`). A fourth signature field in a four-tuple address is the exact construction the IETF rejected, in the tightest regulated broadcast environment ever specified, for the same reason it will fail here.

**Verdict: the four-tuple is wrong.** `x` and `y` are rejected by the fleet's own shipped model; `sig` is redundant with two fields the charter already has and infeasible under the device budget; `t` is the only field with a job, and it is better expressed as `seq`.

---

## 4. The eight cell models, mapped onto the proposal

The intercontext map lists 8 `cell_model` nodes. All eight are addressed. Every line citation in this table was re-fetched and read.

| # | model | repo | current address | verdict | breaks? |
|---|---|---|---|---|---|
| 1 | spreadsheet cell | `SuperInstance/quilt` | chosen string, `packages/core/src/types.ts:43` | **CONFLICTS, must change** — but it is already *on the pair's side* | Nothing. `types.ts:44` already says "Coordinates are display-only": `(sheet,row,col)` becomes a **presentation projection**, which is what it already is. `id` becomes `(site,seq)` encoded as a string, or stays a chosen string with a `(site,seq)` field added alongside. |
| 2 | agent cell | `SuperInstance/quilt-claw` | **none** — `src/types.ts:21` carries `witness_hash` on `ResearchOutput` | **ADOPTS FREE** | Nothing. It has no address to break; it gains one. `witness_hash` maps to `Digest`. |
| 3 | openjev ancestor cell | `SuperInstance/AI-Writings` | random — `cellular-first-design/code/openjev/cell.py:34`: `self.id = id or f"cell-{uuid.uuid4().hex[:8]}"` | **ADOPTS, mechanical** | Trivial. `uuid4[:8]` is a 32-bit random site id; replace with provisioned `site` + counter `seq`. Strictly *better* (collides at 2^32). Its address lives in a different file — `code/spreadsheet/spreadsheet_plugin.py:23`, `parse_address -> (sheet, row, col)` — so that plugin becomes a projection too. |
| 4 | the game's substrate | `SuperInstance/cargo-line-tycoon` | `substrate/ts/src/index.js:65` `this.address = this.computeAddress()`, `:68-74` `fnv1a64(JSON.stringify({s,t,b}))` | **SPLITS IN TWO** | Yes, and it is worth it. `computeAddress()` becomes `computeDigest()`. Add a `CellId` field. This model currently calls a **content hash** an `address` — the sharpest naming defect in the corpus, and the split is exactly its fix. |
| 5 | qthe's lattice cell | `SuperInstance/qthe` | `(slot, x, y, resonance)` — `qthe-core/kernel/qthe.mjs:102`, `:119-126` | **ALREADY CONFORMS** | **Nothing.** Read `qthe.mjs:105-113` carefully: `this.sx`, `this.sy`, `this.sr` are `Int32Array`s **indexed by slot** — `read(slot)` returns `{x: this.sx[slot], y: this.sy[slot], resonance: this.sr[slot]}`. **`slot` is the address; `x, y` are the payload.** qthe has been running this proposal all along, with a 6-bit address, and nobody noticed because it was filed as an adjacent-but-different tuple. The spatial projection is `sx`/`sy` — the "demote to projection" answer, implemented. |
| 6 | receipt cell | `SuperInstance/AI-Writings` (labs) | `labs/quilt-kernel/quilt_kernel.py:259`: `body["activation"] = activation(self.name, body["input_hash"], body["product_hash"])` | **ADOPTS — best precedent in the corpus** | Nothing. This already binds **id + content digests in one value**. `:257` builds the body; `:221` rejects on `content_hash(b['input']) != b['input_hash']`. The fleet wrote the split already. |
| 7 | cell-contract sketch | `SuperInstance/quilt-cells` | none — `QUILT.md:48` `INPUT → OUTPUT + WHY + COST`, docs only | **ADOPTS, doc edit** | No code in the repo. `QUILT.md:145` ("it gets a **phone call**") stays prose; `quilt-subleq/src/block.rs:117` confirms the fetch is still mocked. |
| 8 | demo corpus | `SuperInstance/AI-Writings` | inherits 3 + 1 | **INHERITS** | Inherits models 3 and 1's migration. `code/openjev/test_openjev.py:48` asserts `entry[opcode] == BIND` — unaffected. |

**A ninth model that is not in the map's 8 but matters more than six of them:**

| model | repo | current shape | verdict |
|---|---|---|---|
| the Rust polyformalism cell | `SuperInstance/quilt-rust` | `crates/quilt-polyformalism/src/lib.rs:118-127`: `pub struct Cell { id: &'static str, value: Value, reads: &'static [&'static str], version: u64 }`, and `Engine { tick: u64 }` at `:131-136` | **ALREADY EXACTLY THE PROPOSAL.** `id` → `site`; `version: u64` → `seq`; `reads` are the `parents`; `Engine::tick` is the fleet clock. `lib.rs:37` is `#![cfg_attr(not(test), no_std)]` — the `no_std` ESP32-S3 target, and it already carries the pair. Note `lib.rs:33-34`: *"Hashing: a tiny FNV-1a... Real substrates swap this for the platform's native hash"* — a **content hash living beside the id, as a separate concern**. The split is half-built. |

**Net:** 4 adopt (2, 6, 7, 8), 2 adopt mechanically (3, and 4 with a rename), 1 already conforms (5), 1 conflicts and must change (1), 1 already conforms and is not in the map (quilt-rust). **The proposal is 8-of-8 compatible or improving.** The four-tuple is compatible with **zero**.

---

## 5. The device test: ESP32-S3, 512 KB SRAM, no network

**Hardware confirmed.** Espressif's ESP32-S3 datasheet specifies **384 KB ROM / 512 KB on-chip SRAM / 16 KB SRAM in RTC**, Xtensa dual-core LX7 at up to 240 MHz.

| candidate | bytes/addr (RAM) | bytes on wire | address computable **without reading the cell**? | allocator needed? | lookup |
|---|---|---|---|---|---|
| **`(site: u32, seq: u64)` — proposed** | **12 packed / 16 `#[repr(C)]`** | 5–13 (varint pair) | **YES** | **no** | O(log n) binary search over a 12-byte key, in RAM, zero I/O |
| `(x: i32, y: i32, t: u64)` — 3-tuple | 16 | 16 fixed | partially (`x,y` need an allocator) | **yes** | O(1) per probe |
| `(x, y, t, sig: Ed25519)` — the four-tuple | **80** | 80 | **no** (`sig` needs the witness; `x,y` need the allocator) | **yes** | O(1) per probe |
| `(x, y, t, sig: 8 B)` | 24 | 24 | no | yes | O(1) per probe |
| content hash only (tycoon's FNV-1a-64) | 8 | 16 hex | **NO — O(payload)** | no | n/a |

**Does it survive? Yes, and it is the only candidate that survives on the right axis.**

- **Size.** 16 bytes in RAM, 12 packed. At 16 B/address, 512 KB is 32,768 addresses — but that is the *wrong* question, because no cell fits anyway. The meaningful number is the next column.
- **Lookup is O(1).** `(site, seq)` is `Copy`, `Ord`, `Eq` — no hashing, no serialization, no payload read, no `serde`, no allocation. It compiles unchanged under `#![no_std]` (`lib.rs:37`) and is representable in a SUBLEQ-port target (`quilt-subleq/src/lib.rs`), which a content hash of arbitrary length is not.
- **The decisive property is O(1)-ness of *derivation*, not of lookup.** A content hash is **O(payload)** — to learn a cell's address you must first read and hash the whole cell. On a 512 KB device you therefore cannot address any cell larger than free RAM. The pair has no such floor: the address of a 4 GB cell is still 16 bytes. This is the device argument in one sentence.
- **The four-tuple fails on both size and derivation.** 80 B/address with a full Ed25519 signature puts 6,553 addresses in 512 KB **with zero payload and zero program code**. Worse, `sig` cannot be computed until you have the witness and `x, y` cannot be chosen without an allocator — on a bare-metal S3 the four-tuple is *not constructible*, not merely large.
- **No network is required by any step.** Provisioning `site` is a one-time act (eFuse or config), not a lookup. The `no_std` constraint recorded at `lib.rs:683` — *"No filesystem in `no_std`"* `[map]` — is irrelevant to a 16-byte `Copy` struct, and fatal to the URL-shaped `Port { url, sig, encoding }` at `quilt-subleq/src/block.rs:24`, whose own comment at `:117` admits *"the remote fetch is mocked."*

---

## 6. "The Address Is the Data" — which way to cut

**Both are wrong. Cut clause 3.** But read the section closely first, because it contains a contradiction *within itself* that nobody has named.

`algebra.md:97-108`:

```
## The Address Is the Data

The location of a cell IS its data. Two cells with the same
id and kind ARE the same cell. There is no separate "store".

cell.id = "ack:7"
cell.kind = "ack"
cell.payload = { timeoutMs: 3000 }

The triple `(kind, id, payload)` IS the cell.
```

The heading says "**location**". The body contains **no location** — `cell.id = "ack:7"` is a *chosen name*, the same shape as `@quilt/core`'s `"compass.heading"` (`types.ts:43`). The heading does work its body does not do. Meanwhile clause 3 (`:152`) says the address *is* `(x, y, t, sig)`, a location. The charter asserts two incompatible things about the same word, and the fleet's own cartographer recorded it (`MAP.md`, gap 2).

**My cut, and the reasoning:**

1. **Strike clause 3 (`:152`) entirely.** Do not implement `(x, y, t, sig)`. §3 gives the case: three of four fields have no job.
2. **Rewrite the heading, keep the section.** The defensible reading of "The Address Is the Data" is not *identity = content*. It is **self-certification**: the address travels inside the cell, so there is no directory, no registry, no ID service. That reading is compatible with everything else in the charter and is the one worth keeping — it is the same claim as clause 1's second half, and it is true.
3. **Replace the three sentences with:**

```
## The Address Is the Cell

The address of a cell travels inside the cell. There is no
directory, no registry, no ID service: the address is
derivable from the cell that carries it.

cell.id     = (site, seq)            // durable identity; never changes
cell.digest = BLAKE3-256(...)        // content; changes with the payload

The address is the cell's identity. It is NOT the cell's
content: two cells may share a site and seq only if one is a
fork, and the digest is what catches it.
```

4. **What is lost.** "Two cells with the same id and kind ARE the same cell" was a strong and wrong claim. It makes a cell un-updatable — under it `TICK` cannot advance anything and law 5 (`algebra.md:32`) has no meaning, because a tick that changes the payload creates a *different cell*. **The current charter contains a latent contradiction between line 108 and line 32.** The pair removes it: `seq` advances, `CellId` does not, the cell updates in place, and law 5 becomes checkable at the addressing layer.

The clean statement: **identity is stable, content is not, and confusing them is the defect.** That is also exactly how OpenXR splits `XrSpatialEntityIdEXT` (scoped atom) from `XrUuid` (`XR_EXT_spatial_persistence`, cross-session), how Tahoe splits `CHK` (content-derived) from `SSK` (stable slot), and how `quilt-rust` splits `Cell.id` from `Cell.value`.

---

## 7. Three prior art, and three gaps

### Prior art that bears on this (all verified by me this session)

**1. Yjs — the production four-tuple that is actually a pair.** `yjs/yjs`, branch `main`.
`src/utils/ID.js:5-22` defines `class ID { constructor(client, clock) }` — **two fields**. `src/utils/ID.js:49-52`, `writeID`, serialises it as exactly two varuints. `src/utils/Transaction.js:18` is `export const generateNewClientId = random.uint53`, and `Transaction.js:310-312` regenerates it on collision. *Retrieved 2026-10-18.*

> **The 2026 survey is wrong about this, and it is the survey's load-bearing claim.** It states Yjs addresses items as `(client, random uint32, clock)`, "read from `src/utils/ID.js`". `ID.js` contains **no random field at all** — the random one is the *client*, it is **uint53** not uint32, and the tuple is a **2-tuple**. The survey's "Yjs ships a four-tuple in production" argument does not hold. What holds is *better* for this proposal: **the most widely deployed CRDT identifier on earth is a pair**, and it is the pair this proposal recommends.

**2. RFC 9153 (DRIP), IETF, February 2022** — https://www.rfc-editor.org/rfc/rfc9153.txt
Defines an ICAO 24-bit "address" that is *"arguably really an identifier rather than a locator"* (`:2268`) — the IETF, writing the document that motivated a four-field address, says the address field is an **ID**. It also establishes the **25-byte** budget (`:1266`) and concludes that binding an ID to its signature in one frame is **infeasible** (`:1286-1289`), paging authentication out of band instead (`:1328`).

> **Three corrections to the survey's characterisation.** RFC 9153 is **Informational**, *"not an Internet Standards Track specification"* (`:31`); it is **requirements and terminology, not a wire format** — the wire format is ASTM **F3411-19**, whose `Message Pack` defines up to **seven message types of which only two are mandatory** (`:1315-1337`); and the quoted scope phrase *"network degraded, and network denied"* **does not appear in the document** — grep returns **0 hits for both "degraded" and "denied"**. The *conclusion* survives, strengthened.

**3. Tahoe-LAFS** — https://github.com/tahoe-labs/tahoe-lafs/blob/master/docs/architecture.rst and `docs/specifications/mutable.html`

> *"The capability provides both location and identification: you can use it to retrieve a set of bytes, and then you can use it to validate ('identify') that these potential bytes are indeed the ones that you were looking for."* (`architecture.rst`)

And from `mutable.html`, the passage that settles §6 for us — **Tahoe already faced this exact tension and already resolved it**:

> *"Mutable files are places with a stable identifier that can hold data that changes over time."*
> *"...the mutable file identifier remains fixed for the life of the slot, **regardless of what data is placed inside it**."*

**Also confirmed as the survey stated:** arXiv:2606.16223 *did:crdt* (2026-06-15) — full text confirms *"content-addressed identifier derivation via BLAKE3-256"* and, verbatim, *"Creation needs no network, registry, or synchronisation."* arXiv:2607.01308 *CanonicalMerge* (2026-07-01) — confirmed as *"a set of content-addressed latent fragments merged by set union, a state-based CvRDT"* with a permutation-invariant render; **but its domain is KV-cache merging for multi-agent latent reasoning**, so its bearing on *addressing* is indirect. OpenXR: `XrSpatialEntityIdEXT` is `XR_DEFINE_ATOM` (Khronos `xr.xml`, `XR_EXT_spatial_entity`), with cross-session durability delegated to a separate `XrUuid` (`XR_EXT_spatial_persistence`) — **confirmed**.

### Three things the literature does NOT settle — the gaps this proposal fills

**Gap 1 — durable actor identity with no network and no filesystem.** Every precedent I checked generates actor ids freshly and hopes: Yjs uses `random.uint53` per session (`Transaction.js:18`) and papers over collisions at runtime (`Transaction.js:310-312`); `did:crdt` still requires *"causal delivery"*; Tahoe needs a grid you already know the address of. **I found no prior art for provisioning a stable, never-reused actor id on a microcontroller with no network and no filesystem** — exactly the ESP32-S3 target, and exactly what `site: u32` needs. The ESP32-S3 has a 4 Kbit eFuse with ~1,652 bits reserved for user data per Espressif, an obvious candidate; **I found no work using eFuse as a CRDT actor id.** This is the largest unclaimed piece of my proposal, and it is a *deployment* problem wearing a design costume.

**Gap 2 — privacy leakage in a `(site, seq)` id space, at fleet scale.** The pair leaks **who** and **when**, structurally and irreversibly: `seq` is monotonic, so any observer holding two addresses learns the author's write rate and ordering. For a substrate whose stated actors include "humans, AIs, sensors" (`algebra.md:178`), that is not a small cost. **None of Yjs, OpenXR, Tahoe, or did:crdt addresses it** — all four assume a cooperative or semi-trusted peer set. I found no work on privacy-preserving actor ids for CRDTs at microcontroller scale. If the fleet wants a substrate that can observe humans, this needs an answer (opaque ids + a resolution table is the obvious one) and my proposal does not supply it.

**Gap 3 — "a place" in a substrate with no map, no allocator, and no network.** The charter wants *4D navigation without a map* (`algebra.md:152`). But NDN requires *"address assignment and management"* to stop; Tahoe needs to know which grid; OpenXR needs a runtime owning an `XrSpatialContextEXT`; a spreadsheet address needs a sheet that exists. **Every system that has a place has an allocator, a registry, or both.** I could find **no** prior art for spatial addressing without a registration step. **This is the genuinely unclaimed gap the survey sensed — but it is a gap in the *spatial index*, not in the *address*.** The fleet has been trying to solve an indexing problem with an identity mechanism. Those are different problems, and §2's counter-argument is the honest statement of what is lost when you separate them.

---

## 8. What I could NOT verify

Recorded because an honest gap is worth more than a confident guess.

- **`quilt-claw/_worker.js`** — the map cites `_worker.js:181,447` for the "live canon" witness dialect. The path `SuperInstance/quilt-claw/_worker.js` **404s** on `main`. I did not locate the file, so I make no claim about the live canon's witness chain. (Its relevance here is nil — it has no address.)
- **`quilt-substrate-meta`** — the map marks it UNVERIFIED and I did not clone it. `algebra.md:26`'s claim that the 5 laws are proved there is **still unverified**.
- **The survey's line citations are off.** Both `MAP.md` and `CLAIMS.md` cite `algebra.md:129` for the four-tuple and `algebra.md:128` for the hash-ID clause. **The verified line numbers are 152 and 150.** Line 129 is `sclerotic → frozen`, inside the 6-Tier Substrate table. The offset (+23) matches the second commit's insertion, so the citations appear to have been measured against `3807557abd` and never re-measured after `1da6180bb7` on 2026-10-18T07:46:49Z. **A citation off by 23 lines is a citation nobody opened.** The same defect appears in the map's `lib.rs:86` for `COUNT = 9`, which is at **line 85**.
- **`quilt-c`, `quilt-py`, `quilt-ts`** — named at `algebra.md:143`, not cloned, not measured. The "12 languages" claim remains unenumerated.
- **NDN** — I did not independently verify the *"address assignment and management"* quotation. It is used only as a directional claim in Gap 3, not as a citation.

---

## Appendix — every file:line this proposal rests on

All re-fetched from `api.github.com/repos/.../contents/...?ref=main` on 2026-10-18.

| citation | content |
|---|---|
| `SuperInstance/AI-Writings/algebra.md:97` | `## The Address Is the Data` |
| `…/algebra.md:103` | `cell.id = "ack:7"` — a chosen name, not a location |
| `…/algebra.md:108` | `The triple (kind, id, payload) IS the cell.` |
| `…/algebra.md:32` | Law 5, `TICK is monotonic` — contradicted by `:108` |
| `…/algebra.md:48` / `:93` | `hex: string` / `Witness.sig` — the fields `sig` would duplicate |
| `…/algebra.md:152` | **clause 3, the four-tuple** (not 129, as the map cites) |
| `…/algebra.md:150` | clause 1, `Cell ID = hash of (kind, payload, parents)` (not 128) |
| `SuperInstance/quilt` `packages/core/src/types.ts:42-45` | `STABLE IDENTITY OVER COORDINATES` … `Coordinates are display-only` |
| `SuperInstance/quilt-claw` `src/types.ts:21` | `witness_hash: string;` on `ResearchOutput` |
| `SuperInstance/AI-Writings` `cellular-first-design/code/openjev/cell.py:34` | `self.id = id or f"cell-{uuid.uuid4().hex[:8]}"` |
| `…/cellular-first-design/code/spreadsheet/spreadsheet_plugin.py:23` | `parse_address(...) -> (sheet, row, col)` |
| `SuperInstance/cargo-line-tycoon` `substrate/ts/src/index.js:65,68-74` | `this.address = this.computeAddress()`; `fnv1a64({s,t,b})` |
| `SuperInstance/qthe` `qthe-core/kernel/qthe.mjs:102,105-113,119-126` | 64 slots; `sx`/`sy`/`sr` are `Int32Array` **indexed by slot** |
| `SuperInstance/AI-Writings` `labs/quilt-kernel/quilt_kernel.py:257,259` | `activation(name, input_hash, product_hash)` |
| `SuperInstance/quilt-subleq` `src/block.rs:19,22-24,117` | `Port { url, sig, encoding }`; *"the remote fetch is mocked"* |
| `SuperInstance/quilt-rust` `crates/quilt-polyformalism/src/lib.rs:37,85,118-127,131-136` | `no_std`; `COUNT = 9` (not 86); `Cell{id,value,reads,version}`; `Engine{tick}` |
| `yjs/yjs` `src/utils/ID.js:5-22,49-52` | `class ID(client, clock)` — **a pair** |
| `yjs/yjs` `src/utils/Transaction.js:18,310-312` | `generateNewClientId = random.uint53` |
| RFC 9153 `:31, :1266, :1286-1289, :1315-1337, :2268` | Informational; 25 B; signature "infeasible" to bundle; 7 message types; *"identifier rather than a locator"* |
| arXiv:2606.16223 (2026-06-15) | did:crdt — BLAKE3-256; *"no network, registry, or synchronisation"* |
| arXiv:2607.01308 (2026-07-01) | CanonicalMerge — content-addressed CvRDT over KV-cache fragments |
| Espressif ESP32-S3 datasheet | 384 KB ROM / **512 KB SRAM** / 16 KB RTC SRAM |
