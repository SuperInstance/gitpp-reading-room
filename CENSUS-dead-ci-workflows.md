# THE DEAD CI CENSUS — a workflow that cannot fire, and a badge over it

**Date:** 2026-07-19
**Scope:** every repository in the `SuperInstance` org, enumerated from the API
(**5,315 repos**), each one's default branch read from the repository page, its
`.github/workflows/` listed, and every workflow file read and its `on:` block
parsed. **No GitHub API budget was used for the file reads** — `raw.githubusercontent.com`
and the repository's own tree page, so the census is not rate-limited.

---

## 1. THE FINDING

**A WORKFLOW WHOSE `branches:` NAMES A BRANCH THAT DOES NOT EXIST IS A GREEN BADGE
OVER ZERO JOBS.**

And the fleet has a whole family of them.

| | |
|---|---|
| workflows that **CAN NEVER FIRE** | **28** |
| workflows **DEAD ON THE DEFAULT BRANCH** | **1** |
| repositories affected | **18** |
| (sample size) | **1,000 repos** — the 5,315-repo sweep is running |

**AND THE SHAPE OF THE MISTAKE IS A TEMPLATE, WHICH IS WHY IT REPEATED:**
almost every one of them is a `master` repo whose workflows say

```yaml
on:
  push:
    branches: [develop, main]
  pull_request:
    branches: [develop, main]
```

`develop` and `main` do not exist in those repositories. **The default branch is
`master`, so no push to `master` triggers the workflow and no pull request
targeting `master` triggers it either.** The workflow is present, correct in
shape, and reachable by nobody.

## 2. THE CASE THAT PROVES IT, MEASURED

`SuperInstance/quilt-jetson` has a **six-job Rust CI matrix** — `Lint & format`,
`clippy (-D warnings)`, `MSRV (1.75)`, `Test` on ubuntu/macos/windows, and
`Cross-compile (aarch64-unknown-linux-gnu)` — and:

```yaml
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]
```

**AND THE REPOSITORY HAS NO `main` BRANCH.** Its branches are `master` (the
default) and two fix branches.

**THE PROOF, and it is not an opinion.** The pull request that was merged into
`master` hours earlier — the one that fixed four real defects — has
**EXACTLY ONE CHECK-RUN: GitGuardian.** The six jobs produced **no check-runs
at all**. `master`'s own head has only GitGuardian (skipped).

**SO A SIX-JOB MATRIX HAS NEVER EXECUTED A SINGLE JOB, AND THE REPOSITORY'S
BADGE SITS OVER IT.** That is the fleet's signature defect at its purest —
*infrastructure that looks installed is not* — and it is worse than the ones
this fleet has been cataloguing, because a badge is a **claim**, and a claim
with no receipt is the exact thing this whole body of work has been about.

**AND THE CONFOUNDING PART, which is worth recording because it nearly
derailed the diagnosis:** the repository's historical run logs EXIST, and they
show real job failures. So the workflow DID run at some point — most likely
when a `main` branch existed and was renamed. **A repo can have a working CI
history and a workflow that can no longer fire, at the same time.** The run
history says "it ran"; the branch list says "it cannot." **Only the branch
list and the file settle it.**

## 3. WHY THIS IS THE PUREST FORM OF THE FLEET'S SIGNATURE DEFECT

The house has been cataloguing, all session, a family of the same shape:

- a reusable workflow in the wrong directory → **failure with zero jobs**
- a corpus path one level off → **green badge over zero cases**
- a participant whose env var was never set → **four ports reporting five
  ports' pass count**
- a workflow triggered on schedule but not on push → **a stale badge
  describing a commit that no longer existed**

**A workflow whose trigger names a branch that does not exist is the same
defect one level up: the harness is not merely mis-wired, it is unreachable,
and nothing in the system says so.** The four "pre-existing CI failures" in
`quilt-jetson` were real defects and are now fixed — **but the reason the
repo looked broken was this, and the reason it looked FIXED was that the
matrix had never been running to check it.**

## 4. THE FIX, AND THE PART THAT MATTERS MORE

1. **Point the triggers at the branch that exists** — the repo's DEFAULT
   branch, read from the API, not from a template.
2. **AND ADD A GUARD SO IT CANNOT HAPPEN AGAIN, SILENTLY.** A workflow
   whose `branches:` names a branch that does not exist is a green badge over
   zero jobs, and the house's rule is: *assert that the thing you measured was
   measured.* So the guard **fails loudly when the filter names a branch
   that does not exist in the repository.**
3. **AND THE BRANCH-SPLIT RULE, MEASURED, IN THE OTHER DIRECTION.** In a
   1,000-repo sample the default branches split **705 `main` / 293 `master`** —
   so a workflow that names only one of them is broken in roughly a third of
   the fleet, and **a template written for the other branch is a fleet-wide
   hazard rather than a one-off.**

**The one-word fix is the easy half. The guard is the valuable half** —
because a guard that has never been seen failing is a comment.

## 5. THE CENSUS, PER REPOSITORY

| repo | default | workflow | verdict | branches it names |
|---|---|---|---|---|
| `Scrapcraft` | `main` | `ci.yml` | CAN-NEVER-FIRE | `**` |
| `ai-token-counter` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `ai-token-counter` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `character-agent-integration` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `character-agent-integration` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `character-skill-trees` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `character-skill-trees` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `cocapn-ai-web` | `master` | `ci.yml` | CAN-NEVER-FIRE | `main` |
| `conservation-art` | `main` | `ci.yml` | CAN-NEVER-FIRE | `**` |
| `conversation-toolkit` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `conversation-toolkit` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `distributed-tracing` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `embedding-utils` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `embedding-utils` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `flux-lsp` | `main` | `ci.yml` | DEAD-ON-DEFAULT | `master` |
| `health-monitoring-service` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `health-monitoring-service` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `llm-cost-calculator` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `llm-cost-calculator` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `local-model-manager` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `local-model-manager` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `marine-gpu-edge` | `master` | `mep_bridge.yml` | CAN-NEVER-FIRE | `main`, `production-round3-2026-07-10` |
| `provider-abstraction-layer` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `provider-abstraction-layer` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `rate-limiting-service` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `rate-limiting-service` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `streaming-response-handler` | `master` | `test.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `streaming-response-handler` | `master` | `validate.yml` | CAN-NEVER-FIRE | `develop`, `main` |
| `websocket-fabric` | `master` | `ci.yml` | CAN-NEVER-FIRE | `develop`, `main` |

## 6. WHAT IS STILL MISSING FROM THE CENSUS

- **The 5,315-repo sweep is running.** The 1,000-repo sample is the search
  index's view, which **excludes `quilt-jetson` itself** — the case that
  proves the class. **A sample that misses the known positive is not a
  sample of the thing; it is a sample of the index.**
- **A wildcard `branches: ['*']` is a valid filter** and is excluded above.
  One repo (`spectral-music-v2`) uses it, and it is fine.
- **A workflow with no `branches:` filter fires on every branch** and is
  excluded by construction.
- **The census reads the workflow on the DEFAULT branch.** A workflow that
  exists only on another branch is not counted, which is a small
  undercount.
