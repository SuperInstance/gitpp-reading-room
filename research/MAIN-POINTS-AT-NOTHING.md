# MAIN POINTS AT NOTHING
### A census of "the install is the happy path and the use is the failure"
### SuperInstance / npm — read-only survey. Nothing was published, pushed, or modified.

**Date:** 2026-10-18 · **Scope:** npm packages owned by the npm user `superinstance`,
and all 5,293 public repos of the GitHub account `SuperInstance`.

---

## 0. The short answer

The defect is real, systemic, and still shipping.

| | count | denominator |
|---|---|---|
| npm packages owned by `superinstance` | **161** | 162 names found; 1 is a name collision (§6) |
| published versions downloaded & checked | **455** | 455 / 455 — **zero fetch errors** |
| versions with ≥1 missing declared entry point | **131** | 28.8% of all versions |
| packages with ≥1 affected version (any version) | **45** | 45 / 161 = 28.0% |
| — of which **Node packages where `require()` provably throws** | **12** | 12 / 161 = **7.5%** (10 hard library failures + 2 dead-`main` CLIs whose `bin` works) |
| — of which `require()` OK but `types` unresolvable (TS7016) | **18** | 11.2% |
| — **excluded**: not Node packages at all | **14** | Python/Worker/Rust/.NET (§5) |
| GitHub repos covered | **5,293** | **5,293 / 5,293 = 100%** |
| repos with a root `package.json` | **551** | 10.4% of the account |
| repos with predicted declared-vs-emitted mismatch | **16** | of 138 tsconfig-backed libraries |

**The headline number: 12 published packages where `npm install` succeeds and
`require()` throws.** I ran all 13 candidates; 12 failed, 1 (`gitpp-gate`)
passed on the bare import and failed only on a subpath. Of the 12, ten are
unambiguously broken libraries; two (`quilt-canon-cli`, `starter-shell`) are CLIs
whose declared `bin` works fine and whose `main` is merely a dead declaration.

---

## 1. Method

### Registry side (the cheap side, and the side that matters)

npm's search API has a **coverage gap that would have cost me 19 packages**, so I
did not trust it alone:

| query | total returned |
|---|---|
| `text=@superinstance` | 143 |
| `text=scope:superinstance` | **0** ← useless, returns nothing |
| `text=maintainer:superinstance` | **161** |
| `text=author:superinstance` | 161 |
| **union of all formulations** | **162** |

`motifcode` and `openroom` — both of your confirmed instances — **do not appear in
the 143**. A single-formulation sweep would have missed your own examples. The
union is the denominator: **162**, of which **161** carry `superinstance` in
`maintainers` on every version.

For each of the 455 versions I:
1. fetched the **full** manifest (cache-busted `?cb=<random>`),
2. downloaded `dist.tarball`,
3. listed every file in the tarball,
4. resolved **every** declared `main` / `module` / `types` / `typings` / `exports`
   target / `bin` against that file list — including the implicit
   `main → ./index.js` that Node falls back to when neither `main` nor `exports` exists.

**One methodological note, because it nearly cost me the whole census:** the
abbreviated manifest (`Accept: application/vnd.npm.install-v1+json`) reported
`main: null` for `@superinstance/quilt-nomad@0.1.0`, contradicting the known
defect. The full manifest reported `"main": "./dist/index.js"`. **The abbreviated
packument dropped the field.** I used the full packument for the census and
downloaded the tarball as ground truth, which is the only representation a
consumer's installer actually acts on.

### Repository side

GitHub code search does not index this account (a 0-hit search is not evidence),
so it was not used. Instead:
- REST `users/SuperInstance/repos`, paginated → **5,293** repos (one page needed a
  Python parser: `jq` dies on invalid UTF-8 in a repo description at line 11939).
- REST **core** quota was exhausted by the repo listing, so the file fetch went
  entirely through **GraphQL**, batched 60 repos/query, using `HEAD:` never
  `main:`. Final GraphQL cost for 5,293 repos + 551 tsconfigs: **~40 points of 5,000.**
- The `master`-vs-`main` warning is real: in the 13 affected repos I sampled,
  **3 defaulted to `master`**, and `palaver-math` — my demonstrated instance —
  is one of them.

---

## 2. THE CENSUS — Tier 1: `require()` provably throws (12 packages, all verified by execution)

All rows below were verified by unpacking the **real published tarball** and
running `node -e 'require(pkg)'`. These are not greps.

| # | package@version | declared entry point | what the tarball actually contains | verdict (observed) | one-line fix |
|---|---|---|---|---|---|
| 1 | `adinkra-math@1.0.2` | `main: dist/index.js`<br>`types: dist/index.d.ts` | `dist/src/index.js`, `dist/src/index.d.ts` | `MODULE_NOT_FOUND` | `"main": "dist/src/index.js"` |
| 2 | `kintsugi-math@1.0.2` | `main: dist/index.js`<br>`types: dist/index.d.ts` | `dist/src/index.js`, `dist/src/index.d.ts` | `MODULE_NOT_FOUND` | `"main": "dist/src/index.js"` |
| 3 | `palaver-math@1.0.2` | `main: dist/index.js`<br>`types: dist/index.d.ts` | `dist/src/index.js`, `dist/src/index.d.ts` | `MODULE_NOT_FOUND` | `"rootDir": "./src"` in tsconfig |
| 4 | `quipu-math@1.0.2` | `main: dist/index.js`<br>`types: dist/index.d.ts` | **only** `src/index.ts` — no `dist/` at all | `MODULE_NOT_FOUND` | `"files": ["dist",...]`; build must run before publish |
| 5 | `@superinstance/polyvocoder@0.1.2` | `main: dist/index.js`<br>`types: dist/index.d.ts` | 11 files, **0 `.js`** — `dist/` absent | `MODULE_NOT_FOUND` | ship `dist` in `files` |
| 6 | `@superinstance/equipment-consensus-engine@1.0.1` | `main: dist/index.js`<br>`types: dist/index.d.ts` | 2 files, **0 `.js`** | `MODULE_NOT_FOUND` | ship `dist` in `files` |
| 7 | `@superinstance/equipment-swarm-coordinator@1.0.1` | `main: dist/index.js`<br>`types: dist/index.d.ts` | 10 files, **0 `.js`** | `MODULE_NOT_FOUND` | ship `dist` in `files` |
| 8 | `superinstance-mcp@0.1.2` | `main` **and** `bin` → `dist/index.js` | ships `src/index.ts`, README, docs — **`dist/` declared in `files` but absent** | `MODULE_NOT_FOUND` | build in `prepublishOnly`; keep `dist` |
| 9 | `@superinstance/polln@0.1.1` | 11 `exports` subpaths (`dist/core`, `dist/api`, `dist/sdk`, `dist/spreadsheet`, …) | 3,963 files, 991 `.js`, **but not under `dist/spreadsheet/`** | `ERR_PACKAGE_PATH_NOT_EXPORTED`<br>`No "exports" main defined` | point exports at paths the build emits |
| 10 | `@superinstance/quilt-canon-cli@0.1.1` | `main: index.js` | ships `bin/canon` only; `main` is not even in `files` | `MODULE_NOT_FOUND` | drop `main` (it is a CLI) or ship `index.js` |
| 11 | `@superinstance/quilt-tools@0.1.1` | **no `main` at all** | 125 files incl. `vendor/quilt-core/dist/…`; no root `index.js` | `MODULE_NOT_FOUND` | add `"main"` or `"files"` |
| 12 | `@superinstance/starter-shell@0.1.3` | `main: index.js`<br>`bin: bin/starter-shell.js` | ships `bin/starter-shell.js`; **no `index.js`** | `MODULE_NOT_FOUND` (but `bin` runs, exit 0) | delete the dead `main` field |

**Partial (main works, subpath does not) — 1 package:**

| package | detail | verdict | fix |
|---|---|---|---|
| `gitpp-gate@0.1.1` | `exports: {".":"./src/gate.mjs", "./jev":"./src/jev.mjs", "./triggers":"./src/triggers.mjs"}`; ships `gate.mjs`, `jev.mjs` — **not** `triggers.mjs` | `import 'gitpp-gate'` **OK**; `import 'gitpp-gate/triggers'` → `ERR_MODULE_NOT_FOUND` | ship `src/triggers.mjs` or drop the export |

Raw observed output — 13 tested, 12 FAIL, 1 OK:

```
FAIL  adinkra-math                       MODULE_NOT_FOUND  Cannot find module '.../adinkra-math/dist/index.js'
FAIL  kintsugi-math                      MODULE_NOT_FOUND  Cannot find module '.../kintsugi-math/dist/index.js'
FAIL  palaver-math                       MODULE_NOT_FOUND  Cannot find module '.../palaver-math/dist/index.js'
FAIL  quipu-math                         MODULE_NOT_FOUND  Cannot find module '.../quipu-math/dist/src/index.js'
FAIL  @superinstance/polln                ERR_PACKAGE_PATH_NOT_EXPORTED  No "exports" main defined
FAIL  @superinstance/polyvocoder          MODULE_NOT_FOUND  Cannot find module '.../polyvocoder/dist/index.js'
FAIL  @superinstance/quilt-canon-cli      MODULE_NOT_FOUND  Cannot find module '.../quilt-canon-cli/index.js'
FAIL  @superinstance/starter-shell        MODULE_NOT_FOUND  (bin works — see §5, downgraded)
FAIL  @superinstance/quilt-tools          MODULE_NOT_FOUND
FAIL  superinstance-mcp                   MODULE_NOT_FOUND  Cannot find module '.../dist/index.js'
FAIL  @superinstance/equipment-consensus-engine    MODULE_NOT_FOUND
FAIL  @superinstance/equipment-swarm-coordinator   MODULE_NOT_FOUND
```

---

## 3. THE CENSUS — Tier 2: `require()` works, `types` does not (18 packages)

This is a **different and much less severe defect**, and I am reporting it
separately rather than padding the count. All 18 ship a working `index.js` and
declare `types: "index.d.ts"` (or `src/index.d.ts`) which **is never shipped**
(0 `.d.ts` in the tarball). `require()` succeeds; `tsc` reports **TS7016**
"Could not find a declaration file".

Verified by execution:

```
require() OK   @superinstance/substrate-membership -> membershipIn
require() OK   @superinstance/opcode-canon        -> BASE_OPCODES,PROPOSED_OPCODES,ALL_OPCODES
   declared types: "index.d.ts"   .d.ts shipped: 0   files: index.js package.json test.js
```

Affected (latest version broken): `@superinstance/substrate-bundle`,
`substrate-canary-pin`, `substrate-contest`, `substrate-delegate`,
`substrate-foundation`, `substrate-membership`, `substrate-merger`,
`substrate-revoke`, `substrate-traverse`, `substrate-withdraw`,
`substrate-witness-log`, `@superinstance/three-forms-of-evidence`,
`three-forms-of-forgetting`, `witness-is-prediction`, `@superinstance/opcode-canon`,
`@superinstance/observation-primitive`, `@superinstance/device-router`, `cocapn`.

Note the signature: 3 files each — `index.js`, `test.js`, `package.json` — plus a
`test.js` shipped at the package root. **The test ships; the declaration does not.**
Fix: emit declarations (`"declaration": true`) or delete the `types` field.

---

## 4. Ranked by consequence

### Tier A — published, live, lying to a consumer right now (12 + 1 partial)
`adinkra-math`, `kintsugi-math`, `palaver-math`, `quipu-math`,
`@superinstance/polyvocoder`, `@superinstance/equipment-consensus-engine`,
`@superinstance/equipment-swarm-coordinator`, `superinstance-mcp`,
`@superinstance/polln`, `@superinstance/quilt-canon-cli`, `@superinstance/quilt-tools`

Every one of these installs clean and fails on first use. (The list below is 12:
`quilt-canon-cli` and `starter-shell` are the two whose `bin` still works.)

### Tier B — published, degraded for TypeScript consumers only (18)
The substrate-* family and friends. `require()` fine, `import` in TS fails.

### Tier C — partial / narrow (2)
`gitpp-gate` (one broken subpath export); `@superinstance/polln`'s remaining
non-root subpaths.

### Tier D — sketchbook repos, never shipped (10 of the 16 predicted mismatches)
`claw`, `constraint-flow`, `constraint-ranch`, `fabric-mcp`, `grand-pattern-ts`,
`heddle`, `lumina-lang`, `quilt-k3s`, `quilt-vm-typescript`, `room-render`.
Not published to npm. Same latent defect, no consumer yet — these are the ones
worth fixing **before** the first publish.

### Your already-confirmed instances
- `@superinstance/quilt-nomad` — **published artifact is CORRECT.** `main` and
  `types` both resolve. The fix shipped. Not a live defect. (Its repo, however,
  still shows the *pre-fix* config shape in some paths — see §7.)
- `motifcode@0.4.3` — **confirmed still broken.** `main: "./src/index.ts"`, ships
  `dist/motif.js`. `MODULE_NOT_FOUND`. Not in the SuperInstance scope.
- `openroom@0.0.21` — see §6: the package is **fine**.

---

## 5. FALSE POSITIVES — what I removed, and why

A wrong hit is worse than a missed one. **14 packages were dropped**, and 2 more
downgraded, after inspection.

**Not Node packages — a Node consumer never `require()`s these (14):**

| package | why it is not a defect |
|---|---|
| `@superinstance/agent-field`, `collective-ai`, `commit-predictor`, `swarm-rooms` | ship `pyproject.toml` + a `snake_case/` package dir → **Python** |
| `mud-mcp`, `smartcrdt` | ship `pyproject.toml` → **Python** |
| `@superinstance/cocapn-vessel` | ships `wrangler.toml` → **Cloudflare Worker** |
| `edge-conservation-worker`, `fleet-edge-worker`, `fleet-vector-api`, `superinstance-vectorize`, `ternary-experiment-workers` | `wrangler.toml` (+ `Cargo.toml`) → **Worker/Rust** |
| `quilt-canary` | `Cargo.toml` + `Program.csproj` → **Rust/.NET** |

**Downgraded / not defects:**

- **`openroom@0.0.21`** — declares **no `main`** and ships `dist/cli.js`. It is a
  **CLI**. `node dist/cli.js --help` exits **0** and renders its banner correctly.
  `require('openroom')` fails, but nothing was ever meant to `require` it.
  **The package is fine; the premise that it should be importable is wrong.**
- **`@superinstance/starter-shell@0.1.3`** — `main: index.js` is missing, but the
  `bin` works (prints install instructions, exit 0). A vestigial `main`, not a
  broken consumer path. *(I still list it in Tier A because the dangling `main`
  is a genuine false declaration.)*
- **`@superinstance/quilt-canon-cli`** — same shape: `bin/canon` ships and works;
  `main: index.js` is vestigial and not even listed in `files`.
- **Cloudflare Worker `main: "src/worker.ts"` entries** (`fleet-metrics-cron`,
  `gardenlog-ai`, `travelog-ai`, `personlog-ai`, `cooklog-ai`, `dmlog-ai-1`, …) —
  declaring a `.ts` source as `main` is **normal** for packages run under
  wrangler/tsx/Deno. These are not the defect class. Excluded from the count.

**My own tooling bugs I caught and corrected** (both would have produced wrong numbers):
1. A JSONC comment-stripper using `re.S` made `//.*` run to end-of-string,
   destroying any `package.json` containing `://` — i.e. every `repository` URL.
   This silently dropped ~50 repos and hid `palaver-math`. Fixed with a proper
   character-scanner (`jsonc.py`); parse rate went 501 → 546.
2. The build-output predictor initially dropped the file extension
   (`dist/index` instead of `dist/index.js`), producing an absurd 138/138
   "mismatch". Fixed; the real figure is 54 raw / 16 high-confidence.

I also nearly published a **contaminated result**: the math packages first tested
as `OK` because I had copied already-patched copies out of an earlier test
directory. Re-fetched clean and re-run.

---

## 6. NAME COLLISIONS — where the name is wrong, not the package

1. **`superinstance` (unscoped) is NOT this account's package.**
   It is `superinstance@0.2.0`, published **2017-12-01** by `yutin1987`,
   described *"allow creation of separate instances of superagent"*,
   repo `github.com/f2etw/superinstance`. Unrelated. **Excluded from the
   denominator** (162 → 161). 161 of 162 names carry `superinstance` as maintainer;
   this one does not.

2. **`SuperInstance/adinkra-math` (repo) is NOT the source of `adinkra-math` (npm).**
   The npm manifest's `repository` points at `SuperInstance/adinkra-math`, but that
   repo is a **C library** (`gcc -std=c99`, `Makefile`, `include/`, builds
   `libadinkra.a`) and has **no `package.json` at all**. The real source is
   **`adinkra-math-npm`**, which does carry the defect.
   Same pattern for `kintsugi-math` → `kintsugi-math-npm`, `quipu-math` → `quipu-math-npm`.
   **Trusting `repository` fields here would have produced three confident,
   entirely wrong joins.**

3. **`SuperInstance/griot-math-v2` is a repo-side false positive.** The repo
   predicts a mismatch (`dist/index.js` vs `dist/src/index.js`) but the
   **published tarball is correct** — the repo has drifted from what was
   published. Predicted ≠ broken.

---

## 7. Repository-side census (100% coverage)

Of **5,293 / 5,293** repos, **551** have a root `package.json`; **546** parse;
**138** declare a `main` backed by a `tsconfig.json`. Predicting where `tsc`
actually emits (respecting explicit `rootDir`, and TypeScript's *inferred*
common-root when `rootDir` is absent):

- **84 agree** — declared `main` is where the build puts the file.
- **54 raw mismatches**, of which many are the Worker/`.ts`-`main` class (§5).
- **16 high-confidence rootDir-class defects** (declared `dist/X`, build emits `dist/Y`):

| repo | declared | `tsc` emits | published? |
|---|---|---|---|
| `adinkra-math-npm` | `dist/index.js` | `dist/src/index.js` | **yes** (`adinkra-math`) |
| `kintsugi-math-npm` | `dist/index.js` | `dist/src/index.js` | **yes** (`kintsugi-math`) |
| `quipu-math-npm` | `dist/index.js` | `dist/src/index.js` | **yes** (`quipu-math`) |
| `palaver-math` | `dist/index.js` | `dist/src/index.js` | **yes** |
| `griot-math-v2` | `dist/index.js` | `dist/src/index.js` | yes — but tarball is correct (drift, §6.3) |
| `polln` | `dist/core/index.js` | `dist/index.js` | **yes** |
| `grand-pattern-ts` | `dist/index.js` | `dist/src/index.js` | no |
| `room-render` | `dist/index.js` | `dist/src/index.js` | no |
| `claw` | `dist/index.js` | `dist/extensions/index.js` | no |
| `constraint-flow` | `dist/index.js` | `dist/connectors/index.js` | no |
| `constraint-ranch` | `dist/index.js` | `dist/docs/index.js` | no |
| `fabric-mcp` | `dist/index.js` | `dist/accessibility/index.js` | no |
| `lumina-lang` | `dist/index.cjs` | `dist/cli/index.cjs` | no |
| `quilt-vm-typescript` | `dist/quilt_vm.js` | `dist/src/quilt_vm.js` | no |
| `quilt-k3s` | `dist/src/index.js` | `dist/scenarios/index.js` | no |
| `heddle` | `dist/src/index.js` | `dist/e2e/index.js` | no |

**The static predictor independently rediscovered the exact four packages the
tarball analysis flagged.** Two independent methods, one answer — that is the
cross-check that makes this a census rather than a grep.

**The `files`-field sub-class**, which the tarball test caught but static analysis
cannot: `quipu-math` declares `files: ["src", ...]` while `main` points into
`dist/`. `superinstance-mcp` declares `files: ["dist/", ...]` and ships **no
`dist/` at all** while shipping `src/index.ts`. `files` and `main` describing
different worlds is the single most under-diagnosed form of this bug.

---

## 8. THE DEMONSTRATED INSTANCE

`palaver-math` — chosen because the repo genuinely matches the package (§6.2),
it is **published**, and it is one of your already-known defect shapes.

```bash
$ git clone --depth 1 -b master https://github.com/SuperInstance/palaver-math.git
```
*(default branch is `master`, not `main`)*

### (a) The declared entry point

```json
{ "name": "palaver-math", "version": "1.0.2",
  "main": "dist/index.js",
  "types": "dist/index.d.ts",
  "scripts": { "build": "tsc", "prepublishOnly": "npm run build" } }
```

```json
// tsconfig.json  — THE CULPRIT
{ "compilerOptions": { "outDir": "./dist", "rootDir": ".", "declaration": true },
  "include": ["src/**/*.ts", "test/**/*.ts"] }
```

`rootDir` is `"."` (the repo root) and `include` spans **two** top-level
directories (`src/` and `test/`). TypeScript emits paths *relative to rootDir*,
so it preserves the `src/` segment. The entry lands at `dist/src/index.js`,
and the declared `dist/index.js` is never created.

### (b) Where the build actually put it

```bash
$ npx tsc          # the project's own build script
$ find dist -type f
dist/src/index.d.ts
dist/src/index.js
dist/test/index.d.ts
dist/test/index.js
```

### (c) The consumer failure

```bash
$ node -e 'require("palaver-math")'
FAIL MODULE_NOT_FOUND: Cannot find module
  '/tmp/consume/node_modules/palaver-math/dist/index.js'.
  Please verify that the package.json has a valid "main" entry
```

Note what this package does *not* have: there is no test that loads the built
artifact. The suite imports `../src`, so it passes while the package is unusable.

### (d) The fix

```json
// tsconfig.json
{ "compilerOptions": { ..., "rootDir": "./src" },
  "include": ["src/**/*.ts"] }
```

The non-obvious part: naively setting `rootDir: "./src"` **breaks `npm test`**,
because `test: "node --test dist/test/*.test.js"` reads from `dist/test/`. The
fix needs the test build isolated:

```bash
$ npx tsc                       # build
$ find dist -type f
dist/index.d.ts
dist/index.js

$ npx tsc -p tsconfig.test.json && node --test dist-test/test/index.test.js
# tests 43
# pass 43
# fail 0

$ node -e 'require("palaver-math")'
OK -> PalaverSession,computeCenter,consensusDistance,influenceWeightedCenter,convergenceRate
```

**43/43 tests still green, entry point live.** That is the whole fix.

The identical one-line change also repairs `adinkra-math` and `kintsugi-math`
(verified: both go from `MODULE_NOT_FOUND` to loading and exporting symbols).

---

## 9. Honest limits of this census

1. **Registry side is complete; repository side is a model, not a measurement.**
   All 455 tarballs were downloaded and measured. The repo side is a *prediction*
   from `package.json` + `tsconfig.json`. It found 16; I confirmed 1 by building
   it. The other 15 are high-confidence but **unverified by execution** — treat
   them as leads.
2. **16 predicted repos were not all built.** I proved `palaver-math` only. The
   `predict ≠ broken` case is real (`griot-math-v2` predicted broken, tarball
   fine), so the 15 could contain similar drift.
3. **`npm search` has a coverage gap.** I found 162 names by unioning five query
   formulations. A package published under an unrelated keyword and never
   associated with `superinstance` in search would be invisible to this method.
   The 161 is a **lower bound**, not a proof of completeness.
4. **`files` with negation and `.npmignore` subtleties** can change what a
   consumer actually gets versus what `tar tzf` shows at publish time. I tested
   the published artifact, which is the right target.
5. **I did not execute a TypeScript build against all 551 packages**, so Tier 2 is
   inferred from `.d.ts` absence plus two executed spot checks.
6. **GitHub code search was not used** — it does not index this account, and a
   0-hit result there would have been meaningless.

---

## 10. The pattern, stated once

There is no test anywhere in this account that answers the only question that
matters: **after `npm publish`, does `require()` of the published tarball work?**

Every one of these packages has a passing test suite. None of them test the
artifact. The cheapest possible guard — one that would have caught all 11 of the
Tier-1 defects the moment they were written:

```bash
# postpack / CI
npm pack --dry-run >/dev/null
npx --yes verify-npm-package-entry-points   # or ~20 lines of your own
```

```
$ npm pack && mkdir -p /tmp/v && tar xz -f *.tgz -C /tmp/v \
  && node -e "require('/tmp/v/package')"
```

Run that in `prepublishOnly`. It costs one subprocess. It converts an entire
defect class from "consumer finds it" to "build fails".

---

*Read-only survey. No repository was modified, no package published or unpublished,
no commit, branch, or release created. All verification was done on local copies
and downloaded tarballs.*
