# The fleet's atlas: the core is 31, and the most load-bearing repo is dormant

The account has **5,278 public repositories**. Until now nobody could say which
of them were live work, which were experiments, and which were dead ends.

A researcher mapped all 5,278 by listing every repository and reading trees.
No repository was modified.

## The core is 31, and the "47" has a source

The fleet's own paper (*F99 — The Quilt Atlas: 47 Repositories, 280K Lines of
Code, 1500+ Tests*) says 47. That number is the `seed-dna` study catalog
inside `SuperInstance/quilt-atlas` — `seed-dna/seed-dna.json` — whose README
says *"47 study repos… (49 entries incl. the two context repos)"*.

**That catalog now holds 90 entries.** It has grown since the paper was
written.

Three defensible definitions of "core" exist, and they barely touch:

| definition | n |
|---|---|
| strict core (in the dependency graph **and** has CI **and** pushed < 90d) | **31** |
| declared core (the `seed-dna` catalog) | **90** |
| live edge (non-forks, last 30 days) | **782** |
| **strict ∩ declared** | **2** |

**Only 1 of the 90 catalog repos is in the dependency graph at all.**

> The Quilt is a **curated semantic** core, not an **architectural** one. It
> cannot honestly be described as interdependent services.

That is a statement about the fleet's own paper, and it is worth making
precise rather than leaving "47 Repositories" as a number nobody can check.

## The tiers — every boundary has a stated threshold

Recency cut: 90 days before 2026-10-18.

| tier | n |
|---|---|
| core | **31** |
| active | 566 |
| experimental | 693 |
| dormant | **3,147** |
| dead | **4** |
| archived | 16 |
| fork | 821 |
| | **5,278** |

**Only 4 repos are genuinely dead.** The real problem is not dead code — it
is **3,147 dormant experiments**, and **2,106 of them were last touched on
the single day 2026-07-12**. One day produced 2,106 repos. That is a
generation, not a scatter.

## The finding that would change a decision

**`ternary-types` is the single most load-bearing repo in the account, and
it has not been pushed in 97 days.**

Verified independently:

```
full_name : SuperInstance/ternary-types
pushed_at : 2026-07-12T04:51:53Z     (97 days)
archived  : False   fork: False
desc      : Types for the SuperInstance ternary {-1, 0, +1} ecosystem
```

and **23 repos** depend on it through `Cargo.toml` git dependencies, by
measured in-edge count — the next highest is `observation-primitive-rs` at 16,
then `fleet-agent-early-version` at 10, then `constraint-theory-core` at 8.

It is a *branch* dependency, not a pinned commit:

```toml
ternary-types = { git = "https://github.com/SuperInstance/ternary-types.git", features = ["std"] }
```

so it tracks `main`. That is safer than a pinned `rev` if the repo is deleted,
and *more* dangerous if it is force-pushed: a breaking change flows straight
into 23 dependents.

**And it self-describes as the hub.** Its own description is *"Types for the
SuperInstance ternary {-1, 0, +1} ecosystem"*, and it tiers as **dormant**,
because dormancy is measured by push recency and the thing that measures
importance is the dependency graph.

> **Recency and importance are different axes, and a repo can be old AND
> load-bearing.** Any tiering that uses one axis will put this in the wrong
> bucket.

The project rule is that fleet repositories are not archived or deleted, so
the right response is to **record the fact and make the blast radius
visible**, which is what this document is.

## The trap in the centrality number itself

**`serde` appears to be the #1 dependency in the account, with 757
in-edges.** It is a **fork of the upstream crate**. Resolving names to
non-forks only drops it to **0**.

> **A centrality number computed without excluding forks is wrong by
> orders of magnitude here.**

This is the same class of error as a corpus that cannot see a defect
class: the measurement is dominated by an artifact of how the fleet was
built. It is worth knowing before anyone builds a "what breaks if I
delete this" tool on this data.

## The 280K claim, checked

Measured by reading all **2,997 source blobs**:

```
520,503 src LoC   +   75,113 test LoC
4,168 test-case declarations
```

So *"1500+ tests"* holds at 90 repos. But **24.9% of non-test lines sit in
`experiments/`, `scratch/`, `results/`, `docs/`** — `quilt-gpu-lab` alone is
131,644 LoC of which only **31.3%** is product code. **Product-only:
391,109 lines.**

## What could not be determined, stated plainly

- **CI presence ≠ CI passing.** No check was run and no badge was read. A
  repo can carry four dead workflows.
- **No transitive closure.** The blast radius of `ternary-types` is **≥23**,
  possibly larger.
- **1,556 repos have no root manifest**, so coupling there (submodules,
  Makefiles, Docker `COPY`, prose) is entirely unmeasured.
- **These are git/path dependencies, not registry semver.** npm/PyPI/
  crates.io publication coupling was not examined.
- `patchwork-quilt` has **zero** manifest dependencies on SuperInstance
  repos. It is a from-scratch git server (Python + TS + Elixir ports) with
  independent infrastructure. Whether it and the `SuperInstance` account
  are "tiers" of one system is a judgement call, and it was left open.
- The researcher's own classifier was not reconciled against
  `quilt-atlas`'s name-then-description classifier — different vocabularies.

## One practical note

**`quilt-atlas` already exists and regenerates every 6 hours.** Its README
admits it probes CI only on the top-motion slice and marks the rest
`UNMEASURED`. This atlas measures CI for all 5,278, which is the main thing
it adds — and its core finding is the one neither of them had: the hub is
dormant.
