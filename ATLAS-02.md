# ATLAS-02 — the gold map: 301 uncharted sketches

*2026-10-18. Companion to [ATLAS-01.md](ATLAS-01.md).*

**Method.** `snowball-scout` surveyed the 301 repos whose declared npm
package name 404s on the registry and which are not third-party names —
the least-explored part of the sketchbook. **Read-only throughout: no
writes, creates, pushes, publishes or deletes.** Per-file byte counts came
from the GitHub tree API for all 210 non-fork repos; sparse `depth=1`
clones (`node_modules/`, `target/`, `dist/`, `build/`, `.next/`,
`public/art/`, `public/audio/`, `assets/` excluded) for content reading.

---

## 0. The measurement caveat, which changes how everything else is ranked

> **Repository `size` fields in this account are wildly misleading.**
> Committed `node_modules`, `target/` and asset trees dominate the
> headline numbers.

| repo | reported size | reality |
|---|---|---|
| `flux-wasm` | 114 MB | **99.9% is `target/`** — 110,344 bytes of real Rust |
| `hermes-memory-mcp` | 1,020 files / 31.7 MB | **685 bytes** of actual source |
| `fabric-mcp` | 42 MB | contains a committed `.git-rewrite/` git database of **61 MB** — the reported number is incoherent |
| `flux-lsp` | 6,450 files | 226 KB of real TypeScript |

**And test-count ranking is inverted by vendoring.** `fabric-mcp` appears
to have **497 test files** — the highest in the set. **It has 5.** Every
one of the other 492 belongs to the vendored OpenClaw fork's macOS, iOS
and Android test targets. **Any ranking keyed on test-file count alone
puts this repo first and it would be wrong.**

> **Corrected 2026-10-18 — the conclusion holds, the bookkeeping was
> half wrong, and the 497 is unmeasurable.** Measured over its
> recursive tree: **256** files with a test-like name. Of those,
> **238 are inside `.git-rewrite/`** — the committed 61 MB git database
> — and **14 are under the vendored `claw*/` trees. That leaves 4
> outside vendored paths.** No pattern I used produces 497, and "5" is
> 4 under mine. The conclusion — effectively **zero original tests** —
> is if anything stronger than published.

> **Rank on source bytes by extension, minus vendored paths — never on
> tree size, never on file count, and never on test-file count without
> excluding the vendored ones.**

---

## 1. Where the gold is

**Four repos in this set are large enough to be worth opening, and one
of them (`PersonalLog`) documents its own unreliable tiers in its own
README.** The 210 original repos total only ~191 MB of junk-excluded
source, and **about 60% of that sits in four repos**.

> **CORRECTED 2026-10-18 — this previously read "a band of roughly
> eight to twelve genuinely finished systems".** The hedge on the
> *count* was doing work the hedge on the *quality* was not: the same
> document quotes `PersonalLog`'s README saying its JS unit-test tier is
> unreliable and its UI is scaffolded, and then counts it as finished.
> Size plus a stated limit is the thing worth looking for; size alone is
> what the raw ranking measures, and the raw ranking is what §0 exists
> to distrust.

### The four biggest

| repo | src bytes | files | tests | CI | pushed | what it is |
|---|---|---|---|---|---|---|
| **`PersonalLog`** | 13.9 MB | 2,579 | **142** | **1** | 2026-10-17 | Local-first journal/mood/messenger. Next.js 15 + Python `personal_log` + **Rust→WASM** vector math |
| **`SuperInstance-papers`** | 30.4 MB | 4,986 | **271** | 1 | 2026-10-11 | 73 formal papers **and** the spreadsheet/cell substrate implementation |
| `cloudflare-vibe` | 4.0 MB | 599 | 31 | 1 | 2026-05-08 | rebranded/forked Cloudflare VibeSDK, ~3 MB of local change |
| `cargo-line-tycoon` + `Scrapcraft` | 3.6 MB | — | 87 | **3 + 2** | — | see below |

> **CORRECTED 2026-10-18 — the `CI` column was inflated, in four places.**
> Re-measured as `len([p for p in tree if p.startswith('.github/workflows/')])`
> over each repository's recursive tree:
>
> | repo | this document said | measured |
> |---|---|---|
> | `PersonalLog` | 2 | **1** — `.github/workflows/ci.yml` |
> | `Scrapcraft` | 5 | **2** |
> | `fleet-pipeline` | 3 | **2** |
> | `collective-unconscious` | 2 | **1** |
>
> and three unstated values, now stated: `jev-lattice` **1**,
> `equilibrium-tokens` **1**, `trinity-marine-station` **0**.
>
> Every one of the corrected values was **higher-confidence evidence
> claimed than exists** — which is the failure mode ATLAS-01 §4 exists
> to catch, appearing inside the document that writes the rule. The
> direction is consistent (always too high), which suggests the column
> was read off README badges rather than counted off the tree.

### The finished-and-resting tail

Small, complete, tested, and resting:

```
jev-lattice        243 KB   8 test files   pushed the day of this survey   CI: 1
base60-lattice     186 KB   9 test files   CI: 2
fleet-pipeline     126 KB   9 test files   CI: 2   (was 3)
quilt-swarm        106 KB   5 test files   CI: 1
collective-unconscious 231 KB  10 test files  CI: 1  (was 2)
fleet-connections  264 KB  12 test files   CI: 1
equilibrium-tokens 166 KB  4 test files    Rust, formal, self-contained    CI: 1
```

---

## 2. The four graduation candidates

### `PersonalLog` — the strongest, and the exemplar of the model

**430,636 lines** across `src/`, `packages/`, `personal_log/` (Python)
and `native/rust/` (WASM). **142 test files** (90 under `src/`, 27 under
`packages/`, 25 under `tests/`). **One** CI workflow —
`.github/workflows/ci.yml`. *(Corrected 2026-10-18: this said "Two CI
workflows." Measured: one. A strict-path count of test files gives 141,
against the 142 claimed here — within one file, and not corrected.)*
**19 package scripts**
including `test:unit`, `test:integration`, `test:e2e`, `test:a11y`,
`test:perf`, `test:smoke`, `build:wasm`, `verify:wasm`, `verify:build`,
`verify:deployment`. Playwright at root and a separate smoke config.
`BETA_TEST_REPORT.md`, `SECURITY.md`, `CONTRIBUTING.md`, `CHANGELOG.md`.
Pushed 2026-10-17, one day before the survey.

**And this is the part worth copying.** Its own README says:

> *"large parts of the UI are scaffolded or mocked-up and the JavaScript
> unit-test suite is not yet reliable"* — the JS tests *"need proper
> browser-API mocking before CI can enforce them."*

So: backend, logic and WASM are verified; the UI is not; the JS unit tier
is unreliable. **It says so, in the repository, about itself.** That is
the boundary-honesty rule of ATLAS-01 §4, applied spontaneously by a
sketch that never left the sketchbook.

> **A sketch that states where it is unreliable is more valuable than a
> sketch that claims it is fine. That is the house standard, and
> `PersonalLog` already meets it.**

### `cargo-line-tycoon` — the polyformalism proof, shipped

**Three CI workflows** including a PyPI publishing pipeline. 33 test
files, 1,460 lines of test code, all wired into `npm test`.

**The same 11-opcode cell algebra implemented in TypeScript, Rust, Python
and C99**, cross-checked by an **FNV-1a 64-bit canary pin
`0x024a555471370b18d`** for byte-exact parity:

> **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-rust-is-nine](CORRECTION-rust-is-nine.md)]**
> **This sentence is half true, and the half that is true is the half that
> mattered — so the sentence was load-bearing on a false conjunction.**
> The **canary parity is real**: `substrate/rust/src/lib.rs:17-18` pins
> `FLEET_CANARY_INPUT = "café Δ 日本語"` → `FLEET_CANARY_HASH = 0x024a555471370b18d`,
> and its header says it exists to produce *"identical FNV-1a 64-bit hashes,
> identical signal-chain behavior, and identical cell-graph semantics."*
> **The "same 11-opcode cell algebra in four languages" is FALSE.** Measured from
> the contents API on `main`, 2026-10-18:
>
> | port | path | bytes | blob | opcodes |
> |---|---|---|---|---|
> | TypeScript | `substrate/ts/src/index.js` | 13,917 | `90f04ea95eee` | **6** BIND LINK EFFECT VIEW TICK ATTEST |
> | TypeScript (SDK) | `browser/substrate-sdk.js` | 11,375 | `c94654e3520d` | **4** BIND ATTEST CONTEST WITHDRAW |
> | C99 | `substrate/c/cell_algebra.{c,h}` | 1,570 / 44 ln | `549b8ddea82c` | **5** BIND LINK EFFECT VIEW TICK |
> | Python | `substrate/py/clt_substrate/__init__.py` | 9,935 | `55d889b53dcd` | **1** TICK |
> | Rust (this port) | `substrate/rust/src/lib.rs` | 11,494 | `90841acce4ed` | **0** |
> | **Rust (`quilt-rust`)** | `crates/quilt-polyformalism/src/lib.rs` | 44,728 | `e629bb908e45` | **9** |
>
> **Four ports of a *hash* is not four ports of an *algebra*, and this entry used
> the former to underwrite the latter.** The repo's own doc disagrees with this
> atlas: `cargo-line-tycoon/POLYFORMALISM.md:96-98` lists all three substrate
> ports as **unchecked checkboxes** — `- [ ] substrate/rust/ — signal-chain Rust
> port` — *signal-chain* twice, *unchecked*. **The repository describes its own
> ports as signal-chain parity with the algebra still to do. This atlas read the
> checkboxes as done.** The claim survives in a different form: the hash parity
> is four-port, real, and independently arrived at.

```
substrate/c/cell_algebra.c        substrate/c/test_algebra.c
substrate/rust/src/lib.rs         substrate/py/clt_substrate/__init__.py
```

**That is the git.pp cross-port byte-equivalence invariant — the
load-bearing claim of the whole spec series — independently implemented
inside a game repo, in four languages, with a pinned canary, and nobody
had looked at it.**

> **[CORRECTED 2026-10-18T15:19Z — [CORRECTION-rust-is-nine](CORRECTION-rust-is-nine.md)]**
> **"independently implemented … in four languages" is overstated: four languages
> implement the *canary*, not the algebra (6/5/1/0).** And "independently" is the
> weakest word here: all four ports pin the **same literal**, `0x024a555471370b18d`,
> and `substrate/rust/src/lib.rs:4` names the goal as *"identical … as the
> TypeScript, Python, C, and Go ports"* — **a port of a port, by the same author,
> which is exactly the condition this reading room exists to distrust** (README:
> *"the spec, the corpus, the reference and the test runner were written by the same
> person"*). **The parity is real and it is weaker evidence than this entry claimed:**
> it is evidence that four copies of one constant agree, not that four
> implementations agree. The genuinely independent check on a cell model in this
> fleet remains `quilt-rust`'s 9/11 port against the game's 6/11 — **two different
> opcode sets in two different repositories**, which is the first cross-model check
> the substrate has ever had. Seven locale trees (`ar, en, es, ja, pt, vi, zh`) —
pedagogy as a first-class code path.

**`Scrapcraft`** is the other half: 4,611 lines across 16 test files —
`firmware-golden-tests.mjs`, `firmware-equivalence-tests.mjs`,
`kinematics-tests.mjs`, `seeded-rng-tests.mjs`, `quilt-bridge-tests.mjs`,
`challenge-replay-tests.mjs`, `content-integrity.mjs`,
`drive-gesture-tests.mjs` — and programs that **compile to real Arduino
C++ and MicroPython firmware**. A genuine compiler path, not a
simulation veneer. **3 stars — the highest in the entire 301-repo set.**

### `git.pp-ng` — the best engineering in the set, and the same defect I fixed today

> **GRADUATED 2026-10-18. This repository has left the sketchbook.**
> `GET /repos/SuperInstance/git.pp-ng` → **HTTP 301** →
> **`patchwork-quilt/git.pp-ng`**. The `patchwork-quilt` organization
> was created **2026-10-03** and holds 6 repositories.
>
> This matters for the recommendation below: **you cannot add a
> workflow to it from here**, and "the same fix as `canon-graph`: add
> the workflow" is no longer actionable from this account. The finding
> stands — it is still the best-engineered repo in the set and still
> has no `.github/` — but the fix is now someone else's pull request.
> See [ATLAS-03.md](ATLAS-03.md), which already recorded the
> graduation.

`conformance/` holds a **differential test harness** —
`differential.ts`, `bench_engine.ts`, `prod_scale_driver.ts`,
`prod_scale.py`, `runner.ts` — plus **4,292 lines of tests** across six
named suites including `security-probes.mjs`, `worker-delta.mjs` and
`format-v2.mjs`. Documentation set is unusually disciplined:
`DESIGN.md`, `docs/ENGINEERING.md`, `docs/FORMAT-V2.md`,
`docs/CHECKPOINT-V2.md`, `docs/OPEN-QUESTIONS.md`, `docs/CUTTING-EDGE.md`,
`RESULTS.md`. Design: task as atom, per-task forked repos, state
*derived by folding* the completion record, with **arrival-order
invariance** as an explicit property, and a demo video committed.

**It has no `.github/` directory at all.** It is the only top-25 original
with substantial test infrastructure and **zero CI** — the same defect
class I fixed in `canon-graph` and `canon-claim` today, in a much larger
repository. The fix is the same one: add the workflow.

---

## 3. Duplication — the real structural finding

**The `quilt-` prefix is a naming family, not 39 systems.** Seven
repos — `quilt-arcade`, `quilt-arena`, `quilt-quant`, `quilt-dba`,
`quilt-mesh`, `quilt-learn` and `loom-core` — share an **identical
engine**. **One codebase under seven names.**

> **CORRECTED 2026-10-18 — the byte counts in this paragraph were not
> real.** It previously read "a byte-identical 97,486-byte JS +
> 71,216-byte TS core pair (7,930 bytes common to all seven)". Measured
> across all seven recursive trees: **97,486, 71,216 and 7,930 do not
> occur as the size of any file in any of the seven repositories.** All
> three numbers are gone.
>
> What is actually there:
>
> | measurement | value |
> |---|---|
> | paths common to all seven | 63 |
> | of those, byte-length-identical in all seven | **60** |
> | total bytes across those 60 identical paths | **232,414 B** |
> | largest shared file | **`engine/engine.js`, 28,145 B** |
>
> `engine/engine.js` is identical in all seven down to the **git blob
> SHA** — `66edc4a71a…`, md5 `7084c4c33ffc62e5fa04761968a5cc39`. It is
> not merely the same length; it is *the same object*, committed
> independently into seven repositories.
>
> **One qualification the original sentence hid:** the seven are not
> clones. Their `package.json` files are **seven different files**
> (seven distinct md5s, 72–934 B). What is duplicated is the engine;
> the packaging is not. "One codebase under seven names" is right about
> the engine and wrong about the repositories.

And this **qualifies ATLAS-01's addendum**: it recorded the `quilt-*`
family as "the live edge of the sketchbook", which is true by *count*
and misleading by *substance*. Of the 39 `quilt-*` repos in this set,
only `quilt-dba` (4 tests), `quilt-nn` (5), `quilt-swarm` (5) and
`quilt-cloudflare` (4) have meaningful test counts.

> **This was already corrected once, in a different file.**
> [CORRECTION-quilt-namespace.md](CORRECTION-quilt-namespace.md) §3
> carries "the shared quilt core is 28,145 bytes, not 97,486 + 71,216",
> and rule 69 generalises it: *a count is only as good as the thing it
> counted*. **It was published 2026-10-18 and ATLAS-02 was left holding
> the wrong numbers with no pointer to it.** The correction below is the
> link that was missing, plus the two measurements the earlier
> correction did not make (identical blob SHA; `package.json` differs in
> all seven).
>
> **Correction to the count as well:** the addendum said **259**
> `quilt-*` repositories. The glob matches **257** — `quilt` (the
> spreadsheet) and `quiltcall` (9 bytes) do not match `quilt-*`.
> Seven is a subset of the 257, and the other 250 remain unmapped, so
> this paragraph qualifies the *substance* of the family claim and says
> nothing about the rest of it.

**`prism` and `Claude-prism-local-json` are one codebase.** 53 identical
source filenames, identical `package.json` (`claudes-friend` v0.3.1, same
deps, same 24 scripts), **4,336,631 bytes byte-identical** including
`prism/src` (501,779 B) and `prism/tests` (233,734 B). The variant is
the storage backend — D1/Vectorize versus local JSON. **Two of the
highest test counts in the set (79 and 74) are the same tests counted
twice**, so the set's real independent test mass is smaller than the raw
totals suggest.

### A family of accident backups, left in place — and thirty-one of them are empty

> **Also already corrected elsewhere, also unlinked.**
> [CORRECTION-quilt-namespace.md](CORRECTION-quilt-namespace.md) §3
> records "the 2026-08-24 incident was sixty repositories wide, not
> eight" (59 `recovered-copy-*` plus the `rc-20260824-*` family), and
> rule 69 names the cause. **What that correction did not measure is
> that 31 of the 59 are empty**, which is the part worth having.
>
> **CORRECTED 2026-10-18. This said "Eight repos named
> `recovered-copy-20260824-*`". There are 59.** Re-measured from
> `GET /api/users/SuperInstance/repos`: **59** repositories match
> `recovered-copy-20260824-*`.

**And the real finding is not the near-clones. It is that 31 of the 59
are completely empty.**

| measurement | value |
|---|---|
| `recovered-copy-20260824-*` repositories | **59** |
| **empty (`size == 0`)** | **31** |
| non-empty | 28 |
| largest non-empty | **65,516 B** |
| ≥ 1 MB | **0** |

```
sizes, ascending:
0 ×31, then 2, 4, 4, 5, 19, 20, 22, 22, 27, 28, 35, 37, 52, 60, 62,
93, 123, 137, 183, 219, 236, 275, 307, 471, 578, 1460, 19736, 65516
```

**Half of them are tombstones** — repositories that exist, are not
forks, are not archived, and contain nothing at all. A GitHub repository
at `size 0` was created, initialised to nothing, and abandoned. Fifty-nine
of them, all stamped **2026-08-24**, and **not one of the 59 exceeds
66 KB.**

**That is a far more legible record of an incident than eight
near-clones**, because it says what happened rather than what was
copied: something on 2026-08-24 walked a list of repositories and
produced a recovery artifact for each — and for **31 of them there was
nothing to recover, so the artifact is an empty repository that still
counts as a repository.** The accident was not "some repos got
duplicated." The accident was **a recovery pass that created 31 empty
repositories and did not delete them.**

The eight originally listed were the eight largest of the 59 and remain
representative of the 28 non-empty ones:

```
recovered-copy-20260824-mist-game              clone of mist-game              42,294,315 B identical
recovered-copy-20260824-scrap-quilt            clone of scrap-quilt              252,945
recovered-copy-20260824-elephant-sim-worker    clone of elephant-sim-worker      174,911
recovered-copy-20260824-mist-quilt             clone of mist-quilt               124,607
recovered-copy-20260824-scrap-voice            clone of scrap-voice              111,859
recovered-copy-20260824-mist-lab               clone of mist-lab                 105,987
recovered-copy-20260824-mist-voice             clone of mist-voice                63,409
recovered-copy-20260824-fishinglog-ai          clone of fishinglog-ai            111,430
```

They are the only surviving record of an apparent 2026-08-24 incident.
One of them — `-fishinglog-ai` — diverged from its original in a way
that suggests the recovery was partial: 111 KB of source and **zero
tests** where the original has them.

> **Two of the repos counted as independent finished work in §1 are in
> this family.** `base60-lattice` and `collective-unconscious` are
> listed above under "the finished-and-resting tail", and both have
> `recovered-copy-20260824-*` twins. **This document did not reconcile
> that** — it counted them as finished evidence in §1 and as accident
> residue in §3. Flagged rather than silently fixed, because which of
> the two readings is right depends on whether the twin is the *source*
> of the work or its *wreckage*, and that is a question about the
> 2026-08-24 pass, not about this survey.

**`fleet-static-host` and `rc-20260824-01` are the same repository.**
43,676,142 bytes across 216 byte-identical files, led by a 41,578,025-byte
`public/mist/` asset tree — but they also share real code:
`build_site.py` (28,005 B), **`src/quilt.ts` (27,367 B)**,
`seed/build_seed.py`, `migrations/0001_quilt_schema.sql`.
**`rc-20260824-01` has no description, no topics and no README.** One is
a live static host, the other an unnamed working copy, and neither is in
the published set, so neither is documented anywhere else.

---

## 4. Excluded: vendored upstreams, including two that hide

**91 of the 301 are `fork=true`** and excluded outright, including a
`*-log-ai` family of 22 and a `craftmind-*` family of 9 — one template
instantiated about 40 times.

**Two more hide behind `fork=false`:**

| repo | actual upstream | evidence |
|---|---|---|
| `cloudflare-vibe` | `cloudflare/vibesdk` | README and Deploy-Buttons links point at `github.com/cloudflare/vibesdk`; `ci-python.yml` retained; package renamed `lucineeer` |
| `fabric-mcp` | `openclaw/openclaw` | `claw/README.md`: *"**claw** is the SuperInstance fork of OpenClaw"*; `claw/package.json` has `"name": "openclaw"`, `"repository": "github.com/openclaw/openclaw"`. 118,830 lines of Swift, 47,300 of Kotlin |

---

## 5. The house ritual, visible in the artifacts

`prism` alone ships nineteen root status documents:

```
ACTION_PLAN.md  AUDIT_REPORT.md  DEPLOYMENT_COMPLETE.md
DEPLOYMENT_INSTRUCTIONS.md  DEPLOYMENT_READY.md  DOCUMENTATION_SUMMARY.md
HOTFIX_NEEDED.md  MERGE_READY.md  MVP_COMPLETE.md  OPEN_SOURCE_RELEASE_SUMMARY.md
PRODUCTION_DEPLOYMENT.md  PRODUCTION_READY_SUMMARY.md  PULL_REQUEST.md
ROUND2_COMPLETE.md  ROUND3_COMPLETE.md  SHARE_WITH_DEVELOPERS.md
TROUBLESHOOTING.md  USER_GUIDE.md
```

plus `DOCKSIDE-EXAM.md` and `BOOTCAMP.md` recurring across `murmur-agent`,
`spreader-agent` and `keeper-agent`.

> **The thinking is preserved in commits and docs, exactly as described.**
> That is the model, and the artifacts confirm it was real.

And one small thing worth recording: `fabric-mcp` contains files
literally named `C:Userscaseypollnsimulationsrequirements.txt` and
`C:Userscaseypollnspreadsheet-momentdocsONBOARDING.md` — backslashes
eaten by a shell during a copy. A snapshot of a working machine's
directory, kept as-is.

---

## 6. The ten to look at next

1. **`PersonalLog`** — largest, most complete, most recently pushed, and the clearest graduation candidate. 430K LOC, 142 test files, three-language runtime, honest self-assessment.
2. **`patchwork-quilt/git.pp-ng`** *(was `SuperInstance/git.pp-ng` until it graduated on 2026-10-18)* — best engineering in the set and the one with no CI at all. A differential + scale conformance harness and an arrival-order-invariance proof. **The same fix as `canon-graph`: add the workflow — now against `patchwork-quilt/git.pp-ng`, not from this account.**
3. **`SuperInstance-papers`** — 533K LOC, 271 test files, 73 numbered papers with named theorems, and a live Cloudflare deployment. The only repo where the theory and the implementation are in the same tree.
4. **`cargo-line-tycoon`** — the polyformalism proof: one 11-opcode algebra in four languages with byte-exact canary parity, seven locales, a PyPI pipeline.
5. **`Scrapcraft`** — 4,611 lines of tests including firmware golden/equivalence; compiles to real Arduino C++ and MicroPython.
6. **`the-tap`** — 2,936 lines of tests, CI that installs a Rust toolchain, Durable Objects, and `KNOWN-ISSUES.md` / `PRODUCTION-LOG.md`.
7. **`fleet-static-host` / `rc-20260824-01`** — 43.7 MB byte-identical, with real code (`src/quilt.ts`, `build_site.py`) inside a 41.6 MB asset tree. One is an unnamed working copy with no README.
8. **`trinity-marine-station`** — 25 test files in 78 files, no README, no description. Undocumented and disproportionately well-tested.
9. **`quilt-murmur`** — largest of the 39 `quilt-*` repos (1.4 MB, 2 CI workflows) and **zero tests**. The biggest untested surface.
10. **`jev-lattice`** — pushed **2026-10-18, the day of this survey**. A proof-contract lattice with committed calibration data in `experiments/results/*.json`. Being worked on right now.

---

## 7. What the scout could not determine

Stated, not papered over:

- whether any CI workflow currently **passes** — definitions were read, not run;
- the pass rate of any test suite — test *files* were counted and sampled, suites were not executed;
- whether `recovered-copy-20260824-*` was an intentional archive or an accident — no README or commit explains it;
- what `trinity-marine-station` actually is, and which upstream if any it derives from.

**Read-only compliance:** no writes, creates, pushes, publishes or deletes
were performed against any repository. All GitHub access was via
read-only REST endpoints and `git clone`/`fetch` into `/tmp`.
