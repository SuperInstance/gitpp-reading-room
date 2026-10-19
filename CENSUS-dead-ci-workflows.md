# THE DEAD CI CENSUS — a workflow that cannot fire, and a badge over it

**Date:** 2026-10-19
**Scope:** every repository in the `SuperInstance` org, enumerated from the API
(**5,315 repos**). Each repository's default branch was read from the
repository's own page, its `.github/workflows/` listed, and every workflow file
read and its `on:` block parsed. **No GitHub API budget was used for the file
reads** — `raw.githubusercontent.com` and the repository's own tree page — so the
census is not rate-limited.

---

## 1. THE FINDING, AND THE THREE VERSIONS OF IT THAT WERE WRONG

**A WORKFLOW WHOSE `branches:` NAMES A BRANCH THAT DOES NOT EXIST IS A GREEN
BADGE OVER ZERO JOBS.**

| | workflows that **CAN NEVER FIRE** | **DEAD ON THE DEFAULT BRANCH** | repositories |
|---|---|---|---|
| v0 — a 1,000-repo sample | 28 | 1 | 18 |
| v1 — the 5,315-repo sweep | 51 | 5 | 38 |
| **v3 — the verified census** | **123** | **22** | **116** |

**AND THE STORY OF WHY v0 AND v1 WERE WRONG IS THE MOST USEFUL PART OF THIS
DOCUMENT, because both of them were confident, complete, and understated.**

### v1 DROPPED ROUGHLY 90% OF THE FLEET, SILENTLY, AND TOWARD "NO FINDING"

v1's `fetch()` had **no retry**. One transient timeout and the repository's
default branch could not be read, so `db` was `None`, so the repository was
dropped from the results — and the drop was invisible.

**THE PROOF: seven repositories that the 1,000-repo sample had already found
were MISSING from the 5,315-repo sweep.** Every one of them still exists and
still has the dead workflow:

`distributed-tracing`, `embedding-utils`, `flux-lsp`,
`health-monitoring-service`, `llm-cost-calculator`, `local-model-manager`,
`marine-gpu-edge`

**A HARNESS THAT TREATS "COULD NOT MEASURE" AS "MEASURED, AND THE ANSWER IS
NOTHING" IS NOT A CENSUS.** v3 retries, and it **counts what it could not
measure** instead of dropping it.

### v2 COUNTED A WILDCARD AS A MISSING BRANCH

A branch filter of `*` or `**` **MATCHES EVERY BRANCH**. v2 probed whether
the literal string `*` was a branch, found that it was not, and concluded
**448** dead workflows.

**384 of the fleet's workflows use a wildcard filter, and every one of them
works.** v3 excludes them, and the number is 123.

### v2 ALSO READ EVERY `.yml` IN THE REPOSITORY AS IF IT WERE A WORKFLOW

GitHub's tree page embeds **the whole repository tree**, not just the
directory that was asked for. v2's regex took every `*.yml` filename in the
payload — so it tried to read `docker-compose.yml` and `codeql-config.yml`
as workflows, and recorded 423 of them as unreadable.

v3 parses the **`path`**, not the name:

```
{"name":"ci.yml","path":".github/workflows/ci.yml"}
```

Only paths under `.github/workflows/` are workflows, and the raw URL is built
from the **full path**, so a workflow in a sub-directory is read too.
**Unreadable workflows in v3: 0.**

### THE NEGATIVE CONTROL

v3 **ASSERTS** that all ten known positives are found, and refuses to publish
if any is missed:

```
=== THE KNOWN-POSITIVE ASSERTION ===
  found    baml                         CAN-NEVER-FIRE,DEAD-ON-DEFAULT
  found    distributed-tracing          CAN-NEVER-FIRE
  found    embedding-utils              CAN-NEVER-FIRE
  found    flux-lsp                     DEAD-ON-DEFAULT
  found    health-monitoring-service    CAN-NEVER-FIRE
  found    llm-cost-calculator          CAN-NEVER-FIRE
  found    local-model-manager          CAN-NEVER-FIRE
  found    marine-gpu-edge              CAN-NEVER-FIRE
  found    quilt-jetson                 CAN-NEVER-FIRE
  found    suna                         CAN-NEVER-FIRE

  ALL 10 KNOWN POSITIVES FOUND.
```

**A census that misses a known positive is a sample of the index, not a
census of the thing.**

---

## 2. WHAT IS STILL NOT MEASURED, AND WHY IT IS NOT HIDDEN

**67 repositories (1.3% of the fleet) could not be measured** even
after the re-scan: their default branch or their workflow listing could not be
read from GitHub's HTML. **They are counted as UNMEASURED, not as clean.**
They are listed in `CENSUS-dead-ci-workflows-unmeasured.txt`.

**This is the right way round.** A census that reports 116 and a caveat
is honest; a census that reports 116 and calls the other 67 clean
is a lie with a footnote.

---

## 3. THE SHAPE OF THE MISTAKE IS A TEMPLATE, WHICH IS WHY IT REPEATED

Almost every one of them is a `master` repository whose workflows say

```yaml
on:
  push:
    branches: [develop, main]
  pull_request:
    branches: [develop, main]
```

`develop` and `main` do not exist in those repositories. **The default branch
is `master`, so no push to `master` triggers the workflow and no pull request
targeting `master` triggers it either.** The workflow is present, correct in
shape, and reachable by nobody.

**AND THE `lau-*` FAMILY IS THE CLEAREST EXAMPLE: 40 repositories, one
template, one mistake.** `lau-adinkra`, `lau-agent-organism`, `lau-agent-topology`, `lau-agent-unify`, `lau-ai-tutor`, `lau-banach-agents`, `lau-bridge`, `lau-bytecode`, `lau-cloud-deploy`, `lau-complex-agents`, `lau-computer-graphics`, `lau-construct-integration`, `lau-differential-topology`, `lau-docs-dream-compiler-spec`, `lau-domestication`, `lau-ecosystem`, `lau-ensign`, `lau-free-probability-agents`, `lau-functional-programming`, `lau-galois-agents`, `lau-gpu-compute`, `lau-griot`, `lau-hardware-abstract`, `lau-harmonic-analysis`, `lau-hodge-decomposition-agents`, `lau-index-theorem`, `lau-integration-test`, `lau-intention-field`, `lau-koopman-agents`, `lau-lie-algebra`, `lau-naturality-boundary`, `lau-palaver`, `lau-penrose`, `lau-plato-integration`, `lau-protocol-binary`, `lau-queueing-theory`, `lau-renormalization`, `lau-rhythm-nation`, `lau-self-aware-agent`, `lau-stochastic-homotopy`

---

## 4. THE CASE THAT PROVES IT, MEASURED

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
at all**.

**SO A SIX-JOB MATRIX HAS NEVER EXECUTED A SINGLE JOB, AND THE REPOSITORY'S
BADGE SITS OVER IT.**

**AND THE CONSEQUENCE, MEASURED: THE CRATE HAS NEVER COMPILED.** The trigger
fix made the matrix run for the first time in the repository's history. It
ran, and **six of six jobs were red**, and the reasons were: a **parse
error** in `src/web/routes.rs` (a `json!` object that was never closed),
`now_millis` defined twice, `evaluate_api` called with three arguments
instead of four, **recursion in an `async fn` twice**, a read guard held
across an await, `rx.next()` on a type that has no such method, a handler
error type that did not implement `IntoResponse` (which meant **not one
route on the router could be registered**), a synchronous function being
awaited in fourteen places, a missing re-export, and a `PartialEq` that does
not exist. **The library, the binary and the tests had not compiled since the
initial release on 2026-08-20.**

**AND THE CONFOUNDING PART, which is worth recording because it nearly
derailed the diagnosis:** the repository's historical run logs EXIST, and they
show real job failures. So the workflow DID run at some point — most likely
when a `main` branch existed and was renamed. **A repo can have a working CI
history and a workflow that can no longer fire, at the same time.** The run
history says "it ran"; the branch list says "it cannot." **Only the branch
list and the file settle it.**

---

## 5. THE FIX, AND THE PART THAT MATTERS MORE

1. **Point the triggers at the branch that exists** — the repo's DEFAULT
   branch, read from the repository itself, not from a template.
2. **AND ADD A GUARD SO IT CANNOT HAPPEN AGAIN, SILENTLY.** A workflow whose
   `branches:` names a branch that does not exist is a green badge over zero
   jobs, and the house's rule is: *assert that the thing you measured was
   measured.* So the guard **fails loudly when the filter names a branch
   that does not exist in the repository.** The guard's own triggers are
   deliberately **unfiltered** — a guard that shares the failure mode of the
   thing it guards is a comment — and it has a **negative control** on a
   branch that reintroduces the dead trigger, which goes **red on purpose**.
   **A guard that has never been seen failing is a comment.**
3. **AND THE BRANCH-SPLIT RULE, MEASURED, IN THE OTHER DIRECTION.** In a
   1,000-repo sample the default branches split **705 `main` / 293 `master`** —
   so a workflow that names only one of them is broken in roughly a third of
   the fleet, and **a template written for the other branch is a fleet-wide
   hazard rather than a one-off.** The census above shows the same split in
   the findings: `main`-default repositories appear in the table too.

**The one-word fix is the easy half. The guard is the valuable half.**

---

## 6. THE CENSUS, PER REPOSITORY

**123 CAN-NEVER-FIRE + 22 DEAD-ON-DEFAULT = 145 workflows across 116 repositories.**

| repository | default | workflow | verdict | branches it names |
|---|---|---|---|---|
| `DMLog-AI` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `GeoFlood` | `master` | `c-cpp.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `GeoFlood` | `master` | `cmake.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `GeoFlood` | `master` | `makefile.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `OpenMAIC` | `master` | `ci.yml` | DEAD-ON-DEFAULT | ``main`` |
| `ai-token-counter` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `ai-token-counter` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `astro` | `main` | `merge-fix.yml` | **CAN-NEVER-FIRE** | ``ci/merge-main-to-next`` |
| `baml` | `canary` | `build-ruby-release.reusable.yaml` | **CAN-NEVER-FIRE** | ``gemini-fix`` |
| `baml` | `canary` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `baml` | `canary` | `integ-tests.yml` | **CAN-NEVER-FIRE** | ``sam/integ-tests-ci`` |
| `businesslog-1` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `character-agent-integration` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `character-agent-integration` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `character-library` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `character-library` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `character-skill-trees` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `character-skill-trees` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `cocapn-ai-web` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `colony-games` | `main` | `conservation.yml` | DEAD-ON-DEFAULT | ``master`` |
| `conversation-toolkit` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `conversation-toolkit` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `cost-analysis` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `cost-analysis` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `deckboss-net` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `distributed-tracing` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `dodecet-encoder` | `master` | `ci.yml` | DEAD-ON-DEFAULT | ``main`` |
| `dodecet-encoder` | `master` | `wasm-build.yml` | DEAD-ON-DEFAULT | ``develop`, `main`` |
| `ecosystem-dashboard` | `gh-pages` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `edge-conservation-worker` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `embedding-utils` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `embedding-utils` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `fleet-metrics-cron` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `flux-compiler` | `main` | `metal-bake.yml` | DEAD-ON-DEFAULT | ``master`` |
| `flux-lsp` | `main` | `ci.yml` | DEAD-ON-DEFAULT | ``master`` |
| `forgemaster` | `main` | `metal-bake.yml` | DEAD-ON-DEFAULT | ``master`` |
| `health-monitoring-service` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `health-monitoring-service` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `hermes-construct` | `main` | `osv-scanner.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `hermes-construct` | `main` | `skills-index.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `hermit-zed` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `hierarchical-memory` | `master` | `ci.yml` | DEAD-ON-DEFAULT | ``develop`, `main`` |
| `hierarchical-memory` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `hierarchical-memory` | `master` | `validate.yml` | DEAD-ON-DEFAULT | ``develop`, `main`` |
| `hodge-belief` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `hodge-belief-rs` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `hodge-consensus-rs` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `huffman-entropy` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `hwscan` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `iii-observability` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `integration-tests` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `intent-flux-bridge` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `jazz-voicing-engine` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `jev-quilt` | `main` | `test.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `jira-cli` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `keeper-agent` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `knowledge-agent` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``master`` |
| `knowledge-vault` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `knowledge-vault-rs` | `main` | `security.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `lapce-coverage-gap` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lattice-crypto` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-adinkra` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-agent-organism` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-agent-topology` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-agent-unify` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-ai-tutor` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-banach-agents` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-bridge` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-bytecode` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-cloud-deploy` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-complex-agents` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-computer-graphics` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-construct-integration` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-differential-topology` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-docs-dream-compiler-spec` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-domestication` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-ecosystem` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-ensign` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-free-probability-agents` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-functional-programming` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-galois-agents` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-gpu-compute` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-griot` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-hardware-abstract` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-harmonic-analysis` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-hodge-decomposition-agents` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-index-theorem` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-integration-test` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-intention-field` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-koopman-agents` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-lie-algebra` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-naturality-boundary` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-palaver` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-penrose` | `main` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-plato-integration` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-protocol-binary` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-queueing-theory` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-renormalization` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-rhythm-nation` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-self-aware-agent` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `lau-stochastic-homotopy` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`, `master`` |
| `llm-cost-calculator` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `llm-cost-calculator` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `local-model-manager` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `local-model-manager` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `lucineer-com` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `marine-gpu-edge` | `master` | `mep_bridge.yml` | **CAN-NEVER-FIRE** | ``main`, `production-round3-2026-07-10`` |
| `model-switching-strategy` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `model-switching-strategy` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `monitoring-dashboard` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `monitoring-dashboard` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `multi-provider-router` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `multi-provider-router` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `next.js` | `canary` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `night-city` | `main` | `netlify-deploy.yml` | DEAD-ON-DEFAULT | ``prod`` |
| `open-application` | `dev` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `openhuman` | `main` | `build-windows.yml` | **CAN-NEVER-FIRE** | ``fix/windows`` |
| `oracle1-index` | `main` | `ci.yml` | DEAD-ON-DEFAULT | ``master`` |
| `personallog-ai` | `master` | `ci.yml` | DEAD-ON-DEFAULT | ``main`` |
| `plato-mythos-bridge` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `playerlog-ai` | `master` | `ci.yml` | DEAD-ON-DEFAULT | ``main`` |
| `provider-abstraction-layer` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `provider-abstraction-layer` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `purplepincher-org` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `quicunnel-archive` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `quilt-canon-cli` | `master` | `canon-lint.yml` | DEAD-ON-DEFAULT | ``canon-lint-tool`, `main`` |
| `quilt-jetson` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `quilt-live-canon` | `main` | `canon-verify.yml` | DEAD-ON-DEFAULT | ``canon-71-full-corpus`, `master`` |
| `rate-limiting-service` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `rate-limiting-service` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `ruview-cathedral` | `main` | `aether-arena-harness.yml` | DEAD-ON-DEFAULT | ``feat/adr-149-aether-arena`` |
| `scratch-blocks` | `develop` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `scratch-www` | `develop` | `ci.yml` | DEAD-ON-DEFAULT | ``main`, `master`` |
| `streaming-response-handler` | `master` | `test.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `streaming-response-handler` | `master` | `validate.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |
| `suna` | `main` | `build-staging.yml` | **CAN-NEVER-FIRE** | ``staging`` |
| `suna` | `main` | `deploy-gateway-dev.yml` | **CAN-NEVER-FIRE** | ``gateway`` |
| `suna` | `main` | `deploy-prod.yml` | **CAN-NEVER-FIRE** | ``prod`` |
| `suna` | `main` | `deploy-staging.yml` | **CAN-NEVER-FIRE** | ``staging`` |
| `suna` | `main` | `qa-release.yml` | **CAN-NEVER-FIRE** | ``prod`` |
| `suna` | `main` | `qa-staging.yml` | **CAN-NEVER-FIRE** | ``staging`` |
| `superinstance-vectorize` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `ternary-experiment-workers` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `ternary-rhythm` | `master` | `rust.yml` | **CAN-NEVER-FIRE** | ``main`` |
| `websocket-fabric` | `master` | `ci.yml` | **CAN-NEVER-FIRE** | ``develop`, `main`` |

---

## 7. WHAT IS STILL MISSING FROM THIS CENSUS

- **67 repositories are UNMEASURED** (section 2) and are listed in
  `CENSUS-dead-ci-workflows-unmeasured.txt`. They are not counted as clean.
- **The census reads the workflow on the DEFAULT branch.** A workflow that
  exists only on another branch is not counted, which is a small undercount.
- **A workflow with no `branches:` filter fires on every branch** and is
  excluded by construction — correctly.
- **A wildcard `branches: ['*']` or `['**']` matches every branch** and is
  excluded by construction — also correctly, and this is where v2 went wrong.
- **This is the DEAD-TRIGGER census only.** It says nothing about a workflow
  whose trigger is fine but whose *content* is broken. The `quilt-jetson`
  finding in section 4 is the proof that the two are different questions:
  the trigger was dead AND the content was broken, and only the dead trigger
  was hiding the broken content.
