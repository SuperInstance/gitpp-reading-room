# ATLAS-03 — the ladder: where the work is going, and what the names are pointing at

*2026-10-18. The atlas mapped the sketchbook and the published surface.
It never mapped the three organizations the work graduates INTO — which
is the part that decides what every package should be called.*

---

## 1. The ladder, measured

| rung | where | repos | created | what lives there |
|---|---|---|---|---|
| **sketchbook** | `SuperInstance` (a **user** account) | 5,284 | — | everything, growing ~8–12/day |
| **production** | **`patchwork-quilt`** | 6 | 2026-10-03 | `git.pp`, `git.pp-ng`, `git.pp-grid`, `git.pp-desk`, `gitpp-beam`, `cloudflare-next-git-rd` |
| **hardened** | **`purplepincher`** | 34 | 2026-07-02 | *"Hardened output of the SuperInstance research"* — `deckboss`, `pincher`, `luciddreamer`, `constraint-theory-core`, `vessel-tuner`, `plato-semantic-search`, `exocortex-mcp-ts`, `conservation-guardian`, `git-native-agents`, `intent-directed-compilation`, and a family of real, deployed landing pages |
| **ready-to-use** | **`si-ready`** | 1 | 2026-10-03 | the org profile only — **the shelf is empty** |

**And two orgs that are NOT ours, which is the whole reason the naming
collides:**

| org | repos | created | what it is |
|---|---|---|---|
| `quilt` | 38 | 2019-04-03 | an Ewasm/crypto project — `ewasm-rt`, `turbo-geth`, `sheth`, `cargo-ease`. **It grants no npm scope.** |
| `quiltdata` | 87 | 2016-02-07 | the open-source data catalog — and it **maintains `@quilt/quilt@0.4.2`** under `kevin@quilt.io`, so **the npm `@quilt` scope is occupied** |

**Which means: nine packages are named `@quilt/*`, and `quilt` is not an
organization this fleet owns.** They are pointing at a stranger's scope.

## 2. All four of our npm scopes are free

Direct registry checks, 2026-10-18, every one **HTTP 404**:

```
@superinstance/*     @patchwork-quilt/*     @purplepincher/*     @si-ready/*
```

**We demonstrably hold `@superinstance`** — the maintainer on
`@superinstance/canon-graph`, `@superinstance/sdk`,
`@superinstance/quilt-tools` and `@superinstance/luciddreamer` is
`superinstance`, and the npm token publishes to it.

**So the collision is not a dead end. It is a name pointing at the wrong
rung of our own ladder.**

## 3. The renames, and what each one is for

**None of these names was ever published by us.** Every one is on a
package whose first publication is still ahead of it, so **renaming costs
nothing: no consumer can break.**

| now | becomes | why |
|---|---|---|
| `@quilt/ai` | `@patchwork-quilt/quilt-ai` | the scope must name the org the work is going to, and `quilt` is a stranger's |
| `@quilt/claw` | `@patchwork-quilt/quilt-claw` | likewise |
| `@quilt/evolve` | `@patchwork-quilt/quilt-evolve` | likewise |
| `@quilt/fleet` | `@patchwork-quilt/quilt-fleet` | likewise |
| `@quilt/elf` | `@patchwork-quilt/quilt-elf` | likewise |
| `@quilt/k3s` | `@patchwork-quilt/quilt-k3s` | likewise |
| `@quilt/nomad` | `@patchwork-quilt/quilt-nomad` | likewise |
| `@quilt/rag` | `@patchwork-quilt/quilt-rag` | likewise |
| `@quilt/pincher` | `@purplepincher/pincher` | **this one is different — see below** |
| `motifcode` | `@superinstance/motifcode` | `motifcode` is `taewooopark`'s; ours is `SuperInstance/Motifcode4quilt` and can never publish under that name |
| `quilt-core` | `@superinstance/quilt-core` | `quilt-core` is `niko-dellic`'s pane-layout library; ours is a `better-sqlite3` edge daemon |
| `@luciddreamer/*` | **no rename needed** | the npm scope is free (404). Only the *GitHub* org name is held — by a real **User** |
| `@cocapn/*` | **no rename needed** | the npm scope is free AND the GitHub org is gone. It can be recreated |

**And one of them needs a merge, not a rename.**

`quilt-pincher` in the sketchbook was last pushed **2026-10-15**;
`purplepincher/pincher` was last pushed **2026-07-09**. **The sketchbook
copy is three months ahead of the hardened copy of the same work.** So
`@quilt/pincher` is not a naming problem at all — it is a work-in-progress
problem, and renaming the package without merging the three months of
work would graduate a stale artifact.

The same shape shows up across the ladder, and it is worth naming because
it is the opposite of what the org names suggest:

| sketchbook copy | last | hardened copy | last | which is ahead |
|---|---|---|---|---|
| `SuperInstance/deckboss` | 2026-06-08 | `purplepincher/deckboss` | 2026-10-08 | **hardened** |
| `SuperInstance/luciddreamer` | 2026-09-24 | `purplepincher/luciddreamer` | 2026-07-09 | **sketchbook** |
| `SuperInstance/quilt-pincher` | 2026-10-15 | `purplepincher/pincher` | 2026-07-09 | **sketchbook** |
| `SuperInstance/constraint-theory-core` | 2026-09-20 | `purplepincher/constraint-theory-core` | 2026-08-30 | **sketchbook** |
| `SuperInstance/vessel-tuner` | 2026-04-13 | `purplepincher/vessel-tuner` | 2026-07-09 | **hardened** |

**Three of five sketchbook copies are AHEAD of their hardened
counterparts.** The ladder is not a one-way street, and the org a thing
lives in does not tell you which version of it is the live one.

## 4. The empty rung

**`si-ready` holds one repository: the org profile.** *"SI Ready org
profile: the curated shelf for ready-to-use app…"* — and nothing on the
shelf.

Meanwhile `purplepincher` holds 34 repos, **several of which are landing
pages** (`studylog`, `reallog`, `playerlog`, `personallog`, `makerlog`,
`dmlog`, `businesslog`, `activeledger`, `fishinglog`, `activelog` — all
*"Real, honest landing page for X.ai"*). A family of one template
instantiated ten times, and it is on the rung meant to be *hardened*.

**So the ladder's shape, measured: the sketchbook holds 5,284 repos, the
production rung holds 6, the hardened rung holds 34 (half of them landing
pages), and the ready-to-use rung holds nothing.** Whatever the ladder is
for, it is not yet carrying anything to the bottom.

That is not a criticism of the work — the sketchbook is doing its job and
growing, and `patchwork-quilt` has the real production artifacts. It is
just where the map says the empty space is.

## 5. What this changes

**For the naming question — it is settled by the ladder, not by taste.**
The scope on a package says which rung the work is going to. `@quilt`
says *quiltdata*, which is not a rung we have. `@patchwork-quilt` says
*the production rung*, and it is free. `@purplepincher` says *the
hardened rung*. `@superinstance` says *the sketchbook*, and it is the
right answer for work that has not graduated — which is what
`@superinstance/quilt-tools` already is.

**For the ADR** — [ADR-001](adr/ADR-001-scoped-or-unscoped.md) is written
against the question *"do you hold the npm `@quilt` scope?"* — and the
answer is **no, and you could not**: `quiltdata` holds it. The ADR's
Option 5 ("publish under a scope we demonstrably hold; record `@quilt/*`
as the graduation target in metadata, not in the registry") is the
right shape, and the ladder tells us which scope: **`@patchwork-quilt`**.

**For the atlas** — the sketchbook map and the published-surface map are
now joined by the ladder, and the join is where most of the confusion
lives. Every naming collision in this series traces to the same fact:
**a package name was chosen before the ladder existed, and pointed at a
scope that meant something else.**
