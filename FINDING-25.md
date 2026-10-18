# FINDING-25 — two ports of the same idea, with no equivalence check

*2026-10-18. The scout's preservation sweep found a class I had not looked
for, and it is better than the one I was looking for.*

---

## 1. What I was looking for, and what was actually there

The preservation question was: **which published packages carry source that
no repository contains?** The answer, computed carefully, is a short list.
The sweep's raw output was 165 of 204 — but that number is wrong in three
separate ways, and each wrongness is a measurement error worth naming:

| the raw sweep said | the truth |
|---|---|
| 165 of 204 have tarball content absent from the repo | **55** after excluding build output, and then **the same file at another path** |
| `@superinstance/polln` — 1,980 missing source files | **all 1,980 are `package/dist/`** — build output. The repo has 3,580 files including the real source |
| 22 paths "missing" | **22 are the compiled `.js` of a `.ts` that IS in the repo** (`src/index.js` published, `src/index.ts` in the tree) |

The exclusion that matters most: the npm tarball prefixes every path with
`package/`, so a regex looking for `^dist/` never matches `package/dist/`.
And a published `.js` whose `.ts` sits in the repo is not lost source — it
is a build artifact, and the source is right there.

**So the real list is 55 packages, and 45 of those are the same one file:
`src/index.js`.**

## 2. And that one file is not a copy of the repo. It is a different language.

`SuperInstance/barracks`, in full:

```
the REPO                          the PUBLISHED TARBALL
  src/cocapn_barracks/__init__.py     src/index.js
  src/cocapn_barracks/task.py
  src/cocapn_barracks/queue.py
  src/cocapn_barracks/executor.py
  src/cocapn_barracks/cli.py
  tests/
  .github/workflows/ci-python.yml
  pyproject.toml
```

The Python is *"Task execution and queue management for agent fleets"* —
`Task`, `TaskStatus`, `TaskResult`, `TaskQueue`, `Executor`.

The JavaScript is **an agent housing registry**:

```js
class Barracks {
  constructor() { this.slots = new Map(); this.maxSlots = 50; }
  enlist(agentId, config = {}) { ... this.slots.set(agentId, slot); ... }
  discharge(agentId) { ... }
  deploy(agentId, task) { ... }
  recall(agentId) { ... }
  get roster() { return [...this.slots.values()]; }
}
```

**These are not two builds of one thing. They are two different systems
that share a name.** The repository has been rewritten into a
task-queue; the published package is still the housing registry it
started as.

`@superinstance/bottle-protocol` is the same shape, and the drift is
legible:

| | repo (Python) | published (JavaScript) |
|---|---|---|
| serialise | `to_markdown()` | `toMarkdown()` |
| **parse back** | **`parse_bottle(markdown)`** | **— nothing —** |
| **hash** | **`to_hash()`** | **— nothing —** |
| mailbox | `BottleBox` with `send`/`receive`/`inbox`/`outbox`/`mark_sent` | `FleetMessage` only |
| priority | — | `p2` default |

**The Python grew a parser and a hash. The JavaScript did not.** Two
implementations of one wire format, one of which can read what the other
writes and one of which cannot, and **nothing checks that they agree** —
because there is no shared test, and no shared contract, and they are not
even in the same language.

## 3. Why this is the finding, not "lost source"

I went looking for source that might be lost. What I found is worse and
better at once:

> **A published package and a repository are two ports of the same idea.
> The repository is the living one; the published tarball is a frozen
> earlier generation. The consumer installs the frozen one, and no test,
> fixture, or contract ever compares them.**

**And that is this series' own thesis, found in the fleet instead of in the
spec.** The `git.pp` work exists because *two independently written ports
of one spec must produce byte-identical output, and the only way to know
is to check.* Here is a place where two ports of one idea exist, in two
languages, in the same organisation, with **no equivalence check at all** —
and the divergence is already visible in the API.

The `bottle-protocol` case is the sharpest instance: **the Python can
parse a Bottle back from markdown; the JavaScript cannot.** Any consumer
who mixes the two — a Node service writing a message that a Python worker
reads — is relying on a format that only one side can round-trip.

**The mechanism is the same in both:** the published artifact was made at
some earlier moment and then the repository moved on. Nothing in the
pipeline notices, because the published package and the repository are not
connected by anything. They share a name and a version number, and that is
all.

## 4. The shape of the class, counted

Of the 55 packages with a tarball file that has no counterpart anywhere in
the repository:

- **45** are a single `src/index.js` whose repository is a **Python**
  package with the same subject (barracks, bottle-protocol, the plato-\*
  family, the cocapn-\* family, court, holodeck, keeper-beacon, …).
- **9** are multi-file, and include the genuinely at-risk ones:
  `@superinstance/cocapn-vessel` (9 files: six `lib/*.ts` + `src/worker.ts`
  at 32,868 B), `@superinstance/luciddreamer` (11 Python modules —
  **recovered**), `@superinstance/deadband` (4 Python modules),
  `@superinstance/polyvocoder` (3 TS files), plus `adinkra-math`,
  `symmetry-math`, `smartcrdt`, `quipu-math` and `@superinstance/canary-hash`.
- The rest are single files in a repository that has since changed shape.

**The nine are the preservation list. The forty-five are a version-drift
list.** They need different answers, and conflating them is how a
sixty-repository incident becomes an eight-repository one.

## 5. What I did about it, and what I did not

**Not done, deliberately:** I did not copy the JavaScript into the Python
repositories. That would put a frozen earlier generation next to a living
one with no statement of which is which — and the Python repositories have
their own tests and their own CI. The right move is to *state the
relationship*, not to merge two histories silently.

**Done:** the list is recorded, counted, and classified, so the next person
does not have to re-derive that `src/index.js` in a tarball is a build
artifact, or that a package whose repository is a Python package is a
different-language lineage rather than a broken build.

**Open, and it is the interesting one:** for each of the nine at-risk
packages, decide whether the published implementation is *the* artifact and
the repository has moved on, or the repository is *the* artifact and the
publication is a fork that should be retired. That is a product decision,
and there are nine of them, and nobody should guess.

## 6. The rules this pass adds

| # | rule |
|---|---|
| 76 | **A tarball path is prefixed with `package/`.** An exclusion for `^dist/` never matches `package/dist/`. Measure build output against the repo with the prefix stripped, or you will count a compiled tree as lost source. |
| 77 | **A published `.js` whose `.ts` is in the repository is a build artifact, not lost source.** Match on the path stem, not the filename. |
| 78 | **A published package and a repository can be two ports of one idea in two languages.** Before calling anything "lost", check whether the repository implements the same subject in a different language — and if so, say that, because the answer is version drift, not recovery. |
| 79 | **Two ports with no shared contract will drift, and the drift is invisible.** The barrel-protocol Python can parse a Bottle back from markdown; the JavaScript cannot. Cross-port equivalence is not a spec concern — it is a fleet concern, and it is the same check either way. |
