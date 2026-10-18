# ATLAS-01 — the sketchbook, mapped

*2026-10-18. The first entry in a series that records where the work is.*

**What this is.** `SuperInstance` is a sketchbook: 5,284 repositories where
work grows, is left behind, and keeps its thought in commits, comments and
PRs. Work that becomes production-ready graduates to its own organization
later.

**So this atlas is a MAP, not a defect list.** A sketch that is
incomplete, unpublished or untested is a sketch. What the atlas records
is *where the substance is* — the implemented systems, the preserved
source, the coherent designs — so that a thread can be picked up and
stretched out later without re-deriving what exists.

**The map is the artifact. The reasoning is recorded with it.**

---

## 1. The shape of it

```
repos                                5,284
  own + live                         4,447
  forks                                821
  archived                             16
  pushed on 2026-10-18 alone            70
  pushed in the last 9 days            159
  pushed in the last 17 days           340
  default branch main               3,238  (61.3%)
  default branch master             2,020  (38.2%)
```

**Growth is the design, not a problem.** 70 new sketches a day is the
sketchbook working. A census is a snapshot of a moving target and drifts
~8–12 repos/day; the atlas is therefore keyed by *location*, so it can be
extended without being rewritten.

Two facts that are wrong in most descriptions of this account, both
measured:

- **`SuperInstance` is a `User`, not an `Organization`.**
  `GET /api/orgs/SuperInstance` → **404**;
  `GET /api/users/SuperInstance` → 200, `public_repos: 5,279`
  (five short of the paged list — the counter is a cache, the list is
  the truth). Any tool that enumerates through the org endpoint sees
  nothing, and a 404 is easy to misread as "no repos".
- **38% default to `master`.** The `gitpp-*` family is uniformly `main`,
  which is exactly why the assumption survived so long. A resolver that
  hardcodes `main` silently skips two repos in five and reports them as
  *missing* rather than *skipped*.

---

## 2. The published surface — 573 names, 208 published

```
declared package names (union of every repo's package.json
                        and npm's search index)      573
  on npm                                             208
  404 on the registry                                365
  repository field points at a GitHub repo           191
  no repository field                                 11
```

npm's search index alone found only 117 of the 573 — it is **missing
packages that are demonstrably published** (`gitpp-js`, `gitpp-cs`,
`gitpp-pl`, `gitpp-c`, `gitpp-cross-port`, `substrate-vectors`). Take the
union of the registry and the repos; never one alone.

**365 names 404 on the registry. Under the sketchbook model that is the
expected state**, not a backlog: a sketch that was never published is a
sketch. The one question npm cannot answer is whether any of them was
published once and then removed — and the only place that difference
shows is the repository.

---

## 3. Where the gold is

### 3a. Source that exists in NO repository — the preservation list

**These eight packages ship source that no repo on the account
contains.** The tarball is the only copy. If npm garbage-collects one,
the source is gone. This is the highest-value thing in the atlas,
because it is the only category where *time* is the enemy.

| package | what only the tarball has | tarball size | state |
|---|---|---|---|
| **`@superinstance/luciddreamer`** | **10 Python modules, 3,464 lines, `py.typed`, 8 design docs** | 347,624 B | **RECOVERED** → `SuperInstance/luciddreamer-py`; 3 defects fixed, 76 tests, 13/13 mutation control, CI green |
| `@superinstance/cocapn-vessel` | 9 files incl. `src/worker.ts` (32,868 B) | 481,983 B | preserved, unmapped — the repo holds a *different lineage* under the same name |
| `@superinstance/deadband` | 4 Python modules + `index.js`/`index.d.ts` | 22,627 B | preserved; the repo says `0.2.0`, the tarball is `0.3.1` — a version-drift question, not a recovery |
| `@superinstance/cocapn-plato` | `index.js`/`index.d.ts` + an example | 49,738 B | preserved; the repo has the same modules at a different path with different content |
| `@superinstance/polyvocoder` | 3 TypeScript files | 31,679 B | preserved, unmapped |
| `@superinstance/jev-diffusion` | `index.js` + `test/test.js` | 24,199 B | preserved, unmapped |
| `@superinstance/madlibs-gan` | `index.js` | 20,094 B | preserved, unmapped |
| `@superinstance/peanut-gallery` | `index.js` | 16,580 B | preserved, unmapped |

**Why they were left unmapped rather than copied in:** the repositories
already hold the same modules at a different path with different content.
Copying the tarball in would have *regressed* the repo. That is a
lineage question and it needs a decision, not a copy. **But the tarballs
themselves are worth keeping somewhere durable** — they are the only
copies.

### 3b. The `plato-*` family — five real implementations behind five empty shims

Five published packages, five specific capability descriptions, and
**one identical 21-byte body** — `module.exports = {};` — with a matching
`index.d.ts` that says `export {};`.

But the implementations are real, and they are **not in JavaScript**:

| repo | `src/lib.rs` | also holds |
|---|---|---|
| `plato-room-nav` | 13,055 B, 16 public fns | a Python package, a built wheel |
| `plato-fleet-graph` | 15,382 B | a Python package, a built wheel |
| `plato-tile-validate` | — | `Cargo.toml`, `Cargo.lock`, `src/` |
| `plato-semantic-sim` | — | Python package, wheel |
| `plato-inference-runtime` | — | Python package, wheel |

`plato-room-nav`'s own docstring: *"Room navigation and pathfinding
engine. Finds shortest routes between rooms, discovers connected rooms…"*
— **precisely the npm description.** The capability exists. The npm
package advertises it and exports nothing.

**A thread to pull:** the Rust is the real thing and nobody has read it.
The npm shims are placeholders, which is fine for a sketchbook — what is
not fine is a *description* that reads like a finished capability.

### 3c. The graduation tickets — the scopes say where the work wants to go

The package scope is the graduation ticket. **Fifteen packages name a
destination organization, and not one of them is on npm.**

| scope | packages | org status |
|---|---|---|
| `@quilt/*` | `fleet`, `pincher`, `ai`, `claw`, `elf`, `evolve`, `k3s`, `nomad`, `rag` | **`quilt` org EXISTS** (38 repos, created 2019) — but it holds an unrelated Ewasm/crypto project (`ewasm-rt`, `turbo-geth`, `sheth`, `cargo-ease`) and **none of the nine `quilt-*` repos are in it** |
| `@luciddreamer/*` | `agent-grid`, `cost-analysis`, `monitoring-dashboard` | org **404s** |
| `@cocapn/*` | `cocapn-browser-agent`, `fleet-coordinate-js`, `plato-client` | org **404s** — this is the rename the old `repository` fields were still pointing at |

**Five of the fifteen have a CI workflow that runs `npm publish`**:
`@quilt/fleet`, `@quilt/pincher`, and all three `@luciddreamer/*`.

**That is the graduation attempt, failing.** The scope names the
destination; the publish step is the attempt to leave the sketchbook; the
package never arrives. And the destination is not ready either — so
graduation is currently blocked on **naming and org access**, not on code.
**That is a decision, and it is Casey's.**

### 3d. Vendored upstreams — the exclusion list

Ten repos declare **someone else's** package name. These are copies of
other people's projects and are *not* gold, but they need to be known so
nobody mistakes them for original work:

```
@vitejs/vite-monorepo        repo vite              2,648 files
@baml/root                   repo baml             7,996 files
@studylog/monorepo           repo StudyLog        23,707 files
@deepseek-ai/dsh-root        repo deepseek-harness-quilt
@anthropic/businesslog       repo businesslog-1
@voxel51/voxelgpt            repo voxelgpt
@ws-fabric/status-indicator  repo ws-status-indicator
@promptscript/source         repo promptscript
@dioptx/mcp-atom-of-thoughts repo mcp-atom-of-thoughts
@lucineer/room-render        repo room-render
```

**And the unscoped ones are a live hazard**, because an unscoped name is
publishable by anyone. The sketchbook contains repos named `dify`,
`open-webui`, `paperclip`, `murmur`, `superpowers`, `makerlog`, `modly`,
`www`, `mycelium-ai` — and several of those names **already exist on
npm**: `open-webui@0.1.125` with 316 files of real source,
`paperclip@20.5.0` with one file, `dify@1.0.6` at 295 B.

**A name collision in the registry tells you nothing about who owns it.**
Whether those three are Casey's or the upstream author's is not
determinable from the registry, and the `repository` field does not settle
it either. None of the ten currently has a publishing CI, which is the
only reason this is quiet.

### 3e. The uncharted surface — 301 repos

`/tmp/uncharted.json`. Repos whose declared package name 404s and whose
name is not a third party's: **the least-explored part of the
sketchbook.** Being mapped now (see §5).

---

## 4. The boundary: what may not be a lie

Inside a repository, anything goes — an empty export, a half-written
module, a stub with a grand description. That is what a sketchbook is
for.

**At the boundary, where a sketch becomes a claim to the world, it has to
be true.** The boundary is the npm registry, the `repository` field, the
package description, the exported bytes, and a test's own output.

Four things in the current atlas meet the boundary and misstate it:

1. **The canon clients' self-test.** It reported *"expected at least 70
   papers, got 2"* and named the **service** as the fault. The service
   was fine; the client was counting the response's top-level keys.
   **A test's output is a claim about the world.** — **FIXED**, with an
   offline test that fails 3 of 6 checks on the pre-fix client, and CI
   that runs it.
2. **`@superinstance/canon-claim`'s own header.** It said *"a thin
   client for the Live Canon CLAIM + DRILL endpoints."* Those endpoints
   have never existed — `grep -cE "claim|drill" worker.js` is **0**.
   **FIXED**: the header now lists what is deployed, and `claim()`/
   `drill()` raise `CanonEndpointMissingError` naming the situation.
3. **The five `plato-*` descriptions.** Each promises a capability that
   lives in Rust in the same repo. The *shim* is fine; the *description*
   is not. — **OPEN**, and the fix is an honest description, not five
   implementations.
4. **A published tarball that is not the code that was written.** A
   directory publish shipped another tree's bytes once on 2026-10-18.
   **The recipe that prevents it** is now the standing rule: `npm pack` →
   inspect → `npm publish ./pkg.tgz` → re-download → diff → point
   `latest` only at a version whose tarball you have actually fetched.
   A tarball publish was measured at 4m15s *and* ~40 minutes to become
   downloadable, so "published" is not a finished state.

---

## 5. The atlas convention

The sketchbook grows ~8–12 repos a day, so the atlas cannot be a
snapshot that has to be regenerated.

- **One entry per place, not per sweep.** Keyed by location, so a new
  sweep extends it.
- **Each entry records the thought, not just the finding** — why a thing
  matters and what a thread could pull from it, so the next person does
  not have to re-derive it.
- **Incompleteness is not recorded as a defect.** It is recorded as
  *state*.
- **The boundary is where the rules bite.** Inside, anything goes.
- **Corrections are published, not silently fixed.** See
  [CORRECTION-canon-shape.md](CORRECTION-canon-shape.md), which is
  itself an atlas entry: a wrong claim about the canon service, written
  down and corrected in place, because the correction is more useful
  than the error was.

**In progress:** the 301 uncharted repos of §3e are being mapped now.

---

## 6. The measurement, for anyone checking this

- Package list: union of every repo's `package.json` (GitHub GraphQL,
  `object(expression:"HEAD:package.json")` over all 5,284 repos) and
  npm's search index. 573 names.
- Tarball scan: every published tarball downloaded and its file list
  compared against the repository's tree, by path and then by
  basename-and-size. 171 of 204 carry source.
- Census: `GET /api/users/SuperInstance/repos`, 53 pages.
- Download counts: `api.npmjs.org/downloads/point/last-month` — the five
  empty exports together take **49 downloads/month**.
