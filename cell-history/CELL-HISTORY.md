# CELL-HISTORY — a trace of the cell models in `SuperInstance`

> **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-eight-cell-models](CORRECTION-eight-cell-models.md)]**
> The title said *"three (and four) cell models."* **There are eight.**
> `SuperInstance/intercontext-map` measured the corpus the same day and found
> `node_kinds: {"cell_model": 8}` over `intercontext.json` (blob `6622a8fe1f16`,
> 48 nodes / 70 edges, all eight `verified: true`, each with per-node `evidence`).
> **Three of the five this title was missing were already in this document under
> other headings** — `openjev` (rows 9, 10), `qthe`, and the `labs` receipt cell.
> The title is kept above in the correction marker rather than rewritten, so the
> count that was believed stays visible.

*Scout report. Read-only. Everything below is measured from git objects and the GitHub REST/GraphQL API.
`/tmp` only. Nothing was written to `/workspace`, nothing was pushed, published or deleted.*

**Date of measurement:** 2026-10-18 (fleet clock). **Account:** `SuperInstance` (Casey Digennaro), 5,289 public repos.

---

## 0. TL;DR

The current shape is three repos. The origin is **not in those repos** — it is in
`SuperInstance/AI-Writings`, a 3.7 GB corpus that holds the charter (`AGENTS.md`), the formal
reference (`algebra.md`), the working validator (`_worker.js`), the growth log (`lab/brew/`), and
the first working `Cell` class of all (`cellular-first-design/code/openjev/cell.py`).

The charter's three claims each point at a *different* model, and two of the three are off:

| charter claim | actually describes | measured status |
|---|---|---|
| "11 opcodes" | **none of the three**, as code. It is an AI-Writings canon claim (`algebra.md`). | 6 of 11 executable in the game substrate; 5 in the C port; 1 named in py; 0 in rust. |
| "26 kinds" | **the agent cell** (`quilt-claw`) — by way of a miscount. | 25 modules on disk, 25 cards in the zoo, **24** in the live validator. |
| "the address is the data" | **the game's substrate** (`cargo-line-tycoon`), descending from `algebra.md`. | real and byte-exact: `96de3eeaabd90b9b`. |

---

## 1. TIMELINE

All dates UTC. "First commit" = oldest commit reachable from any branch/ref in the repo.

| # | Date (UTC) | Model | Repo | What it is | Evidence (commit / path) |
|---|---|---|---|---|---|
| 1 | **2026-08-17T07:34:37Z** | **3. spreadsheet cell** | `SuperInstance/quilt` | `packages/core` = `@quilt/core` 0.3.0. yaml+zod. `CellKind` = 8 literals. **Oldest of the three.** | `e92bd41` "feat: initial release of Quilt v0.1.0"; `packages/core/src/types.ts:150-158` → `value\|formula\|api\|program\|sensor\|listener\|router\|io`; `types.ts:43` *"A cell is addressed by its `id` (e.g. "compass.heading"), not"* [by coordinates] |
| 2 | 2026-08-19T16:27:54Z | 3. spreadsheet cell | `quilt` | 9th kind `ai` lands. The kinds go 8 → 9; the prose says "eight" and never updates. | `5bb39cf` "Integrate AI cells into @quilt/core" (`git log -S "  \| 'ai'"`) |
| 3 | **2026-09-17T10:26:24Z** | **2. agent cell** | `SuperInstance/quilt-claw` | `input → bus → cells → store`. **No `class Cell`. No address. `witness_hash` is a sha256 string.** | `25c8772` "quilt-claw: Quilt-native knowledge crew"; `src/types.ts`, `src/quilt-claw.ts`; `git grep "class Cell" HEAD` → **0 hits** |
| 4 | 2026-09-17T19:31:32Z | 2 → 3 (weld #1) | `quilt-claw` | 4 crew cells promoted to `@quilt/ai` canonical kinds. | `2b6d61e`; `src/cells-as-ai-kinds.ts` → `ai.researcher`, `ai.teacher`, `ai.critic`, `ai.distiller` |
| 5 | 2026-09-19T00:28:02Z | 2. agent cell | `quilt-claw` | **25th** cell module (`cell.lore`). At this commit `src/cells-*/` = **25 dirs**, plus `src/cells/` = 4 crew files. | `0c924f0`; `git ls-tree -d HEAD src/ \| grep '^src/cells-'` = 25 |
| 6 | **2026-09-19T01:43:41Z** | 2. agent cell | `SuperInstance/quilt-claw-cells-game` | "Cell Zoo — **26** cell kinds as clickable cards". **The array has 25 entries.** | `5ef7416`; `index.html:129` *"26 kinds of computation"*; `index.html:157` `const CELLS = [` → parsed **25** ids; ops used: BIND(10) EFFECT(17) TICK(8) — **the first appearance of opcode names on a kind card** |
| 7 | **2026-09-19T21:49:03Z** | **2 + canon, ONE FILE** | `SuperInstance/AI-Writings/_worker.js` | **The weld.** `KIND_WHITELIST` (24 agent-cell kinds) and `OP_WHITELIST` (11 canon opcodes) as sibling constants; `validateCell()` enforces the canon 14-tuple with a `hex` field. | `b0852936`; `_worker.js:21-22`; `_worker.js:41-54` `validateCell`; example payload `{ id:'ack:0', kind:'ack', ticker:0, hex:'0xa1', tag:'test' }` |
| 8 | **2026-09-20T03:50:50Z** (in the filename; committed 2026-09-21T15:36:26Z) | **canon "11 opcodes" is grown here** | `AI-Writings/lab/brew/` | 113 dated brew notes, 2026-09-19T23:12 → 2026-09-21. This is where the opcode set is *grown*, not designed. | `lab/brew/20260920-035050-when-the-substrate-has-11-opcodes--bind-link-effec.md`; commits `6d5f729a`, `995b4842` |
| 9 | 2026-09-21T00:08:03Z | **5. the ancestor `Cell`** | `AI-Writings/cellular-first-design/00_MASTER_VISION.md` | 7-layer thesis. Cell = **State + Type + Hooks + Drops + JEV confidence**. Spreadsheet-addressed as `A1`/`B2`. No 14-tuple, no opcodes, no kinds, no hash. | `34cc1534` "cellular-first-design + agent-harness + ideation-genres: real work begins" (only 2 blobs in the tree) |
| 10 | **2026-09-21T00:14:35Z** | **5. the ancestor `Cell`** | `AI-Writings/cellular-first-design/code/openjev/cell.py` | The first real `Cell` class. **Identity is random: `self.id = id or f"cell-{uuid.uuid4().hex[:8]}"`.** Fields: `id, state, schema, witness_log, proof_chain, scars, hooks, drops, jev_confidence, bound_cells, created_at`. Opcodes observed as `opcode:` strings `WITNESS`, `PROOF`. | `afe2ed2f`; expanded at `41a3c772` (00:23:53Z) |
| 10b | 2026-09-21T00:23:53Z | 5. ancestor | `.../code/spreadsheet/spreadsheet_plugin.py` | The cell↔spreadsheet bridge: `address` = `Sheet1!C3` regex, `parse_address`. **The address is a coordinate, not a hash.** | `41a3c772` — **re-verified 2026-10-18T15:19Z:** `parse_address` is defined at `spreadsheet_plugin.py:23` (blob `139059f5ca5c`), called at `:48`, and `openjev/cell.py` (blob `dbdda303d456`) has **zero** occurrences of it. This is the only mention of `parse_address` in the room and it was already right — see [CORRECTION-phantom-ladder](CORRECTION-phantom-ladder.md) §4 |
| 11 | 2026-09-21T11:10:50Z | canon, playable | `AI-Writings/cellular-first-design/algebra/index.html` | "Substrate Algebra — 11-opcode playground". `OPCODES` array = BIND LINK EFFECT VIEW TICK **FORGET PROOF ROUTE CRDT WORLD TIME**. Cells hashed `'0x'+fnv1a64(...).padStart(8,'0')`; state hash `padStart(16,'0')`. | `b194bef8` "feat: substrate-algebra demo — 11-opcode playground" |
| 12 | **2026-09-21T14:46:45Z** | **THE CHARTER** | `AI-Writings` root | **`algebra.md`, `ENGINEERING.md`, `AGENTS.md` land together.** This is the source of "A cell is a 14-tuple. There are 11 opcodes. There are 26 kinds." and "The address is the data." | `3807557a` "WIP: cellular-first-design + selected canon content". ~~`algebra.md` has **exactly one commit, ever**~~ **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-algebra-second-commit](CORRECTION-algebra-second-commit.md)]: it now has 2.** The second is `1da6180bb77a`, 2026-10-18T07:46:49Z, *"Make the witness carry the answer, and witness what the tick adopts"*, adding `result?: any` to the `Witness` type (+13/−0 in `algebra.md`, +48/−0 in `_worker.js`). **This row was written before 07:46:49Z the same day and was true when written.** `algebra.md` still lists eleven A — the opcode drift finding survives |
| 13 | **2026-09-21T22:01:18Z** | canon mutates | `AI-Writings/cellular-first-design/reports/wr-obs13-missing-opcodes.md` | **The six are replaced, ~7h after the charter.** Names `ATTEST`, `CONTEST`, `REVOKE`, `WITHDRAW`, and (as "a fifth shadow") `MERGE` + `DELEGATE`. Ends: *"Run the numbers: eleven plus four is fifteen."* | `46766b56` "feat: Observation-Primitive Reverse Engineering Rounds 1-3 (15 pieces)" |
| 14 | **2026-09-22T03:12:44Z** | **1. game's substrate** | `SuperInstance/cargo-line-tycoon` | Repo's oldest commit — a **docs-only** kimi1 branch tip. `POLYFORMALISM.md` already pins the FNV canary `0x024a555471370b18d` over `"café Δ 日本語"`. **No `substrate/` code yet.** | `7782f0e` on `rescue/kimi1-recommendations-20260922`; 609 files, 0 under `substrate/` |
| 15 | **2026-09-27T08:14:21Z** | **1. game's substrate** | `cargo-line-tycoon` | **The substrate lands.** `class Cell { state, witness_log, behavior, type }`; `computeAddress() = fnv1a64(JSON.stringify({s,t,b})).toString(16).padStart(16,'0')`; `apply()` implements **6** opcodes. | `011736b` "feat: Phase 0 world-model + Phase 1 playable tycoon toy" — an **orphan root commit** (0 parents); 832 files, 515 under `substrate/` |
| 15b | 2026-09-27 (same snapshot) | 1. game's substrate | `cargo-line-tycoon` | `docs/DIALECTIC_IN_11_OPCODES.md` (7,873 B) restates the 11 as "5 base (proven in C99 algebra)" + "6 proposed (R11 derivative)": ATTEST CONTEST **MERGER** REVOKE DELEGATE WITHDRAW. | same commit; note `MERGER`, not `MERGE` |
| 16 | 2026-09-27T07:20:04Z | arch doc | `AI-Writings/situations/arch/CARGO-LINE-TYCOON.md` | Opus architecture. `:77` *"`Cell` (`state`, `witness_log`, `behavior`, content-addressed `address`)"*. `:78` lists `BIND/LINK/EFFECT/VIEW/TICK/ATTEST`. | `8c35d77c`, revised `da164f8c` (08:42:33Z) |
| 17 | **2026-09-27T05:51:44Z** | **4. qthe's lattice cell** | `SuperInstance/qthe` | Ternary CA kernel. 8-bit primitive `[tau:2][d:6]`, Moore-8 toroidal tick, 64-slot WormholeTable, "pixel = cell". **No address, no opcodes, no kinds, no FNV.** | `be7849f` "qthe genesis: SPEC"; `SPEC.md:52-66`; `.quilt/links.yml` |
| 18 | 2026-09-23T00:51:20Z | polyformalism | `SuperInstance/quilt-canary` | "FNV-1a 64-bit canary — the minimum polyformalism artifact in 5 languages". 2 commits. | `85446048`; `CANARY.md`, `ALGORITHM.md` |
| 19 | 2026-09-29T03:12:36Z | idiom | `AI-Writings/labs/situation-recorder/recorder.py` | fnv1a-64 + canonical JSON + `GENESIS = 0x0000000000000000` chain. **This is the code the agent cell's hash algebra was later ported from.** | `a9ed5ea7`; `labs/quilt-kernel/EXTRACTION.md` names it "chain byte-compatible with MicroMoth-quilt" |
| 20 | **2026-10-10T03:47:08Z** | **6. `quilt-cells`** | `SuperInstance/quilt-cells` | "INPUT → OUTPUT + WHY + COST". A spec seed. 2 files, 13,238 B. **No opcodes, no kinds, no address.** | `5b8f477` "Seed: quilt-cells — first planting"; `QUILT.md` |
| 21 | **2026-10-18T06:15:43Z** | **2. agent cell, address arrives — as a ledger** | `quilt-claw` | fnv1a-64 enters quilt-claw for the **first time in its history**, on an unmerged branch, as a *witness ledger chain*, not a cell address. | `18f7bc3` on `fix/test-harness-and-witness`; `witness/ledger.js` (448 L). Commit body: *"ported from SuperInstance/AI-Writings `labs/situation-recorder/recorder.py`"*. **`main` has zero `fnv` bytes.** |
| 22 | 2026-10-18T06:03:30Z | canon audits itself | `AI-Writings/609-the-slot-nothing-is-in.md` | The recognition sentence: *"read the slot your cargo names; leave your address in it"*; *"The witness log is the slot. The receipt is the address."* Also quotes the charter's "182 tests, 26 kinds" and confirms 182. | `6cbe249b`, corrected `0d5f2120` |

### Order verdict
**Oldest → newest:** `quilt` (spreadsheet cell, Aug 17) → `quilt-claw` (agent cell, Sep 17) → `cargo-line-tycoon` (game substrate, Sep 27) → `qthe` (lattice cell, Sep 27, +5 h).
**The charter (Sep 21) was written after 1 and 2 and before 3.** It was therefore written *about* `quilt-claw`, using a cell model that had not yet been implemented anywhere.

---

## 2. VERDICT ON THE CHARTER'S THREE CLAIMS

Source: `SuperInstance/AI-Writings/AGENTS.md`, 6,208 B, one commit `3807557a` (2026-09-21T14:46:45Z).

### 2.1 "There are 11 opcodes" → describes **the AI-Writings canon**, not any of the three repos.

It is a **prose claim**. Measured implementation:

| port | path | opcodes present |
|---|---|---|
| TypeScript | `substrate/ts/src/index.js` `apply()` | **6** — BIND LINK EFFECT VIEW TICK **ATTEST**; `default:` throws `Unknown opcode` |
| C99 | `substrate/c/cell_algebra.{c,h}` | **5** — BIND LINK EFFECT VIEW TICK |
| Python | `substrate/py/clt_substrate/__init__.py` | **1** — `TICK = 'tick'` |
| Rust | `substrate/rust/src/lib.rs` | **0** (no opcode identifiers; `Tick` is a signal-chain type, not an opcode) — **CONFIRMED for this file, 2026-10-18T15:19Z**, blob `90841acce4ed`, 11,494 B: 0 occurrences of all 17 opcode names; `Tick` at `:102,:105`. **But "0 Rust" is not the fleet's number — see [CORRECTION-rust-is-nine](CORRECTION-rust-is-nine.md):** `quilt-rust/crates/quilt-polyformalism/src/lib.rs` implements **9 of 11** (`COUNT = 9` at `:85`, `ALL_OPS: [Op; 9]` at `:195`, `struct Engine` at `:131`, six `law_` fns at `:212-250`) |

`CONTEST`, `MERGER`, `REVOKE`, `DELEGATE`, `WITHDRAW` are **never implemented** — they appear in
`docs/DIALECTIC_IN_11_OPCODES.md` as "6 proposed (R11 derivative)" and nowhere else in code.
**[PARTIALLY CORRECTED 2026-10-18T15:19Z — [CORRECTION-the-elevens-remeasured](CORRECTION-the-elevens-remeasured.md)]**
The claim is **falsified for two of the five**. `CONTEST` and `WITHDRAW` *are* implemented, in
`cargo-line-tycoon/browser/substrate-sdk.js` — **`CONTEST` at `:242`**, **`WITHDRAW` at `:251`**
(alongside `BIND` `:217` and `ATTEST` `:229`), 11,375 B, blob `c94654e3520d`, duplicated identically
at `browser-deploy/` and `substrate-bundle/`. **Why the original search missed it: that file is in
`browser/`, not under `substrate/`, and the search was scoped to `substrate/`.**
**`MERGER`, `REVOKE` and `DELEGATE` survive the claim** — 0 occurrences of any of the three strings
in that file, and no measured port implements them under any spelling.

**Worse: the canon's eleven is not stable.** There are ~~**two different elevens**, seven hours
apart~~ **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-the-elevens-remeasured](CORRECTION-the-elevens-remeasured.md)]: there are THREE.**
A and B are still 7 h 14 m 33 s apart as described. **C is a third list, three weeks later, and it is
in running code:** `cargo-line-tycoon/tests/sim/massive_simulation.js:16`
`const OPCODES = ['ATTEST','CONTEST','MERGER','REVOKE','WITHDRAW','DELEGATE','BIND','LINK','EFFECT','VIEW','TICK']`
(`595f6f32`, 2026-10-03) — **`MERGER`, not B's `MERGE`**, one letter, two vocabularies, and an eleven
that is neither A nor B. **Eleven C is stated a second time in prose at
`cargo-line-tycoon/POLYFORMALISM.md:58`.** No adoption commit exists for any of the three. The two
original elevens, seven hours apart:

* **six A** (`algebra.md`, 2026-09-21T14:46:45Z; `_worker.js:16,22` `CANON` + `OP_WHITELIST`; the playable
  playground `cellular-first-design/algebra/index.html:81`):
  `BIND LINK EFFECT VIEW TICK` + **`FORGET PROOF ROUTE CRDT WORLD TIME`**
* **six B** (`wr-obs13-missing-opcodes.md`, 2026-09-21T22:01:18Z):
  `BIND LINK EFFECT VIEW TICK` + **`ATTEST CONTEST REVOKE WITHDRAW MERGE DELEGATE`**

`algebra.md` was **never revised** (1 commit, ever). So the corpus simultaneously asserts both.
**[CORRECTED 2026-10-18T15:19Z — [CORRECTION-algebra-second-commit](CORRECTION-algebra-second-commit.md)]**
"Never revised" is now **false**: `1da6180bb77a`, 2026-10-18T07:46:49Z, added `result?: any` to the
`Witness` type. **But `algebra.md:76` still reads
`op: 'BIND' | 'LINK' | 'EFFECT' | 'VIEW' | 'TICK' | 'FORGET' | 'PROOF' | 'ROUTE' | 'CRDT' | 'WORLD' | 'TIME'`**
— eleven A, untouched, **verified inside the patch that proves the file moved.** So the opcode-drift
finding survives its own falsification: *the file was revised, and the opcode list was not.*

**Map:** nearest of the three = **1 (game substrate)**, where 6/11 run. `quilt-claw` has the words
BIND/LINK/EFFECT/VIEW/TICK only in comments and test descriptions (116 hits, zero as an implemented
switch). `quilt` has them only in `landing/decide.html:315-321`, as five `kind-chip` labels in a demo
page, added 2026-09-25T04:56:35+08:00 (`3175dcc`) — i.e. `quilt` also treats the five base opcodes as
**cell kinds**, a fourth reading.

### 2.2 "There are 26 kinds" → describes **the agent cell** (`quilt-claw`) — with an off-by-one.

I counted every enumeration that could be pointed at:

| source | count | path |
|---|---|---|
| `quilt-claw` `src/cells-*/` directories at HEAD | **25** | `git ls-tree -d HEAD src/` |
| `quilt-claw-cells-game/index.html` `CELLS` array | **25** (labelled "26") | `index.html:157` |
| live validator `KIND_WHITELIST` | **24** | `AI-Writings/_worker.js:21` |
| `quilt-claw` `src/cells/` (crew: critic, distiller, researcher, teacher) | 4 | separate directory |
| `@quilt/core` `CellKind` | **9** (`value formula api program sensor listener router io ai`) | `packages/core/src/types.ts:150-158` |
| `cargo-line-tycoon` | **0** — no kind vocabulary at all | `substrate/*` |
| `qthe` | **0** | — |

**The zoo array is byte-for-byte the same 25 names as `quilt-claw`'s `src/cells-*`** (ack audit
breeder broadcast chaos chirp crypt death feedback flock hex lore music neighbor partition pathos
perception perm ranking swarm thermal triage twist voice world — set-equal). So the number the charter
carries is **the agent cell's roster, mislabelled by one**. The 24-vs-25 gap is exactly `perm`, which is
in the zoo and in quilt-claw but was dropped from the validator whitelist some time between
2026-09-19T01:43Z and 21:49Z.

**No catalog, registry, doc or constant anywhere declares 26.** `grep "26 known kinds"` account-wide
returns exactly **1** file: `AI-Writings/algebra.md` — the charter's own line, with no enumeration
beneath it. The zoo is the only artifact that claims 26 and *is* the list, and it has 25 entries.

### 2.3 "The address is the data" → describes **the game's substrate** (`cargo-line-tycoon`).

* **True there, and byte-exact.** I ran the shipped TS port (`substrate/ts/test.js`, node v22.19.0):
  ```
  Canary: 0x24a555471370b18d match: true
  Cell address: 96de3eeaabd90b9b
  ```
  `tests/canary/run_all_ports.sh` (1,934 B, "the single polyformalism-truth command") asserts exactly
  that string. `Edge` is likewise content-addressed over `{f, t, e}`.
* **Stated canonically first** in `algebra.md` §"The Address Is the Data" (Sep 21 14:46Z), as
  *"The location of a cell IS its data… The triple `(kind, id, payload)` IS the cell"*, and in
  §Engineering Cleverness *"Cell ID = hash of (kind, payload, parents)"*, with `hex: string // hash (0x...)`
  as field 9 of the tuple. Example in the canon: `cell.id = "ack:7"` — **a structured string id, not a hash**.
* **False for the agent cell.** `quilt-claw` has no address on `main`, in any commit, on any branch
  (see §4).
* **Explicitly false for the spreadsheet cell, by design.** `packages/core/src/types.ts:43`:
  *"A cell is addressed by its `id` (e.g. "compass.heading"), not"* [by coordinates], and §1 above it
  *"STABLE IDENTITY OVER COORDINATES"*. This is a stated design decision, not an omission.
* **The canary** `0x024a555471370b18d` over `"café Δ 日本語"` is shared by the game substrate,
  `quilt-canary`, `AI-Writings/labs/quilt-kernel`, and (per that file) `forge-quilt`. It is the one
  constant that does cross models.

*Footnote on the "14-tuple":* the `type Cell = {…}` block in `algebra.md` declares **15** fields
(`id, kind, payload, parents, links, witnesses, effects, ticker, hex, tag, meta, created_at,
updated_at, author, sig`). `validateCell()` in `_worker.js` enforces **7** of them.

---

## 3. DID THEY EVER MEET

**Yes — three times, all of them in `AI-Writings` or in a dependency edge. Never inside the three repos themselves.**

### Meet #1 — same file: `AI-Writings/_worker.js` (2026-09-19T21:49:03Z, `b0852936`)
```js
const KIND_WHITELIST = ['ack','music','voice','swarm','world','crypt','pathos','ranking','partition',
                        'neighbor','audit','twist','death','broadcast','chirp','chaos','thermal',
                        'feedback','lore','perception','hex','flock','triage','breeder'];   // 24
const OP_WHITELIST   = ['BIND','LINK','EFFECT','VIEW','TICK','FORGET','PROOF','ROUTE','CRDT','WORLD','TIME']; // 11
```
and `validateCell()` (`:41`) requires `id` matching `/^[a-z]+:\d+$/`, `kind ∈ KIND_WHITELIST`,
`ticker: number`, optional `hex: /^0x…/`, `parents[]`, `links[]`.
**This is the exact moment the agent cell's kinds were welded onto the canon's 14-tuple opcodes** —
and it happened **before** the charter that later describes both. Note this predates the charter by
~41 h and postdates `quilt-claw`'s last cell extraction by ~21 h.

### Meet #2 — dependency edge: `quilt-claw` → `quilt` (Sep 17 → Oct 18)
`quilt-claw/package.json` declares `"@quilt/core": "workspace:*"`, `"@quilt/ai": "workspace:*"`,
`"@quilt/evolve": "workspace:*"`. `src/cells-as-ai-kinds.ts` (commit `2b6d61e`) promotes 4 crew roles
into the `@quilt/ai` canonical kind namespace (`ai.researcher|teacher|critic|distiller`).
`src/cells-*/flock/flock-sheet.ts` builds a Quilt *sheet*. So model 2 is downstream of model 3 **by import**.
The edge is unresolvable in the wild — `18f7bc3` measures `npm error code EUNSUPPORTEDPROTOCOL`.

### Meet #3 — same corpus: `AI-Writings`, single commit `4ad65e22` (2026-09-21T15:44:04Z)
One commit lands, together: `algebra.md`-adjacent canon pages, `polyformalism/index.html`
(*"if all opcodes + all **26 kinds** + all 5 laws work in the port, the port is faithful"*)
— and `quilt-claw-cells-game/index.html` (the 25-card zoo), `playtest/index.html` (*"The 26 kinds of
Quilt cells"*), `playtest-harness/`, `philosophy/quilt-claw-and-the-substrate-crew.md`,
`philosophy/promoting-knowledge-cells-to-ai-kinds.md`, `philosophy/extracting-cell-twist-from-physics.md`.
**The charter's "26 kinds" and its "quilt-claw/" pointer are the same sentence about the same zoo.**

### Searches that found NOTHING

| query | scope | result |
|---|---|---|
| `ATTEST CONTEST MERGER` (all three words) | `user:SuperInstance` | **0 files** |
| `ATTEST CONTEST MERGER REVOKE` | `user:SuperInstance` | **0 files** |
| `ATTEST CONTEST` | `user:SuperInstance` | 13 files — 11 in `AI-Writings`, 1 `ability-transfer`, 1 `flux-tensor-midi`, 1 mirrored `Lucineer`/`SuperInstance-papers`. **None in the three repos.** |
| the 6-opcode tail inside `quilt-claw` / `quilt` / `qthe` | git HEAD | `ATTEST CONTEST MERGER REVOKE DELEGATE WITHDRAW` = **0** in all three |
| agent-cell kind names inside `cargo-line-tycoon` | git HEAD | only English coincidences in narrative/apps (`world` 265, `critic` 75, `voice` 69, `teacher` 67, `audit` 33, `hex` 21, `swarm` 6, `pathos` 6, `ack` 2, `death` 2, `music` 2). **No kind vocabulary, no `src/cells-*`, no catalog.** |
| `"quilt-claw"` | code search | 36 files, **all** in `AI-Writings` |
| `"cargo-line-tycoon"` | code search | 18 files, **all** in `AI-Writings` + `SuperInstance/README.md` |
| `computeAddress` | code search | **0 files** — the game substrate's method exists only inside `cargo-line-tycoon` and is not indexed as a code-search hit |

**No issue or PR crosses models.** Issue counts: `quilt-claw` 3 (all Oct 18, all harness/deps),
`cargo-line-tycoon` 3 (all Sep 22–27 game work), `qthe` **0**, `quilt-claw-cells-game` **0**,
`quilt-cells` **0**.

---

## 4. ADDRESS OR KINDS FIRST?

**The kinds came first. By four days.**

* Agent-cell kinds: first `quilt-claw` cell modules land **2026-09-17T10:26Z–2026-09-19T00:28Z**
  (`twist`, `breeder`, `chaos`, `feedback`, `thermal`, `death`, `flock`, `broadcast`, `neighbor`,
  `audit`, `partition`, `ack`, `triage`, `ranking`, `voice`, `perception`, `music`, `swarm`, `pathos`,
  `world`, `crypt`, `lore` — 25). Full roster exists by `0c924f0`.
* First *hash-shaped* identity in the lineage: **2026-09-19T01:43Z** (the zoo's `hex` cards) — the zoo
  labels every card `cell.<id>` and carries opcode button labels, but **no hash is computed anywhere**.
* The canon's address sentence: **2026-09-21T14:46Z** (`algebra.md`).
* The first *executable* content address: **2026-09-27T08:14Z** (`cargo-line-tycoon` `computeAddress`).
* The agent cell's only fnv: **2026-10-18T06:15Z**, 21 days after the game shipped one.

### Was there ever an agent cell with an address? **No — and the one fnv that exists is not one.**

`git log -S` across **all branches** of `quilt-claw` (26 commits, 5 refs):

| pickaxe | hits |
|---|---|
| `-S 'fnv'` | **1** — `18f7bc3` (2026-10-18, branch `fix/test-harness-and-witness`, **unmerged**) |
| `-S 'FNV1a'` | **1** — same |
| `-S 'padStart(16'` | **1** — same |
| `-S 'cbf29ce4'` | **0** |
| `-S '0x811c9dc5'` | **0** |
| `-S 'computeAddress'` | **0** |
| `-S 'address'` | **2** — `27d82a3`, `1699a38` (both Sep 18; `address` as an English word in cell.audit / cell.thermal prose, not an identifier) |
| `-S 'witness_hash'` | **2** — `25c8772` (genesis) and `2b6d61e` |

`main` and `fix/workspace-protocol-to-file` contain **zero bytes matching `fnv`**. There is no
`witness/` directory on `main`. What `18f7bc3` adds is `witness/ledger.js` (448 lines) +
`witness/ledger.jsonl` (29 records) — a **hash-chained receipt ledger over the test suite**, with
`GENESIS = 0x0000000000000000` and the rule
`hash = "0x%016x" % fnv1a64(canon({prev, …}))`. Its own commit body says it was
*"ported from SuperInstance/AI-Writings `labs/situation-recorder/recorder.py`"* (first committed
2026-09-29T03:12Z) and *"verified byte-compatible against that Python implementation"*.

**So the address algebra entered the agent cell from the AI-Writings *witness-log* idiom, 19 days
after the same idiom had already been the game substrate's *cell address*.** The agent cell never
hashed a cell.

The output objects carry `witness_hash: string` — but it is **sha256**, not FNV, and it is computed
over content, not used as an identity:
```ts
const witness_hash = await sha256(`${config.topic}:${summary.length}:${Date.now()}`);
```

---

## 5. IS THERE A FOURTH MODEL?

Yes — and by the time you count carefully, **six**.

### 4.1 `qthe`'s lattice cell (2026-09-27T05:51:44Z, `be7849f`) — **resembles none of the three**
`SPEC.md:52-66`: the 8-bit primitive `[tau:2][d:6]`, `Psi(tau) ∈ {0,+1,-1,i}` (Ground/Attract/Repel/Abstain-as-i),
toroidal **Moore-8** tick, and a 64-slot **WormholeTable** (LWW, content-addressed by resonance).
`cloudflare/assets/demo.html:121` — *"Every cell has exactly 8 [neighbours]"*. `README.md:16` —
*"pixel = cell, direct buffer-to-canvas"*.

* **no address**, **no opcodes**, **no kinds**, **no FNV** (`git grep -i 'fnv1a' HEAD` → 0; "addressing"
  appears only as *"content-addressed, not identity-addressed"* for receipt blobs).
* This is a **spatial** cell (a lattice site with 8 neighbours), not a **capability** cell.
* **But it is the one the canon's recognition story is written from.** `AI-Writings/609-the-slot-nothing-is-in.md:140,170`:
  *"read the slot your cargo names; leave your address in it"* … *"The witness log is the slot.
  The receipt is the address."* Its vocabulary — slot, wormhole, non-local twins, cargo, resonance —
  is `qthe`'s, lifted verbatim. `AI-Writings/situations/arch/QTHE-H3-SPEC.md` first committed
  **2026-09-28T04:16:24Z** (`997f7b43`), the day after `qthe` was born.
* **Consequence for the canon:** the recognition mechanism does **not** need an FNV address. In `qthe`
  the "address" is *a slot number plus a resonance value that another cell can independently compute*.
  The FNV-1a address in `cargo-line-tycoon` is a **different answer to the same question**, arrived at
  six days later, in a game, without reference to `qthe`.

### 4.2 `quilt-cells` (2026-10-10T03:47:08Z, `5b8f477`) — the **INPUT → OUTPUT + WHY + COST** cell
2 files. A deliberately minimal spec seed: *"Cells are seeds, not constraints"*, a "prism" cell,
and a table of 9 model-backed cells. **No opcodes, no kinds, no address, no hash.** Replaces the
14-tuple with an explainability triple.

### 4.3 `AI-Writings/labs/quilt-kernel` (2026-09-30T19:43:13Z) — the **receipt** cell
`Cell(fn, name, ledger, cost=None, route=None, keep=("input",), ignore=(), clock=perf_counter)`.
Its own `EXTRACTION.md` §4 explicitly refuses the canon's claims:
> *"The 14-tuple cell, 11 opcodes, 26 kinds, 4D lattice, the five laws — the kernel needs none of it…
> should not inherit its claims"* … *"The kernel receipts **functions**; it does not model stateful cells."*

### 4.4 The openjev cell (2026-09-21T00:14:35Z) — **the actual ancestor of the game's `Cell`**
`cellular-first-design/code/openjev/cell.py`. Same field names as the game's
(`state`, `witness_log`); adds `proof_chain`, `scars`, `hooks`, `drops`, `jev_confidence`,
`bound_cells`; **identity is a random uuid8, not a hash**; addressing is a spreadsheet `A1`/`Sheet1!C3`
regex (`spreadsheet_plugin.py`). Its `opcode:` values are `WITNESS` and `PROOF` — neither is in any
eleven.
The game keeps 2 fields of 12, drops the scars/hooks/drops/JEV layer entirely, and *adds* `behavior`
and `type`, then bolts an FNV address on. **This is the parent the three repos all grew out of, and
it is not one of them.**

### 4.5 The `cellular-first-design/` demo corpus — a fifth model, changing under itself
39 HTML pages (2026-09-21T11:10Z → 23:xxZ). They disagree with each other on the address width:
`algebra/index.html` hashes cells with **`padStart(8,'0')`** (`0x`+fnv1a64 of the cell id) and the
state hash with `padStart(16,'0')`; `witness-log/index.html` uses **`padStart(16,'0')`** for ledger
links; `signal-chain/index.html:201-209` has a `class Cell` with `this.x, this.y` geometry and
`hashHex = '0x'+fnv1a64(text+x+y).padStart(16,'0').slice(-8)` — **an 8-hex-digit address cut from a
16-digit hash**. Three widths, one afternoon.

### 4.6 The substrate-count family that is neither 26 nor 8 nor 11
`AI-Writings` also carries, in the same canon corpus: **"7 substrates: Address, Scale, Room, Elephant,
Protocol, Form, State"** (`paper_32`, `essay_86/87/88`), **"8 cell primitives + 1 meta-primitive"**
(`essay_87`), **"8 primitives … Seven substrates … Nine dials … one conservation law"** (`essay_98`,
`paper_35`), **"13 substrates, 1 cell"** (`paper_37`), and **"8 abstraction levels"** (`paper_33`).
A parallel count family that the charter never reconciles with 26.

### 4.7 The 5-opcode VM — located, not cloned
`SuperInstance/quilt-foundation` *"The 5-opcode Quilt VM. Foundation layer for cells, plugins, sheets,
MUDs, TTRPGs"* (2026-08-25), `quilt-types` *"the 5 opcodes as type-driven cell-graphs. 16 tests"*
(2026-08-25), `quilt-spreadsheet`, plus byte-exact FNV ports in `quilt-go` / `quilt-zig` / `quilt-j`
(2026-09-07/08) and `QuiltCanary.jl` / `QuiltEgg.jl` (2026-09-30). **Five** opcodes, address-carrying,
predating `cargo-line-tycoon` by two months and `quilt-claw` by three weeks. **Not cloned — this is a
measured gap, see §6.**

---

## 6. WHAT I COULD **NOT** DETERMINE

1. **Why "26".** No catalog, registry, or constant declares 26 anywhere in the account
   (`grep "26 known kinds"` → 1 hit, the charter's own line, with no list under it). Best measured
   account: the zoo's 25 entries, generated from `quilt-claw`'s 25 `src/cells-*` dirs, labelled 26;
   `AGENTS.md` copies 26 onto both the zoo and `quilt-claw` on 2026-09-21T14:46Z. The 24↔25 gap is
   exactly `perm`. **I looked at:** every `src/cells-*` path ever added/deleted in `quilt-claw`
   (`git log --all --diff-filter=AD -- 'src/cells-*'` — 27 adds, **0 deletions**, no 26th ever),
   `algebra.md`, `ENGINEERING.md`, `_worker.js`, `canon-index.json`,
   `cellular-first-design/canon/canon.json`, `playtest/index.html`, `harbor/index.html`,
   `labs/quilt-kernel/EXTRACTION.md`, `polyformalism/index.html`. **No commit message in any of the
   six repos mentions the number 26.**
2. **Whether `cargo-line-tycoon`'s `Cell` was copied from `openjev/cell.py` or written fresh.**
   The overlap is 2 fields of 12 (`state`, `witness_log`) and `CARGO-LINE-TYCOON.md:77` describes it in
   the openjev vocabulary, but `behavior` and `type` have no antecedent and `scars`/`hooks`/`drops`/
   `jev_confidence` were dropped. **I looked at:** the commit body of `011736b` (no provenance
   statement), `POLYFORMALISM.md`, `SUBSTRATE_FOUNDATION.md`, `SESSION_SUMMARY_2026-09-22.md`.
   The repo is **9 orphan root commits (0 parents each)** — there is no incremental history to read a
   diff against. Every "first added" date for this repo is really "first snapshot in which it appears."
3. **When the FNV canary constant `024a555471370b18d` was first minted.** It is in
   `AI-Writings/labs/quilt-kernel` (2026-09-30, EXTRACTION.md), `forge-quilt` (per EXTRACTION.md),
   `quilt-canary` (2026-09-23), and `cargo-line-tycoon` (2026-09-22). **I did not clone
   `forge-quilt` or `quilt-canary`** — I read the API listing and `_worker.js` references only. The
   2026-09-22 `POLYFORMALISM.md` is the earliest *statement* I found; the constant's true first
   appearance is unmeasured.
4. **The 5-opcode quilt VM family (§4.7).** Located by the repos API and code search; **not cloned**,
   so I cannot say whether `quilt-foundation`'s VM shares `cargo-line-tycoon`'s `computeAddress`
   formula, or only its FNV primitive. This is the single biggest hole left, because it is the most
   plausible home of a **pre-game address**.
5. **Whether the 6 lifecycle opcodes were ever formally adopted.** ~~`algebra.md` has exactly one
   commit~~ **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-algebra-second-commit](CORRECTION-algebra-second-commit.md)] → 2 commits
   (`1da6180bb77a`, 2026-10-18T07:46:49Z, added `result?: any`) and still lists six A**; `wr-obs13`
   names six B; `DIALECTIC_IN_11_OPCODES.md` ships six B (with `MERGE`→`MERGER`). **No adoption
   commit exists.** **The corpus asserts THREE elevens, not both — and the third is executable:**
   `cargo-line-tycoon/tests/sim/massive_simulation.js:16` carries eleven C with `MERGER`, and
   `POLYFORMALISM.md:58` states the same list in prose. See
   [CORRECTION-the-elevens-remeasured](CORRECTION-the-elevens-remeasured.md).**
6. **Whether the account's other ~5,285 repos contain further cell cores.** GitHub code search covers
   default branches only and does not index every repo; `path:packages/core/src/types.ts` returned
   **0** account-wide, which I read as "indexing gap", not as proof of uniqueness. I did not enumerate
   every repo's tree.
7. **Whether `SuperInstance/quilineer` exists.** `GET /repos/SuperInstance/quilineer` → **404 Not Found**.
   Either it was renamed or it never existed; I did not search the rename API.
8. **Byte counts.** `quilt-claw` = 210 KB repo size / 944 KB on disk, 26 commits.
   `cargo-line-tycoon` = 82,990 KB, 9 commits (all orphan roots).
   `quilt` = 15,272 KB, 218 commits, 15 branches.
   `qthe` = 3,907 KB, 110 commits. `AI-Writings` = 3,704,801 KB (**not cloned** — read via API +
   `raw.githubusercontent.com`, ~30 files).
   `quilt-claw-cells-game` = 14 KB, 1 commit. `quilt-cells` = 5 KB, 1 commit, 2 files (13,238 B).
9. **Test-count provenance.** I did **not** run `quilt-claw`'s suite (it cannot install:
   `EUNSUPPORTEDPROTOCOL` on `workspace:*`, measured by `18f7bc3` and independently reproducible).
   The 182/27 figure is corroborated from three sides: commit `18f7bc3`'s before/after TAP,
   `AI-Writings/609-the-slot-nothing-is-in.md`'s hand count, and my own static `grep -c "test("`
   over the 27 files in `test/` — I confirm **27 files**, and I confirm **zero `it(` blocks**.
   I take 182 on the strength of those three agreeing, not of a run I performed.

---

## 7. SKELETON — what I recommend you believe, and what I won't let you believe yet

```
2026-09-17 ─ quilt-claw ──── 25 KIND MODULES ─── no address, ever
                │
                ├─ imports @quilt/core,@quilt/ai  (workspace:* — never resolves)
                │
2026-09-19 ───┴─ zoo: 25 cards labelled 26 ────┐
                    _worker.js: KIND(24)+OP(11)  ├── THE WELD (AI-Writings/_worker.js)
                                               │
2026-09-20 ── lab/brew: the eleven is GROWN ───┤
2026-09-21 ── algebra.md/AGENTS.md = THE CHARTER 11 = 5 base + FORGET PROOF ROUTE CRDT WORLD TIME
              wr-obs13: six B = ATTEST CONTEST REVOKE WITHDRAW MERGE DELEGATE   ← canon contradicts itself
2026-09-27 ── cargo-line-tycoon: 6 opcodes RUN, 11 DOCUMENTED, fnv1a ADDRESS = 96de3eeaabd90b9b
2026-09-27 ── qthe: lattice cell, slot+resonance address, no hash
2026-10-18 ── quilt-claw finally sees fnv1a — as a LEDGER, ported from AI-Writings, on an unmerged branch
```

**The honest one-liner:** the three repos are three *portraits* of a model that was written down in
`AI-Writings` on 2026-09-21 and has been drifting ever since — the opcodes mutate, the kind count is
off by one, the address arrives in the game and never reaches the agent, and the "recognition"
mechanism in the canon is actually `qthe`'s wormhole table wearing a different hat.
