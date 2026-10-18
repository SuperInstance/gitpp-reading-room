# PLAN — what is done, what is in flight, what is left, and what it depends on

*2026-10-18. The inventory and the refined plan for the substrate work.
Everything below is a receipt or a named open question. Nothing is a
recommendation to do busywork.*

---

## 0. The shape of the work

The account is a **sketchbook**, not a fleet. So the organising rule for
this plan is:

> **Inside the sketchbook, incompleteness is the medium. The defect is a
> sketch that MISREPRESENTS its state — and misrepresentation happens at
> the boundary: the npm registry, the `repository` field, the package
> description, the exported bytes, a README, a test's own output.**

That rule is what the last three weeks of work actually found, and it is
what everything below is sorted against.

---

## 1. DONE and merged, with receipts

### The substrate, measured and published

| artefact | what it says | where |
|---|---|---|
| `ATLAS-01` | the sketchbook, mapped: 5,284 repos, where the substance is, the boundary rule | `gitpp-reading-room` |
| `ATLAS-01` addendum | the quilt family (259 repos) and the graduation blocker | `gitpp-reading-room` |
| `ATLAS-02` | the gold map: 301 uncharted repos surveyed, the finished systems, the drift | `gitpp-reading-room` |
| `ATLAS-03` | **the ladder**: SuperInstance -> patchwork-quilt (production) -> purplepincher (hardened) -> si-ready (empty shelf) | `gitpp-reading-room` |
| `FINDING-24` | the npm layer: tarball-only recoveries, the publish trap, the scope collisions | `gitpp-reading-room` |
| `FINDING-25` | **two ports of one idea, no equivalence check** — `barracks` (Python repo) vs the published JS `src/index.js` | `gitpp-reading-room` |
| `FINDING-26` + 2 corrections | **three cell models, one charter**; the address is not the hash | `gitpp-reading-room` |
| `CORRECTION-canon-shape` | the service never changed shape; the client was written against a shape it never had | `gitpp-reading-room` |
| `CORRECTION-quilt-namespace` | the name "quilt" is held three times over | `gitpp-reading-room` |
| `CORRECTION-third-party-packages` | 36 published packages are other people's; `motifcode` is `taewooopark`'s | `gitpp-reading-room` |

### The corpus, on its own terms

| artefact | what it is | where |
|---|---|---|
| `609-the-slot-nothing-is-in.md` | the handoff told every agent to run a command that could not succeed | `AI-Writings` |
| `the-answers-the-log-did-not-keep.md` | a tick asks six systems one question and writes none of it down | `AI-Writings` PR #85 |
| `PRICED-STANDARD.md` | a standard is a pre-registered predicate at a rung of a ladder, applied to a witnessed answer | `AI-Writings` PR #85 |
| `algebra.md` -> `Witness` gains `result?: any` | a witness must record the outcome, not only the request | `AI-Writings` PR #84 |
| `_worker.js` -> one witness per stream | the corpus's most expensive code path becomes a teaching corpus | `AI-Writings` PR #84 |

### The fleet, closer to production

| change | what it fixes | where |
|---|---|---|
| `quilt-claw` PR #1 | **0 tests ran**; the glob took one file of 27 and the build landed a directory too deep. Now 182/182, with a CI step that **asserts the count** — a probe that deleted one test file was green on the run and killed by the count check. | merged |
| `quilt-claw` PR #3 | `workspace:*` -> `file:`, so `npm install` resolves at all | merged |
| `git.pp-ng` PR #29 | the repo had **no `.github/` at all**; 4,292 lines of tests and a differential harness had never run. The workflow asserts the measured count. | merged |
| `Motifcode4quilt` PR #1 | the fork's README said nothing about the fork, and credited upstream's benchmark to us | merged |
| six name renames | out of a third party's scope and into one we hold | 4 merged, 2 held (below) |

---

## 2. THE CRITICAL PATH — one name holds the graduation

**This is the thing to do next, and it is small.**

`@quilt/core` is the **root** every other package reaches for. It exists
in the account, in `SuperInstance/quilt/packages/core` (v0.3.0, yaml+zod,
not private, no workspace deps, builds with tsc, tests with vitest).

**It cannot be published under its own name.** The `@quilt` npm scope
belongs to `quiltdata`, who maintain `@quilt/quilt@0.4.2` under
`kevin@quilt.io`.

**The consequence is measurable.** `quilt-nomad`'s CI fails, and the
failing step is the install, not the test:

```
npm error 404  '@quilt/core@*' is not in this registry.
```

The tests themselves pass — **14/14 on `main` and 14/14 on the rename
branch.** The job is red because the *dependency* cannot be fetched.

**The fix, in order:**

1. **Publish the root as `@superinstance/quilt-core@0.3.0`** from
   `SuperInstance/quilt/packages/core`. The name is free (404).
   `@superinstance/quilt-tools` is already published there, so the family
   has a home.
2. **Point the dependents at it.** Three repos carry `workspace:*` for
   `@quilt/core`; the `file:` swap in `quilt-claw` PR #3 covers one, and
   the others need the registry spec.
3. **Then the graduation is real** and the scope question moves to
   `@patchwork-quilt/*` when the work earns it.

**Until step 1 lands, every package in the family that depends on the
root cannot install.** That is why two rename PRs are held: their CI is
red for a cause older than the rename.

### 2a. STEP 1 IS DONE, and it cost four broken versions

`@superinstance/quilt-core` and `@superinstance/quilt-sdk` are both
published. **Getting them right took four versions and three distinct
defects, two of them mine:**

| version | what was wrong |
|---|---|
| `quilt-core@0.3.0` | **no `files` field**, so npm fell back to `.gitignore` (which lists `dist/`) and the tarball shipped `src/` without the build. `main` named a file that was not in the package and `require()` threw. **The same defect I had fixed in `motifcode` an hour earlier — reproduced on the way to the fix.** Deprecated with the reason. |
| `quilt-core@0.3.1` | builds and `import()`s cleanly — 20 exports, `yaml` + `zod`, no workspace deps. |
| `quilt-sdk@0.2.0`, `0.2.1` | the compiled output did `export { default as manifestSchema } from '../../../schemas/manifest.schema.json'` — **a path three levels ABOVE the published package**, so `import()` threw. The schema had to move inside the package. Deprecated with the reason. |
| `quilt-sdk@0.2.2` | still broken: the SDK's own `dependencies` still said `@quilt/core`. |
| `quilt-sdk@0.2.3` | right deps in the tarball — **but the REGISTRY METADATA still said `@quilt/core`.** |
| `quilt-core@0.3.2`, `quilt-sdk@0.2.4` | published from a **clean tree** (no `node_modules`) with the dependency sets written out by hand. |

**THE RULE, and it is the most reusable thing in this section:**

> **npm's publish records the INSTALLED TREE's dependencies, not the ones
> in your `package.json`.** Publishing from a directory that has
> `node_modules` in it will inject whatever is installed there as a
> dependency of the published package.

That is why `quilt-core@0.3.1` carried a dependency on `@quilt/ai` that
its tarball never mentioned, and why `quilt-sdk@0.2.3` carried
`@quilt/core` after I had removed it from the file. **The packument and
the tarball disagreed, and npm installs from the packument.** A package
can therefore have a manifest that says one thing and a registry that
says another, and the failure looks exactly like a missing package.

**The cure is unglamorous: publish from a clean copy.** No
`node_modules`, dependencies written out explicitly, `npm pack`, inspect
the tarball, publish the tarball, then **re-download it and install it
from a clean cache** — because a cached packument will keep serving you
the version you broke.

**And a third trap, for the record:** `latest` on a brand-new package name
points at a `0.0.0-stage` placeholder until the real versions land, and
**a consumer who installs during that window gets the placeholder** —
which is what happened here, and it produced an install that "succeeded"
and then threw on `require`. There is a fourth: `require()` of an
ESM-only package (`"type": "module"`, `exports` with only an `import`
condition) throws `ERR_PACKAGE_PATH_NOT_EXPORTED`, and **that is correct
ESM behaviour, not a defect.** `import()` is the test.

**Still open after this:** the two held rename PRs (`quilt-nomad#5`,
`quilt-k3s#4`) need their dependency specs repointed from `@quilt/core`
to `@superinstance/quilt-core`, and the `quilt` monorepo PR (#40) needs
the same rename applied to `packages/sdk` and the schema import fixed.

### 2b. THE SCORE TABLE: THERE ISN'T ONE, AND THE CLAIM THAT THERE WAS IS FALSE

A claim in this session's own reporting — repeated to Casey — was that
`quilt-live-canon` "scores models on a 0–1 scale with declared
denominators and score-stable campaigns" and has a ledger. **It is false
on every clause**, and the scout checked it properly:

- `quilt-live-canon`'s 23 `score` hits in `worker.js` are **cosine
  similarity for search ranking** (`0.50*cos + 0.20*fProx + …`). Zero
  hits for `ledger`, `denomin`, `campaign` or `stable`. Its "ledger" is
  `CANON.md:14` -> `ledger: git-log`.
- **Account-wide, `"score table"` -> 0 results and `"planned
  denominator"` -> 0, across all 5,289 repositories.**
- **`quilt-claw/packages/eval/` does not exist on any of its 3
  branches.** The "Immutable benchmark manifests, planned denominators,
  trust…" description in this session's own notes describes a directory
  that is not in the repository.
- `cargo-line-tycoon` has no `toolkit/` and no `packages/eval/`.

**So the score table has to be BUILT, not generalised.** Which makes
`PRICED-STANDARD.md` the first measurement of its kind rather than a
generalisation, and the house's own `PRICED-TRAINING.md` ladder the
thing to build it on.

**What already exists to build it on, and it is the right shape:**

| what | where | what it is |
|---|---|---|
| **a live scorer** | `SuperInstance/gitpp-oracle` | cron `17 */6 * * *`, last run 2026-10-18T04:44, **success**, declared >=1000-record floor. Its `runOnce()` already emits a score-table row: per-port SHA, `{agree, disagree, status}`, new-vs-accepted divergences. |
| static scoreboards | `qthe/docs/CLAIM-VERDICTS.md`, `flux-tensor-midi/research/FALSIFICATION-CAMPAIGN.md` | 7 and 10 claims with denominators and receipt-chain tips |
| **pre-registration** | `qthe/experiments/pre_registration_*.json` | 14 files, 143,291 B — the tolerances fixed before the run |

**THE DEFECT THAT MATTERS MOST, and it is the fleet's signature one:**

`gitpp-oracle/.gitignore` is **one line**: `ledger.jsonl`.

Its README promises *"an append-only ledger"* and *"a week of hourly runs
you can answer **'it was green yesterday'**"* and *"differential across
**time**, not just across ports… with a long ledger, ledgers turns a
single green into a quorum."*

**Run it:**

```
$ node oracle.mjs ledger
{"runs": 0}
```

**The ledger is gitignored, the cron never commits it, the history does
not exist, and the fleet's only continuous cross-port check goes GREEN
while holding no memory of a single previous run.** Everything the README
promises about time is unreachable. The witness-log idiom exists in this
very repo — and this repo's own witness is thrown away every six hours.

**That is the whole fleet's signature defect in the one place it costs
the most, and it is a one-line fix plus a CI assertion.**

## 2c. What the fleet's own READMEs claim, measured

The scout ranked README misdescriptions by **consequence**, not by how
wrong the text is:

| repo | the claim | reality |
|---|---|---|
| **`gitpp-oracle`** | an append-only ledger; *"it was green yesterday"* | `.gitignore` is one line: `ledger.jsonl`. `node oracle.mjs ledger` -> `{"runs": 0}`. **The only live scorer has no memory, and goes green while having none.** |
| **`quilt-ai`** | `npm test` (README:217) | `"@quilt/core": "workspace:*"` in a non-monorepo; `@quilt/core` does not exist. Proved: `EUNSUPPORTEDPROTOCOL`, exit 1. **(The root is now published as `@superinstance/quilt-core`; the spec still has to be repointed.)** |
| **`quilt-evolve`** | the same, reciprocally | forms a **dependency cycle** with `quilt-ai`; neither can build or test |
| **`plato-room-deployment-approval`** | a CI badge for `ci.yml` | the repo has **no `.github/workflows/` at all**. A badge for a workflow that does not exist. |
| **`gitpp-cross-port`** | "4 ports" at lines 3, 7, 17, 96 | "5 ports" at line 31 |

**And the `plato-*` npm-shim claim did not reproduce** — 0 of 283 repos have both
`Cargo.toml` and `package.json`; `"module.exports = {}"` returns 2 hits, both in
test files. That section of the atlas is already corrected, and the
correction stands.

---

## 3. IN FLIGHT

| who | what | why it matters |
|---|---|---|
| wesley-mechanic | generalise the ledger from **documents** to **paid experts**: the observation record gains request/response/price/latency; `distinctStates` counts experts separately from documents; a sufficiency gate that **respects the measured 0.01–0.02 noise band** | this is the bridge from a paid expert to a re-checkable standard |
| snowball-scout | does a **live score table** exist anywhere? And the **READMEs that misdescribe their own repo**, ranked by consequence | a negative here is the finding: it would make `PRICED-STANDARD` the first measurement of its kind |
| kira-cartographer | `SuperInstance/intercontext-map` — the graph of cell models, witness dialects, media, orgs, scopes, with **gaps drawn as gaps** | the map that makes the drift legible |

---

## 4. OPEN — yours to decide

1. **The `@quilt` scope.** `quiltdata` owns the npm scope; the GitHub org
   `quilt` (2019) holds an unrelated Ewasm/crypto project. **Does Casey
   hold write access to either?** Every branch of the answer selects a
   different destination. (`ADR-001` states the question and the four
   outcomes.)
2. **`@quilt/pincher`.** The sketchbook copy is **three months ahead** of
   `purplepincher/pincher`. Merging is a merge question, not a rename.
3. **`Motifcode4quilt`.** A fork of `TaewoooPark/Motifcode` that depends
   on the **upstream author's** scope `@motifcode/*`. Renaming it into
   our scope would publish a fork of his package under our name while
   still importing his scope.
4. **The empty `si-ready` shelf.** The ladder's bottom rung holds one
   repo: the org profile.
5. **Which cell model is canonical**, if any. Five exist, on two
   independent axes (ID / ADDRESS), and the 4D address and the three
   projections have **no implementation at all**.

---

## 5. THE INVENTORY — what is left, by kind

### 5.1 Things that are *wrong* (and where the receipt is)

| # | thing | state |
|---|---|---|
| 1 | `@quilt/core` unpublishable under its own name | **critical path**, §2 |
| 2 | 55 packages with tarball content no repo has (9 at risk) | named in `ATLAS-01` §3a; 1 recovered |
| 3 | 3 witness dialects that disagree; the charter's carries no result | **fixed** in `AI-Writings` PR #84 |
| 4 | the 4D address and the lattice's three projections | **no implementation anywhere** |
| 5 | the charter's "11 opcodes" = two elevens, 6/5/1/0 implemented | named in `CORRECTION-cell-history` |
| 6 | the charter's "26 kinds" = 25/25/24 | same |
| 7 | `motifcode@0.4.3`'s `main` names a file the tarball does not ship | **fixed** in `Motifcode4quilt` |
| 8 | the fork's CI has never run on `main`; when it did, 3 of 5 jobs were red | **open** — see §5.4 |
| 9 | `superinstance/npm-publish-safe`, `superinstance/cell-differ` | **not created** — 6 of 10 dispatches reported success and shipped nothing |

### 5.2 Things that are *incompleteness*, and are fine as they are

- 248 packages declared publishable and not published — sketches.
- The 5 `plato-*` empty exports — **placeholders**; the real
  implementations are 13–15 KB of Rust in the same repos. The only defect
  is the *description*, which reads like a finished capability.
- 36 published packages that are other people's — vendored upstreams
  with correct provenance.
- 19 `workspace:` users outside the graduation queue.
- 8 recovered-copy backup repos — an incident record, and a good one.

### 5.3 Things that are *waiting on a decision* (§4)

### 5.4 Things that need *checking*, not fixing

- **The fork's red CI.** `Motifcode4quilt`'s `typescript (22)`, `python`
  and `linux-arm64` jobs fail on `pnpm test` and the python unit tests.
  A README change cannot cause it, no test reads the README, and `main`
  has never run CI — so it is pre-existing and undiscovered. **It cannot
  be reproduced locally**, because the repo's `workspace:*` specs make
  `npm install` fail outright. Same defect class as `quilt-claw`'s, one
  repo over.
- **The `quilt-nomad` / `quilt-k3s` rename PRs.** Held: their CI is red
  for §2's reason, which predates the rename. The tests pass locally on
  both `main` and the branch.
- **The 45 MMAP-mapped packages whose npm names are someone else's.**

### 5.5 Things that are *done and should not be reopened*

- The leaf corpus: **20,043/20,043**, five ports.
- The fold conformance: **29/29**.
- The mutation corpus: **131,499 mutants, 0 disagreements**.
- The Elixir port: 44 tests, 0 failures, 4/4 pinned leaves.
- `gitpp-sheet@0.3.0`: 76 tests, score spread, source thresholds,
  latest-reading-wins, decomposition guidance.
- `@superinstance/sdk@0.4.0`: 113 tests, 29 suites, 0 failures.
- `luciddreamer-py`: 76 tests, **13/13 mutants caught**, CI green.
- The canon clients: shape-normalised, CI-green, and honest about the
  two endpoints that never existed.

---

## 6. The refined plan — the order, and why

1. **Publish `@superinstance/quilt-core`.** It unblocks every dependent
   and it is the whole graduation. (§2)
2. **Point the dependents at it, and let the two held renames go green.**
3. **Let the three in-flight agents land** — the ledger generalisation,
   the score-table answer, the intercontext map. The first is the
   mechanism; the second may be a negative, which is also a finding; the
   third is the map the rest of this plan hangs on.
4. **Then, and only then, the naming decisions** — because a name is
   cheap to change before anything depends on it, and expensive after.
5. **Then the device question**, which is the one worth the most and the
   one with the least machinery behind it: the charter already lists
   `quilt-subleq` (1-instruction) and `quilt-rs` (`no_std`) as media, and
   `quilt-subleq` already has a `BlockRef::Port{url,sig,encoding}` whose
   fetch is **mocked**. That is the smallest honest first step: make the
   port real on a device, with a witness that carries the answer.

**And the thing worth remembering while all of this happens.** The
charter's own sentence is the whole plan:

> **The expensive part (observing) and the cheap part (recombining) come
> apart.**

A paid expert is an expensive observer. The log is what carries the
standard afterwards. A $3 microcontroller needs neither the expert nor
the network — it needs the log, and a witness that carries the answer.

*— the audit agent, 2026-10-18. Every number here is a receipt; every
open question is named as open.*
