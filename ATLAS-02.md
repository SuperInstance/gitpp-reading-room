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

> **Rank on source bytes by extension, minus vendored paths — never on
> tree size, never on file count, and never on test-file count without
> excluding the vendored ones.**

---

## 1. Where the gold is

**It is a band of roughly eight to twelve genuinely finished systems, not
a long tail of stubs.** The 210 original repos total only ~191 MB of
junk-excluded source, and **about 60% of that sits in four repos**.

### The four biggest

| repo | src bytes | files | tests | CI | pushed | what it is |
|---|---|---|---|---|---|---|
| **`PersonalLog`** | 13.9 MB | 2,579 | **142** | 2 | 2026-10-17 | Local-first journal/mood/messenger. Next.js 15 + Python `personal_log` + **Rust→WASM** vector math |
| **`SuperInstance-papers`** | 30.4 MB | 4,986 | **271** | 1 | 2026-10-11 | 73 formal papers **and** the spreadsheet/cell substrate implementation |
| `cloudflare-vibe` | 4.0 MB | 599 | 31 | 1 | 2026-05-08 | rebranded/forked Cloudflare VibeSDK, ~3 MB of local change |
| `cargo-line-tycoon` + `Scrapcraft` | 3.6 MB | — | 87 | 5 | — | see below |

### The finished-and-resting tail

Small, complete, tested, and resting:

```
jev-lattice        243 KB   8 test files   pushed the day of this survey
base60-lattice     186 KB   9 test files   2 CI workflows
fleet-pipeline     126 KB   9 test files   3 CI workflows
quilt-swarm        106 KB   5 test files   1 CI workflow
collective-unconscious 231 KB  10 test files  2 CI
fleet-connections  264 KB  12 test files   1 CI
equilibrium-tokens 166 KB  4 test files    Rust, formal, self-contained
```

---

## 2. The four graduation candidates

### `PersonalLog` — the strongest, and the exemplar of the model

**430,636 lines** across `src/`, `packages/`, `personal_log/` (Python)
and `native/rust/` (WASM). **142 test files** (90 under `src/`, 27 under
`packages/`, 25 under `tests/`). Two CI workflows. **19 package scripts**
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

```
substrate/c/cell_algebra.c        substrate/c/test_algebra.c
substrate/rust/src/lib.rs         substrate/py/clt_substrate/__init__.py
```

**That is the git.pp cross-port byte-equivalence invariant — the
load-bearing claim of the whole spec series — independently implemented
inside a game repo, in four languages, with a pinned canary, and nobody
had looked at it.** Seven locale trees (`ar, en, es, ja, pt, vi, zh`) —
pedagogy as a first-class code path.

**`Scrapcraft`** is the other half: 4,611 lines across 16 test files —
`firmware-golden-tests.mjs`, `firmware-equivalence-tests.mjs`,
`kinematics-tests.mjs`, `seeded-rng-tests.mjs`, `quilt-bridge-tests.mjs`,
`challenge-replay-tests.mjs`, `content-integrity.mjs`,
`drive-gesture-tests.mjs` — and programs that **compile to real Arduino
C++ and MicroPython firmware**. A genuine compiler path, not a
simulation veneer. **3 stars — the highest in the entire 301-repo set.**

### `git.pp-ng` — the best engineering in the set, and the same defect I fixed today

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
`quilt-mesh`, `quilt-learn` and `loom-core` — share a **byte-identical
97,486-byte JS + 71,216-byte TS core pair** (7,930 bytes common to all
seven). **One codebase under seven names.**

And this **qualifies ATLAS-01's addendum**: it recorded 259 `quilt-*`
repos as "the live edge of the sketchbook", which is true by *count* and
misleading by *substance*. Of the 39 `quilt-*` repos in this set, only
`quilt-dba` (4 tests), `quilt-nn` (5), `quilt-swarm` (5) and
`quilt-cloudflare` (4) have meaningful test counts.

**`prism` and `Claude-prism-local-json` are one codebase.** 53 identical
source filenames, identical `package.json` (`claudes-friend` v0.3.1, same
deps, same 24 scripts), **4,336,631 bytes byte-identical** including
`prism/src` (501,779 B) and `prism/tests` (233,734 B). The variant is
the storage backend — D1/Vectorize versus local JSON. **Two of the
highest test counts in the set (79 and 74) are the same tests counted
twice**, so the set's real independent test mass is smaller than the raw
totals suggest.

### A family of accident backups, left in place

Eight repos named `recovered-copy-20260824-*` are near-exact clones of
live work, all dated **2026-08-24**:

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
2. **`git.pp-ng`** — best engineering in the set and the one with no CI at all. A differential + scale conformance harness and an arrival-order-invariance proof. **The same fix as `canon-graph`: add the workflow.**
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
