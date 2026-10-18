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
repos                                5,289
  own + live                         4,452
  forks                                821
  archived                             16
  pushed on 2026-10-18 alone            85
  pushed in the last 9 days            166
  pushed in the last 17 days           329
  default branch main               3,242  (61.30%)
  default branch master             2,021  (38.21%)
  on neither main nor master            26
```

> **Re-measured 2026-10-18T07:00Z** against
> `GET /api/users/SuperInstance/repos` (53 pages, 5,289 rows), replacing
> the 5,284 sweep. `forks 821`, `archived 16`, and the main/master split
> held exactly; everything else moved by +4 to +5 repos, which is the
> drift this document's §5 convention is built to absorb. The `main`,
> `master` and "neither" figures sum to 5,289 exactly (3,242 + 2,021 +
> 26), which is the invariant worth checking. The 26 third-branch repos
> are `develop`x5, `poc`x2, `gh-pages`x2, `canary`x2, `ci/add-github-actions`x2,
> and 13 singletons — so the resolver's "skips two in five" claim
> *understates* the problem: three repos in a hundred are on a branch
> that is neither.

**Growth is the design, not a problem.** 70 new sketches a day is the
sketchbook working. A census is a snapshot of a moving target and drifts
~8–12 repos/day; the atlas is therefore keyed by *location*, so it can be
extended without being rewritten.

Two facts that are wrong in most descriptions of this account, both
measured:

- **`SuperInstance` is a `User`, not an `Organization`.**
  `GET /api/orgs/SuperInstance` → **404**;
  `GET /api/users/SuperInstance` → 200, `public_repos: 5,289`
  — **now equal to the paged list (5,289); the delta is 0.**
  *(Corrected 2026-10-18. This line previously read `5,279`, "five
  short of the paged list". The *specific* claim is now false: the
  counter has caught up with the list and they agree to the repo. The
  general claim it rested on — the counter is a cache, the list is the
  truth — was right, and this is that prediction coming true. Any tool
  that enumerates through the org endpoint still sees nothing, and a
  404 is still easy to misread as "no repos".)*
  Any tool that enumerates through the org endpoint sees
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

npm's search index alone found only 117 of the 573. Take the union of
the registry and the repos; never one alone.

> **CORRECTED 2026-10-18. This section previously claimed npm's search
> index was "missing packages that are demonstrably published" and named
> six: `gitpp-js`, `gitpp-cs`, `gitpp-pl`, `gitpp-c`,
> `gitpp-cross-port`, `substrate-vectors`. Five of the six are false.**
>
> Re-measured, each name against `registry.npmjs.org` and against
> `api.github.com/repos/SuperInstance/<name>`:
>
> | name | on npm | is a repo in this account |
> |---|---|---|
> | `gitpp-js` | **404** | 200 |
> | `gitpp-cs` | **404** | 200 |
> | `gitpp-pl` | **404** | 200 |
> | `gitpp-c` | **404** | 200 |
> | `gitpp-cross-port` | **404** | 200 |
> | `substrate-vectors` | **200** | 200 |
>
> The five are **repository names read as package names**. `npm search
> gitpp` returns `gitpp-fold`, `gitpp-gate`, `gitpp-sheet`,
> `gitpp-oracle` — four real published packages, none of them the five
> above. Only `substrate-vectors` was ever on npm.
>
> **None of the six is even in the 573-name union**, so the error did not
> corrupt the count — it corrupted the sentence built on top of it.
>
> **Audit of the rest of this list, as demanded:** the other names in
> this section come from `package.json` `name` fields harvested from the
> repositories, not from a repository-name sweep, so the confusion is
> confined to this one sentence. The 573/208/365 split below was
> re-measured independently and is exact.

> **The 573 / 208 / 365 split was re-verified independently on
> 2026-10-18T07:05Z**: all 573 names re-checked against the registry,
> **208 → HTTP 200, 365 → HTTP 404, 0 other, 208 + 365 = 573 exactly.**
> This box is correct as published and did not need changing.

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
| **`@superinstance/luciddreamer`** | **10 Python modules, 3,566 lines, `py.typed`, 8 design docs** | 78,028 B | **RECOVERED** → `SuperInstance/luciddreamer-py`; 3 defects fixed, 76 tests, 13/13 mutation control, CI green |
| `@superinstance/cocapn-vessel` | 9 files incl. `src/worker.ts` (32,868 B) | 481,983 B | preserved, unmapped — the repo holds a *different lineage* under the same name |
| `@superinstance/deadband` | 4 Python modules + `index.js`/`index.d.ts` | 22,627 B | preserved; the repo says `0.2.0`, the tarball is `0.3.1` — a version-drift question, not a recovery |
| `@superinstance/cocapn-plato` | `index.js`/`index.d.ts` + an example | 49,738 B | preserved; the repo has the same modules at a different path with different content |
| `@superinstance/polyvocoder` | 3 TypeScript files | 31,679 B | preserved, unmapped |
| `@superinstance/jev-diffusion` | `index.js` + `test/test.js` | 24,199 B | preserved, unmapped |
| `@superinstance/madlibs-gan` | `index.js` | 20,094 B | preserved, unmapped |
| `@superinstance/peanut-gallery` | `index.js` | 16,580 B | preserved, unmapped |

> **Corrected 2026-10-18 (measurement, not substance).** The
> `luciddreamer` row's counts were measured against the shipped tarball
> `luciddreamer-0.1.2.tgz`: **11 `.py` files** (10 excluding
> `__init__.py`, which is how "10 modules" is counted here),
> **3,566 lines** of Python, `py.typed` at `luciddreamer/py.typed`, and
> **exactly 8** files under `docs/`. The `.tgz` is **78,028 B**; the
> unpacked tree is **300,738 B** by `du -sb`, and the sum of file sizes
> alone is **288,450 B**. The previously published "347,624 B" matches
> none of these three and has been replaced. The recovery itself — 76
> tests, 13/13 mutation control, CI green — is unaffected and was
> re-verified by running both.

**Why they were left unmapped rather than copied in:** the repositories
already hold the same modules at a different path with different content.
Copying the tarball in would have *regressed* the repo. That is a
lineage question and it needs a decision, not a copy. **But the tarballs
themselves are worth keeping somewhere durable** — they are the only
copies.

### 3b. The `plato-*` family — five PyPI packages whose descriptions advertise a Rust capability

> **CORRECTED 2026-10-18. This section was wrong about which registry.
> It said "five published packages ... one identical 21-byte body,
> `module.exports = {};`" and quoted a download count. All of that was
> an npm story about packages that are not on npm. The real finding is
> on PyPI and is better. The false text is preserved in
> [CRITIQUE-RESPONSE.md](CRITIQUE-RESPONSE.md).**

**The five packages are on PyPI, not npm.** Measured:

| package | npm | PyPI |
|---|---|---|
| `plato-room-nav` | **404** | **200** (0.1.1) |
| `plato-fleet-graph` | **404** | **200** (0.1.1) |
| `plato-tile-validate` | **404** | **200** (0.1.1) |
| `plato-semantic-sim` | **404** | **200** (0.1.1) |
| `plato-inference-runtime` | **404** | **200** (0.1.1) |

**There is no npm surface to be empty.** Measured across all five
repositories on `main`: **0 `package.json`, and 0 files with a `.js`,
`.mjs`, `.cjs`, `.jsx`, `.ts` or `.tsx` extension.** The trees are
`Cargo.toml`, `src/lib.rs`, `pyproject.toml`, `dist/*.whl` and one
CI workflow each. A "21-byte empty export" and a "downloads/month"
figure were both unmeasurable; they are deleted rather than restated.

**What is actually there, and it is not empty:**

| repo | `src/lib.rs` | `pub fn` | PyPI wheel |
|---|---|---|---|
| `plato-room-nav` | 13,055 B | 16 | `nav.py` 3,614 B + `router.py` 1,631 B |
| `plato-fleet-graph` | 15,382 B | — | Python package + wheel |
| `plato-tile-validate` | 13,806 B | — | Python package, no wheel in tree |
| `plato-semantic-sim` | 13,590 B | — | Python package |
| `plato-inference-runtime` | 13,189 B | — | Python package + wheel |

The published `__init__.py` is **244 bytes** and it genuinely
re-exports a working implementation. *(The earlier "21 bytes" was not
this file and does not exist in any of the five.)*

**The real finding — two ports of one idea, with no equivalence check.**
`plato-room-nav`'s Rust docstring reads:

> *"Room navigation and pathfinding engine. Finds shortest routes
> between rooms, discovers connected rooms, and provides spatial
> awareness for PLATO agents."*

and its PyPI description reads:

> *"MUD-style room navigation with exits, locked doors, pathfinding,
> and topic-based room routing."*

Same capability, two languages, **one repository. And they are not
connected.** Measured on `plato-room-nav/src/lib.rs`: it contains **no
`find_path`, no `RoomNavigator`**, and no PyO3 binding of any kind
(`pymodule`/`pyo3` absent). The Python side implements `RoomNavigator`
and `find_path` itself, in its own 3,614-byte `nav.py`.

So the Rust is a **parallel implementation that nothing calls**, and the
package description advertises the union of the two without saying which
one you are getting. This is the same shape as
[FINDING-25](FINDING-25.md) — two ports of the same idea with no
equivalence check — and it is a **description** defect under §4, not an
implementation defect: both implementations are real and neither is
broken.

**A thread to pull:** the Rust is 13 KB of unread BFS/Dijkstra that
nobody has called, next to a Python implementation of the same graph
problem that nobody has diffed against it.

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

> **Read this section with
> [adr/ADR-001-scoped-or-unscoped.md](adr/ADR-001-scoped-or-unscoped.md),
> which supersedes it.** Added 2026-10-18: this section was standing
> alone while its own replacement existed in another file with **no link
> between them**, which is the precise failure the §5 convention is
> written to prevent — a correction published in one place and a false
> claim left standing in another.
>
> Two measured consequences for the table above:
>
> - **Fourteen of the fifteen are publishable from their own repository;
>   the fifteenth is not.** `SuperInstance/plato-client` contains **no
>   `package.json` at all** — it is a Python package (10 blobs,
>   `dist/cocapn_plato-0.1.0-py3-none-any.whl`). The npm name
>   `@cocapn/plato-client` exists only as a **vendored** manifest at
>   `cocapn-browser-agent/vendor/plato-client/package.json`. Both counts
>   are right about different things: **15 names** appear in the account,
>   **14** can be published from the repository that owns them.
> - **The destination decision is PENDING and owned by Casey**, and the
>   interim position is that **no package is published under any
>   `@quilt/*` or `quilt-*` name until it is answered.**
>   See also [CORRECTION-quilt-namespace.md](CORRECTION-quilt-namespace.md)
>   (rules 65–69), which this section also contradicted and did not cite.

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
3. **The five `plato-*` descriptions.** Each PyPI description promises a
   capability that is implemented twice in the same repo — once in Rust,
   once in Python — with no binding between them (§3b). The *code* is
   fine; the *description* does not say which implementation you get.
   — **OPEN**, and the fix is an honest description, not five
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

**Mapped:** the 301 uncharted repos of §3e are in
[ATLAS-02.md](ATLAS-02.md).

---

## 6. The measurement, for anyone checking this

- Package list: union of every repo's `package.json` (GitHub GraphQL,
  `object(expression:"HEAD:package.json")` over all 5,284 repos) and
  npm's search index. 573 names.
- Tarball scan: every published tarball downloaded and its file list
  compared against the repository's tree, by path and then by
  basename-and-size. 171 of 204 carry source.
- Census: `GET /api/users/SuperInstance/repos`, 53 pages.
- Download counts: `api.npmjs.org/downloads/point/last-month`.
  **Removed 2026-10-18.** This line read "the five empty exports
  together take 49 downloads/month". There are no five empty exports:
  the `plato-*` packages are PyPI packages (§3b), the five repositories
  contain no `package.json` and no JavaScript, and the npm names 404.
  The figure was unmeasurable and is deleted rather than corrected.
  If a download count is wanted for this family, the correct call is
  `pypi.org` — which is a different question and has not been measured.
