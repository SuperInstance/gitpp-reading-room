# FINDING-28 — three censuses, and the one fix that would have caught a quarter of the fleet

**Date:** 2026-10-18
**Sources:** `research/SILENT-GREEN-CENSUS.md`, `research/MAIN-POINTS-AT-NOTHING.md`,
`research/ADDRESS-PROPOSAL.md`
**Status:** the fleet's largest single measurement, in three parts

---

## 1. The pattern under all three

> **"No package here tests the artifact it publishes."** — the package census
>
> **"The fleet verifies that a command WAS INVOKED, never that it DID THE
> WORK."** — the silent-green census
>
> **"The fleet has independently arrived at the pair three times."** — the address proposal

Three different sweeps, three different methods, and **the same shape**: something that
*looks* like it is checked, and is not.

---

## 2. The package census — 455 versions, every one fetched

| | count | denominator |
|---|---|---|
| npm packages owned by `superinstance` | **161** | 162 names found; 1 is a name collision |
| versions downloaded and checked | **455** | 455/455, **zero fetch errors** |
| versions with a missing entry point | **131** | 28.8% |
| **`require()` provably throws** | **12** | 7.5% — all executed, all failed |
| `require()` OK, `types` missing (TS7016) | **18** | reported separately, not padded in |
| GitHub repos covered | **5,293** | 5,293/5,293 |

**The family**, reproduced verbatim: `adinkra-math`, `kintsugi-math` and `palaver-math` all
declare `dist/index.js` and ship `dist/src/index.js` — a `rootDir`/`include` mismatch. The
repo-side GraphQL predictor **independently rediscovered the same four packages** the tarball
analysis flagged. **Two methods, one answer.**

**The fix is one subprocess.** `npm pack`, untar to a temp directory, `require()` the entry
point, fail the publish if it does not load. That converts this entire class from *"a consumer
finds it"* to *"the build fails."*

**Two traps the census caught that would have produced wrong numbers:**

1. **npm's search index has a coverage gap.** `text=@superinstance` returns 143;
   `maintainer:` returns 161; **`scope:superinstance` returns 0**. `motifcode` and `openroom`
   appear in **neither** of the 143. A single-formulation sweep misses the instances we already
   knew about.
2. **The `repository` field lies.** npm's `adinkra-math` points at `SuperInstance/adinkra-math`,
   which is a **C library** — `gcc -std=c99`, `libadinkra.a`, no `package.json`. The real source
   is `adinkra-math-npm`. **Trusting `repository` produces confident wrong joins**, and a wrong join
   here means editing the wrong repo.

**And a correction to our own atlas: `openroom` is NOT a defect.** The census withdrew it — it is
a CLI, `node dist/cli.js --help` exits 0, and only `require()` fails, which nothing was meant to
do. We had it in the atlas as a finding. It is not one.

**The census also found two bugs in its OWN tooling** — a `re.S` comment-stripper that ate every
`package.json` containing `://` (hiding `palaver-math` itself), and an extension-dropping bug that
produced an absurd 138/138 "mismatch". Both are documented. **A census that hides its own errors
isn't a census.**

---

## 3. The silent-green census — 2,318 workflow files

| | count | denominator |
|---|---|---|
| repos scanned | **2,776** | of 5,293 (all counts are **floors**) |
| workflow files scanned | **2,318** | — |
| Python files scanned | **9,945** | — |
| `\|\| true` on a test/install step | **161** | across 135 repos |
| coercion instances (`== '0'`) | 3 | — |
| **uses of the CORRECT `steps.*.outcome`** | **0** | across 2,318 workflows |
| badges with `?event=` | **0** | of 34 |

**`|| true` came from ONE template.** `ci-rust-python.yml` propagated it across ~10 repos. **One
decision, not 135 bugs.**

**The polarity the first finding hid.** The census found the `== '0'` shape — the cosmetic one. It
also found **`!= '0'`, where the comparison IS the sole fail-closed mechanism.** Strictly worse,
and the first finding drew the eye away from it.

**The worst three:**

1. **`plato-room-deployment-approval`** — **reproduced by execution**: `ModuleNotFoundError` *and*
   `AttributeError: 'tuple' object has no attribute 'block'`. **Defect 1 masks defect 2** — fixing
   the import alone still leaves the gate dead.
2. **`git-agent`** — `twine upload` under `continue-on-error`, then an **unconditional GitHub
   Release**. Green job, no PyPI upload, release published anyway.
3. **`jev-lattice/action.yml`** — a published gate action whose **sole** enforcement is
   `code != '0'`, which `''` defeats.

**And the cheapest fix available, which is a deployment failure rather than a detection
failure:** the account **already contains a working detector** —
`plato-room-code-review/reviewer.py:148,150` flags `continue-on-error` and `set +e` — **wired
into exactly one repo, its own.** Meanwhile `quilt-k3s`, `jev-lattice` and two `quilt-base`
security scanners still carry the smell. **The detector works. It is not deployed.**

**Two self-corrections by the census, both worth keeping:**

- `pong-quilt/merge-gate.yml` looked like the worst instance. It is an **intentional canary** with
  a second full-suite job.
- `node --test test/*.test.mjs` is **not** a defect — bash expands the glob, so all files run. The
  real shape (`node <glob>` *without* `--test`) is already at **zero**, and `quilt-claw/test.yml`
  is an in-repo post-mortem of it: *"It ran 4 of 182 and exited 0 — a green result over a truncated
  suite."*

**The generalisable finding, which is the sentence to remember:**

> **The fleet verifies that a command WAS INVOKED, never that it DID THE WORK.**
> `''` reads as `'0'`, `except` reads as a verdict, `|| true` reads as success.

---

## 4. The address proposal — the fleet's one unclaimed idea, and it is not what we thought

**The verdict: the four-tuple is wrong. Ship the pair.**

```
CellId { site: u32, seq: u64 }     12 bytes packed, 16 aligned
Digest = BLAKE3-256(0x01 ‖ len(kind) ‖ kind ‖ len(payload) ‖ payload ‖ parents)
```

`site` is provisioned once per vessel and never reused; `seq` increments per state change and is
never reused. **Keep the content hash — demote it from address to digest.**

**Three claims from the cutting-edge survey were WRONG, and one was right for the wrong reason:**

1. **"Yjs addresses every CRDT item as `(client, random uint32, clock)`" is false.**
   `src/utils/ID.js:10` is `constructor(client, clock)` — **a pair**. The random field is the
   *client*, it is **`uint53`** not uint32, and `ID.js` contains no random field at all.
   **The survey's load-bearing claim collapses — and what replaces it is better: the most-deployed
   CRDT identifier on earth is a pair.**
2. **"RFC 9153 is a regulated 4-field wire format" is false.** It is **Informational, not an
   Internet Standards Track specification**, and it is *not* a wire format — the format is
   **ASTM F3411-19**, a Message Pack of **seven** message types. And *"network degraded, and
   network denied"* **appears nowhere in the document** — 0 hits for both words.
3. **"`algebra.md:129`" is the wrong line.** Line 129 is `sclerotic → frozen`. The four-tuple is
   at **line 152**; the hash-ID clause at **150**. The offset is +23 — the second commit's
   insertion. **Nobody opened the file after 07:46 that day.** The same defect: `lib.rs:86` for
   `COUNT = 9`, which is at **85**.

**But the survey UNDER-called RFC 9153, and this is the part that matters.** At `:1286` the IETF
wrote that binding an ID to its signature in one frame is **"infeasible"** under a 25-byte budget —
so they **paged authentication OUT of the address** and correlated by MAC instead.

**That is the empirical refutation of `sig` as a fourth field, in the tightest regulated
environment ever specified.**

**The fourth field has no job. Nor the first two.** `@quilt/core` — the only model with a
published npm version — already ruled, at `types.ts:42`:

> *"STABLE IDENTITY OVER COORDINATES … Coordinates are display-only."*

And `quilt-rust` (the `no_std` target, 9 of 11 opcodes) **already is** the proposal:
`Cell { id, value, reads, version: u64 }` + `Engine { tick: u64 }` at `lib.rs:118-136`. And
`qthe` already is too: `read(slot)` returns `{x: this.sx[slot], …}`, so **slot is the address and
x,y are the payload** (`qthe.mjs:105-126`).

**The fleet has independently arrived at the pair three times.**

**The counter-argument, steelmanned:** the pair has **no geometry**, and `ROUTE` / "what is near
me?" is the charter's headline. Spatial queries become O(n) scans in 512 KB. Demoting `x,y` to a
projection moves the problem rather than solving it. **The answer, from the charter itself:**
`algebra.md:162`, *"Don't invent new abstractions"* — a four-tuple requires an **allocator**, and
*"without a map"* means no allocator, so **without an allocator there is no `x`.**

**The decisive axis is DERIVABILITY, not lookup cost.** A content hash is O(payload), so on an S3
you cannot address a cell larger than free RAM. **The four-tuple at 80 bytes is not constructible
on bare metal** — `sig` needs the witness, and `x,y` need an allocator.

**And the charter's contradiction, named:** **strike clause 3, rewrite the heading to "The Address
Is the Cell."** Tahoe already resolved this exact tension: *"Mutable files are places with a
stable identifier… fixed for the life of the slot, regardless of what data is placed inside it."*
There is also a contradiction nobody had named: `:108` ("same id and kind ARE the same cell") makes
cells **un-updatable**, which makes law 5 meaningless.

**Three gaps the proposal admits, honestly:**

1. durable **network-free actor provisioning** (eFuse as a CRDT actor id — nothing found)
2. **privacy leakage of `(site, seq)`** at fleet scale (nobody addresses it)
3. *"a place"* without an allocator — a gap in the **spatial index**, not in the address

---

## 5. What the three censuses change about how we work

| the census | the rule it yields |
|---|---|
| packages | **Publish the artifact, not the tree.** `npm pack` → untar → `require()` → fail. In `prepublishOnly`, for every package. |
| silent-green | **Assert that the thing measured was measured.** `steps.*.outcome`, never a string comparison against an output that may be empty. |
| silent-green | **Deploy the detector you already have.** `plato-room-code-review/reviewer.py` exists and works. It is wired into one repo. **That is the cheapest fix in this finding.** |
| silent-green | **One decision, not 135 bugs.** Fix the template; the dependents follow. |
| address | **Keep the hash; demote it to a digest.** The address is a small, provisioned, never-reused pair. |
| address | **An address must be constructible on the smallest device.** A hash is O(payload); a place needs no allocator. |

## 6. The honest limits

- The package census's **registry side is measured**; its repo side is **predicted**. It proved 1 of
  16 by building it, and one predicted-broken package turned out fine. **The other 15 are leads, not
  findings.**
- The silent-green census covered **2,776 of 5,293 repos**. **Every count is a floor.**
- The address proposal could not verify `quilt-substrate-meta`, NDN, or `_worker.js` (404), and it
  flagged that the cached `intercontext.json` in the working directory was **stale** (53 KB vs
  74 KB on `main`).
- **The magnitudes in the ledger-anchor experiment are not established across document shapes.**
  Do not quote them as constants.
