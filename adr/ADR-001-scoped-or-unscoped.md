# ADR-001 — Scoped or unscoped: what the `@quilt/*` namespace graduates into

- **Status:** DECIDED — but the decision is **PENDING**, owned by Casey. See §7.
- **Date:** 2026-10-18
- **Decider:** Casey (SuperInstance)
- **Architect:** Bruno (`bruno-architect`)
- **Scope of this ADR:** the *name* under which the Quilt packages publish. Not the
  contents, not the tests, not the runtime.
- **Artifacts:** `quilt-graduation-graph.dot`, `quilt-graduation-graph.json`,
  `INTEGRATION-NOTE.md`
- **Doctrine:** *form follows force.* The shape of the namespace is being decided by
  the force of what already exists — and what already exists is three other people's
  `quilt`. This ADR exists to make that force visible before a name is spent.

---

## 0. The short version

The briefing asks whether `@quilt/*` or unscoped is the right graduation shape. Measured
against npm and GitHub, **that question cannot be answered first, because a third party
already holds `@quilt` on npm, and a different third party already holds `quilt-core`
unscoped.** Both of the scoped and unscoped homes Casey was choosing between are
occupied.

The name is not the last step of graduation. **The name is the gate, and it is currently
shut by someone else's account.**

Separately — and this is separable — **five packages are blocked by nothing but plumbing,
and two of them could ship today** if their scope question were settled. That work does not
have to wait for the `@quilt/core` decision at all.

---

## 1. Context

`SuperInstance` is Casey's sketchbook, not a production fleet. Work grows there, is left
behind as artifacts, and the thought is preserved in commits. Production-ready work
graduates to its own organization later. The package **scope is the graduation ticket**:
when a sketch's package is named `@quilt/*`, the scope says which organization it wants to
leave for.

This ADR does not attempt to fix broken packages. It attempts to describe the shape of the
path out.

### 1.1 What was measured, and how

Everything below was re-derived from the GitHub REST API and the live npm registry on
2026-10-18 rather than taken on trust. Method and per-edge results are in
`quilt-graduation-graph.json`. The four load-bearing reproductions:

```bash
# the plumbing failure, reproduced from scratch in an empty directory
mkdir /tmp/wsrepro && cd /tmp/wsrepro
printf '{"name":"repro","version":"1.0.0","dependencies":{"@quilt/core":"workspace:*"}}' > package.json
npm install
# npm error code EUNSUPPORTEDPROTOCOL
# npm error Unsupported URL Type "workspace:": workspace:*

# who holds the npm @quilt scope
curl -s https://registry.npmjs.org/@quilt%2fquilt | jq '.maintainers, .versions["0.4.2"].description'
# -> [{"name":"kevinklin","email":"kevin@quilt.io"}]

# what the unscoped quilt-core on npm actually is
curl -s https://registry.npmjs.org/quilt-core | jq -r '.versions["0.11.0"].repository.url'
# -> git+https://github.com/niko-dellic/quilt.git  (directory: packages/core)

# the second in-account claimant of @quilt/core
curl -s -H "Authorization: token $GITHUB_TOKEN" \
  https://api.github.com/repos/SuperInstance/quilt/contents/packages/core/package.json \
  | jq -r '.content' | base64 -d | jq '.name, .version'
# -> "@quilt/core"   0.3.0
```

### 1.2 The measured state

| Fact | Measured value | Source |
|---|---|---|
| `quilt-*` repos in the sketchbook | **257** | full pagination of `users/SuperInstance/repos?type=owner` (5,280 repos total) |
| archived among them | **0** | same |
| pushed in last 3 months | Aug 47 / Sep 145 / Oct 65 | same |
| of the 15 scoped packages, on npm | **0** | `registry.npmjs.org`, all HTTP 404 |
| `quilt-core` unscoped on npm | 0.11.0, 18 versions | `registry.npmjs.org/quilt-core` |
| `quilt-core-os` declared version | 0.4.0, 32 blobs, Apache-2.0 | `SuperInstance/quilt-core-os` |
| `quilt-core-os` on npm | **never** — name is taken | name collision below |
| `@quilt/core` in-account version | 0.3.0 (`SuperInstance/quilt` `packages/core`) | **corrects the briefing** |
| `@quilt/sdk` in-account version | 0.2.0 (`SuperInstance/quilt` `packages/sdk`) | **new** |
| repos with a `npm publish` workflow | **5** | `@quilt/fleet`, `@quilt/pincher`, 3 × `@luciddreamer/*` |
| `quilt-claw` | 210 KB, 27 test files, **182 `test()` calls** | shallow clone + `grep -o` |
| `quilt` monorepo | 15,272 KB, 528 blobs, 5 packages, `private: true` | `SuperInstance/quilt` |

### 1.3 Seven corrections to the briefing

These were measured, not assumed. Four of them change the decision.

**C1 — The unscoped `quilt-core` on npm is not ours. This is the important one.**
The briefing inferred that because `quilt-core` exists on npm at 0.11.0 *and* the repository
records 0.4.0, "the root of the dependency tree exists and is published, but under an
UNSCOPED name." **That inference is wrong.** The published `quilt-core` belongs to
**`niko-dellic/quilt`, `packages/core`** — a MIT-licensed browser pane-layout library,
maintained by `nikodellic@gmail.com`, with **zero dependencies**, shipping
`model.ts` / `controller.ts` / `join.ts` / `join-rectangle.ts` / `types.ts`.

`SuperInstance/quilt-core-os` is *"the TypeScript daemon that ships inside the quilt-core
snap"* — `better-sqlite3`, Apache-2.0, Jetson/Raspberry-Pi install docs, snap hooks.

These are not the same artifact. The shared `0.4.0` in a `0.1.0…0.11.0` series is a
coincidence of numbering that invites exactly this false conclusion. **`quilt-core-os` has
never been published, and never can be under that name without the owner's cooperation.**
Option 1 below is therefore not a shortcut — it is a trapdoor.

**C2 — `@quilt/core` *is* declared on the account.** The briefing states it is declared by no
repository. It is declared by `SuperInstance/quilt` at `packages/core`, version **0.3.0** —
in the monorepo `quilt`, not in any `quilt-*` repo. That monorepo declares **five** packages
(`@quilt/core` 0.3.0, `@quilt/sdk` 0.2.0, `@quilt/mcp` 0.3.0, `@quilt/tui` 0.3.0,
`@quilt/cli` 0.3.0), all `private`-adjacent, all 404 on npm. `@quilt/sdk` exists as source.

**C3 — There are two different products in this account claiming the name `core`.**
`@quilt/core` 0.3.0 is a yaml+zod spreadsheet-cell model. `quilt-core` 0.4.0 is an IoT edge
daemon. They are not variants of one another. Unifying them is a **product** decision
masquerading as a packaging decision, and it has not been made.

**C4 — The npm `@quilt` scope is occupied by a third party.** `@quilt/quilt@0.4.2` is
published; its sole maintainer is `kevin@quilt.io`; the matching GitHub org is
**`quiltdata`** (created 2016-02-07, 87 repos — the open-source data catalog:
`quilt`, `open-data-registry`, `jupyterlab-quilt`, `notebooks`). Both Options 2 and 3
require write access to a scope this organization appears to control.
**UNVERIFIED whether Casey holds any right to it** — see §6, V1. This must be answered
before any publish attempt, not discovered by one.

**C5 — `luciddreamer` is not a 404, but the org still doesn't exist.**
`GET /orgs/luciddreamer` → 404. `GET /users/luciddreamer` → **200**: a GitHub **User**
(`id 131851688`, created 2023-04-26, 0 public repos). So a real account holds the name, and
it is a person, not an org. That is a different problem from an unclaimed namespace: you
cannot create the org later without resolving the user collision.

**C6 — `@cocapn/plato-client` is not a fifteenth package.** `SuperInstance/plato-client`
contains **no `package.json` at all**. It is a Python package —
`src/cocapn/__pycache__/client.cpython-310.pyc`, `dist/cocapn_plato-0.1.0-py3-none-any.whl`.
The npm name exists only as a vendored JS shim at
`cocapn-browser-agent/vendor/plato-client/package.json`. There are **14** publishable
manifests, not 15.

**C7 — the shared-core finding holds, with different numbers.** Seven repos
(`quilt-arcade`, `quilt-arena`, `quilt-quant`, `quilt-dba`, `quilt-mesh`, `quilt-learn`,
`loom-core`) do share a byte-identical core: **`engine/engine.js`, 28,145 bytes, identical
in all seven**, plus `engine/engine.js.map` at 17,339. The briefed pair
(97,486-byte JS + 71,216-byte TS) **does not reproduce at HEAD** of any of the seven
(current `HEAD` = `main`, single branch each) — **UNVERIFIED**, possibly an older commit.
The *substance* stands: the `quilt-` prefix is a naming family, not 39 systems. Family size
is a count, not a substance measure.

### 1.4 Three distinct "quilt"es exist

This is the diagram. Everything else in this ADR follows from it.

```
                    the name "quilt" is held three times over
                    ──────────────────────────────────────────

   quilt (GitHub org)          quiltdata (GitHub org)        SuperInstance (user)
   created 2019-04-03          created 2016-02-07            5,280 repos
   38 repos · Ewasm/crypto     87 repos · data catalog       257 × quilt-*
            │                          │                            │
            │ grants no               │ maintains                  │ 257 repos
            │ npm scope               │ @quilt/quilt@0.4.2          │ name no
            │                          │ (kevin@quilt.io)           │ one owns
            ▼                          ▼                            ▼

   npm:  @quilt/*  ──────────────► OCCUPIED (quiltdata)     no @quilt scope
   npm:  quilt-core ────────────► OCCUPIED (niko-dellic)   no quilt-core
   npm:  quilt      ────────────► (SuperInstance/quilt)     ——
```

---

## 2. Forces

Ranked by how much force they actually exert on this decision. Not by how much they feel
like they should.

| # | Force | Magnitude | Direction |
|---|---|---|---|
| F1 | **The `@quilt` npm scope is held by a third party** (C4) | **Blocking** | toward renaming |
| F2 | **`quilt-core` unscoped is held by a third party** (C1) | **Blocking** | away from unscoped `core` |
| F3 | **Two in-account products claim `core`** (C3) | High | toward deciding, not packaging |
| F4 | The three blocked packages are blocked by **`workspace:*`**, a protocol bug — not by naming | High | toward fixing plumbing *first* |
| F5 | `workspace:` is a monorepo-internal protocol; `file:` is the publish-time rewrite mechanism, and `@quilt/tui` already uses it successfully (C2) | High | toward `file:` |
| F6 | Two of the five failing publish CI jobs (`@quilt/fleet`, `@quilt/pincher`) have **no internal dependencies at all** | High | toward unblocking now, separately |
| F7 | The cycle `@quilt/ai ⇄ @quilt/evolve` is **not** what breaks publish | Medium | toward deferring the cycle |
| F8 | npm unpublishes are slow, irreversible after 72h, and names are permanent | Medium | toward deciding once |
| F9 | The `quilt` GitHub org is unrelated Ewasm work; using it as a destination asserts ownership that isn't there (C4) | Medium | away from Option 3 as written |
| F10 | The `quilt-*` family is 257 repos but shares cores and has thin tests (C7) | Low | toward scope ≠ substance |

**The forces do not agree, and that is the finding.** F1–F3 are about *ownership and
collision*; F4–F6 are about *protocol*. They are different problems that happen to share a
word. Conflating them is what produced 15 packages that cannot publish and a `@quilt/core`
that is 404 — the name problem has been masking a plumbing problem.

---

## 3. Options

Four options, as briefed, plus the two the measurement forced into existence.

For each: **what must be true**, **what it costs**, **what it breaks**, **what it makes
possible that is currently impossible**.

---

### Option 1 — Keep `quilt-core` unscoped; change the three dependents to depend on `quilt-core`

**What must be true:** that the npm package `quilt-core` is the thing you want. It is not.
It is `niko-dellic`'s pane-layout library (C1).

- **Costs:** one line in three manifests.
- **Breaks:** `@quilt/ai`, `@quilt/evolve`, `@quilt/claw` would pull a stranger's
  MIT pane-layout code into the Quilt runtime. Because `@quilt/ai` and `@quilt/evolve` are
  each other's dependencies, this is not one bad edge — it is the root of the tree silently
  becoming someone else's library. The pane library also runs npm lifecycle scripts on
  install.
- **Makes possible:** nothing that is not already possible.
- **Verdict: REJECT — this is the trapdoor.** It is the cheapest line to write and the only
  option that hands a third party code execution inside the Quilt runtime. It looks correct
  because "0.4.0 is in the 0.1.0…0.11.0 series" and the name matches exactly.

> If the pane-layout library genuinely *were* the root, this would be free. Proving it is not
> takes one command (§6, V2). Do not skip V2.

---

### Option 2 — Publish `@quilt/core` as a NEW scope from the same source; keep `quilt-core` as a deprecated alias

**What must be true:**
1. Casey can write to the npm `@quilt` scope. **Evidence says a third party holds it (C4).**
2. Nothing in the account already means `@quilt/core` by a different definition. **False —
   `SuperInstance/quilt` `packages/core` 0.3.0 already owns that name in-repo (C2, C3).**

- **Costs:** renaming the name in `quilt-core-os/package.json`; resolving the collision with
  `SuperInstance/quilt`'s `packages/core` (0.3.0) which is a *different product*; a decision
  about whether `@quilt/core` 0.3.0 or the edge daemon inherits the name.
- **Breaks:** publishing the edge daemon as `@quilt/core` silently repoints an in-repo name
  to a different product. `@quilt/core` 0.3.0 has a peer dep on `@quilt/ai` and deps on
  yaml/zod; consumers of that meaning would receive an IoT daemon. The version line would
  also collide in appearance — 0.3.0 vs 0.4.0 are adjacent, which makes the mistake easy to
  miss in a diff.
- **Makes possible:** the scoped name the three dependents already want, without moving a repo.
- **Verdict: VIABLE ONLY IF V1 and V3 both come back yes, and only after C3 is decided as a
  product question.** As briefed it inherits a scope it probably cannot write to.

---

### Option 3 — Move `quilt-core-os` into the `quilt` org; publish `@quilt/core` from there; let the account hold the sketch

This is the graduation story the scope is meant to express. It is also the option that
depends most heavily on facts that are not ours.

- **What must be true:**
  1. The `quilt` GitHub org (created 2019-04-03, 38 Ewasm/crypto repos: `ewasm-rt`,
     `turbo-geth`, `sheth`, `pm`, `kernel`, `lighthouse`, `trin`, `openeethereum`) is
     Casey's to write to. **No evidence it is.** It is unrelated work by a different owner.
  2. The npm `@quilt` scope is free or ours. **Evidence says Quilt Data holds it (C4).**
  3. Writing `quilt-core-os` into an org full of Ewasm crypto repos is a coherent act.
     **That is a governance claim, not a technical one.**
- **Costs:** repo transfer, redirect setup, npm scope acquisition (possibly requiring a
  relationship with a third party), and a naming decision on `@quilt/core` 0.3.0 (C3).
- **Breaks:** if the org is not Casey's, this is publishing into someone else's namespace.
  A `quilt-core` snap and a `turbo-geth` fork under one roof is also a *meaning* collision
  that future readers — and future Casey — will have to unpick.
- **Makes possible:** the clean graduation the scope implies; the sketchbook genuinely stops
  holding production code.
- **Verdict: the right destination, but not yet a legitimate move.** Blocked on V1 (npm scope)
  and V4 (org ownership). It is a decision about ownership, and it is Casey's to make.

---

### Option 4 — Drop the scope; publish all ten as unscoped

- **What must be true:** that unscoped names are free. **They are not** — `quilt-core` is
  taken (C1), and the naming family `quilt-*` collides with a third party's monorepo on npm
  (`quilt-vanilla`, `quilt-react`, `quilt-sdk`, `quilt-nightly` all exist unscoped).
- **Costs:** lowest ceremony — no scope to own.
- **Breaks:** every future `quilt-<thing>` is a global name grab in a namespace where a
  third party is actively publishing (`quilt-vanilla`, `quilt-react` at 0.11.0 from the same
  pane-layout repo). The first genuine collision — and `quilt-core` proves one already
  happened — costs a rename in every dependent, forever. Unscoped names are also
  unqualified by default, so the graduation ticket disappears entirely: `@quilt/core` says
  *which organization this wants to leave for*; `quilt-core` says nothing.
- **Makes possible:** shipping today, in the most literal sense.
- **Verdict: viable only for the packages that are genuinely standalone.** It is a real
  option for `@quilt/pincher` and `@quilt/fleet` (no internal deps) and a bad one for a
  shared core. Blanket-applying it is how you end up renaming `core` four times.

---

### Option 5 — *NEW, forced by measurement* — Publish under a scope nobody holds; leave `@quilt/*` as an aspiration

The measurement says the aspiration and the destination disagree. The alternative is to
decouple them: publish immediately under a scope we demonstrably control, and record
`@quilt/*` as the graduation target in package metadata rather than in the registry.

- **What must be true:** that a scope exists and is ours. **UNVERIFIED** — `npm access list`
  returned 403 for every scope tested *including our own profile endpoint*, so the token in
  this environment cannot answer the question. It must be answered with a real account
  credential (§6, V1).
- **Costs:** one coordinated rename across the manifests that name a scope; two npm orgs
  later if the graduation happens.
- **Breaks:** consumers written against `@quilt/*` need a rewrite — but there are **none**,
  because nothing is published and nothing external depends on it. There is no migration
  cost here that does not already exist as a broken install.
- **Makes possible:** the five failing publish jobs go green this week; `quilt-claw`'s 182
  tests get a CI signal; the sketchbook starts leaving receipts again.
- **Verdict: the only option whose cost does not depend on V1 coming back yes.**

---

### Option 6 — *NEW* — Fix the protocol first; decide the name second

The five packages with publish CI fail **at install**, not at publish. The name is not
consulted by `npm install`.

- **What must be true:** nothing. It is true regardless of every unresolved question above.
- **Costs:** change `workspace:*` → `file:` in **3 repos** (`quilt-ai`, `quilt-evolve`,
  `quilt-claw`), touching **7 dependency lines** total. Two of those three (`quilt-claw`,
  `quilt-ai`) have no lockfile.
- **Breaks:** nothing outside those three repos. The cycle and the missing `@quilt/core`
  remain, and `file:` does not fix either — but it converts a hard install failure into a
  soft one, which is the difference between "cannot be tested" and "tested and failing."
- **Makes possible:** `npm ci`, `tsc`, and the 182 `quilt-claw` tests all running. That is
  the precondition for every other option being evaluated on evidence instead of on hope.
- **Verdict: strictly enabling. Do it regardless of what Casey decides about names.**

---

## 4. Comparison

| | 1. unscoped `core` | 2. new `@quilt/core` | 3. move to org | 4. drop scope | 5. new scope | 6. fix protocol |
|---|---|---|---|---|---|---|
| Blocked on third-party ownership | no — **takes it** | **yes** (V1) | **yes** (V1, V4) | no | no | **no** |
| Wires in a stranger's code | **YES** | no | no | no | no | no |
| Handles `@quilt/core` 0.3.0 collision (C3) | no | **no** | no | no | n/a | n/a |
| Preserves the graduation ticket | no | yes | yes | **no** | deferred | n/a |
| Unblocks any publish today | no | no | no | partly | **yes** | **yes (install)** |
| Cost | 3 lines | rename + collision | transfer + ownership | rename | rename | **7 lines, 3 repos** |
| Reversible later | **no** | partly | no | **no** | **yes** | yes |

**Every option that is cheap today is expensive later, and every option that is expensive
today is blocked on facts we do not yet have.** That is the shape of this problem.

---

## 5. Recommendation

**Recommendation (architect's; the decision is Casey's):**

1. **Do Option 6 now — unconditionally.** Replace `workspace:*` with `file:` in `quilt-ai`,
   `quilt-evolve`, `quilt-claw`. 7 lines, 3 repos, no external dependency, and it makes
   182 tests runnable that currently cannot be installed at all. This is independent of
   every naming question and is not blocked by any of them. Detail in `INTEGRATION-NOTE.md`.

2. **Run V1 and V2 today (≈2 minutes).** They are the two facts the whole ADR pivots on,
   and both are one command each. If `@quilt` is writable by Casey, Option 2 or 3 becomes
   live. If `quilt-core` on npm is *not* the pane library, Option 1 becomes live. Right now
   both are unverified in the direction that matters.

3. **Reject Option 1 outright**, on the evidence in C1. Not "prefer another option" —
   reject. It is the only option that would execute third-party code inside the Quilt
   runtime.

4. **If V1 comes back negative:** take Option 5 — publish under a scope we control, keep
   `@quilt/*` as the recorded graduation target in metadata — and re-open Option 3 when
   Casey actually owns the destination org.

5. **Defer the `@quilt/core` 0.3.0 vs 0.4.0 product question (C3) explicitly and visibly.**
   It is not a packaging question. It should be its own ADR (ADR-002) once install is fixed
   and the code is readable again.

6. **Do not touch the cycle yet (F7).** npm does not forbid cyclic dependencies between
   separately-published packages. It becomes fatal only under a single-monorepo workspace
   build. Break it when and if these packages are actually co-located — not before.

**Why this ordering:** the name is the gate, and a gate you cannot open is not worth
inspecting. Option 6 converts five red CI jobs into a real signal for the price of seven
lines, and it is the only action on this page whose cost does not depend on an answer Casey
does not have yet.

---

## 6. Verification — the four commands that settle it

Each is read-only, repeatable by a stranger, and each leaves a receipt.

**V1 — Do we hold the npm `@quilt` scope?** *(the fork in the road)*
```bash
npm whoami --registry=https://registry.npmjs.org/
npm access list packages @quilt --registry=https://registry.npmjs.org/
# reads "…@quilt/fleet" as writable  -> YES, Options 2 and 3 are live
# E403 Forbidden                   -> NO,  Option 5 is the only live path
```
*Receipt:* the command output, pasted into this ADR, replacing the DECIDED/PENDING header.
*Note:* the token in the build environment returns 403 **even for `npm profile get`**, so
it cannot answer this. A human credential must.

**V2 — Is npm `quilt-core` the pane library or our daemon?** *(confirms C1)*
```bash
curl -s https://registry.npmjs.org/quilt-core \
  | jq -r '.versions["0.11.0"] | "\(.repository.url)  |  \(.description)  |  deps=\(.dependencies|length)"'
# expect: git+https://github.com/niko-dellic/quilt.git
#         Framework-independent pane layout model, validation, commands, and subscriptions
#         deps=0
```
*Receipt:* that one line. **If it ever prints `SuperInstance/quilt-core-os` and
`better-sqlite3`, Option 1 is live and this ADR is wrong — re-open it.**

**V3 — Does `@quilt/core` 0.3.0 still mean something to someone?** *(C3)*
```bash
curl -s -H "Authorization: token $GITHUB_TOKEN" \
  https://api.github.com/repos/SuperInstance/quilt/contents/packages/core/package.json \
  | jq -r '.content' | base64 -d | jq '{name,version,dependencies,peerDependencies}'
```
*Receipt:* the manifest, plus a one-line note from Casey: *"is this the same product as
quilt-core-os?"* — yes / no / related. That answer is ADR-002's input.

**V4 — Is the `quilt` GitHub org Casey's to write to?**
```bash
curl -s -H "Authorization: token $GITHUB_TOKEN" https://api.github.com/orgs/quilt \
  | jq '{login,created_at,public_repos}'
# 2019-04-03, 38 public repos, all Ewasm/crypto — no SuperInstance relationship
```
*Receipt:* one line from Casey: *"I own this org" / "I do not" / "I have asked."* Option 3
is **blocked** until that line says yes.

---

## 7. The decision — PENDING

> **The decision is not the architect's. It is Casey's.**
>
> ### ADR-001 Status: **DECIDED — PENDING**
> >
> > **The exact question to answer:**
> >
> > **Does Casey hold write access to the npm `@quilt` scope — and separately, is the
> > `quilt` GitHub org his to write to? The answer selects the graduation destination:
> >
> > - **YES to npm `@quilt`, YES to the org** → **Option 3.** Move `quilt-core-os` into the
> >   org, publish `@quilt/core` from there, account keeps the sketch. This is the shape the
> >   scope was invented for.
> > - **YES to npm `@quilt`, NO to the org** → **Option 2.** Publish `@quilt/core` from
> >   `SuperInstance/quilt-core-os` in place; keep `quilt-core` as a deprecated alias (which
> >   is impossible while the third party holds it — so the "alias" degrades to a README
> >   note). ADR-002 must still resolve C3.
> > - **NO to npm `@quilt`, YES to the org** → **Option 3'**, split the two: publish under a
> >   scope we control, point `repository` at the org, graduate the repo when the org is
> >   ready.
> > - **NO to both** → **Option 5.** Publish under a scope we demonstrably hold; record
> >   `@quilt/*` as the graduation target in metadata, not in the registry; revisit when
> >   ownership changes.
> >
> > **In every branch, Option 6 (fix `workspace:*` → `file:`) happens first and regardless.**

**Interim position while pending:** no package is published under any `@quilt/*` or
`quilt-*` name; Option 1 is rejected and must not be attempted; Option 6 may proceed.

---

## 8. Rejected options, with reasons

| Option | Rejected because |
|---|---|
| **1. unscoped `quilt-core`** | The package that name resolves to is a third party's pane-layout library (C1). Accepting this wires a stranger's code — and its lifecycle scripts — into the Quilt runtime, at the root, beneath a mutual dependency. The single cheapest-looking option is the only actively dangerous one. |
| **4. drop the scope, blanket** | Not wrong for standalone packages, wrong as a policy. It deletes the graduation ticket (the whole reason the scope exists), and it puts the family into an unscoped global namespace where a third party is already publishing `quilt-vanilla`, `quilt-react`, `quilt-sdk`, `quilt-nightly`. `quilt-core` is proof the collision already happened. |
| **3. move to the `quilt` org — as briefed** | Not rejected on merit. **Blocked on V4.** The org holds 38 repos of unrelated Ewasm/crypto work. Treating it as a destination without Casey asserting ownership would mean publishing Quilt into someone else's namespace. Re-open the moment V4 is yes. |
| **"Break the cycle first"** | The cycle is real but is not the failure. npm permits cyclic deps between separately-published packages; it only bites under a single-workspace build or a naive publish-order script. Breaking it now is work that may be undone by the org decision. Sequence it *after* Option 6, not before. |
| **"Publish `quilt-core` from `quilt-core-os` under the existing unscoped name"** | Not an option. The name is owned by a third party. npm would reject the publish, and the attempt is how you discover that by losing a version number. |

---

## 9. What would change our mind

Stated in advance, so the next reader does not have to re-derive it.

| # | Finding | Effect |
|---|---|---|
| **1** | V2 prints `SuperInstance/quilt-core-os` and `better-sqlite3` — i.e. npm `quilt-core` **is** ours | **Option 1 is live.** C1 is wrong; three lines become the whole fix. Re-open this ADR. |
| **2** | Casey holds `@quilt` **and** the `quilt` org | **Option 3, fully.** The scoping question dissolves into a repo move. |
| **3** | `@quilt/core` 0.3.0 and `quilt-core` 0.4.0 turn out to be the same product (V3 = yes) | C3 dissolves; Option 2 loses its main objection; the ADR collapses toward a naming-only decision. |
| **4** | They are confirmed to be *different* products (V3 = no) | `@quilt/core` must be split or renamed **before** anything publishes. This becomes the critical path, above the cycle. |
| **5** | `quilt-claw`'s 182 tests fail once `file:` lands | **This would falsify the briefing's measurement.** The problem is then not plumbing-only, and §5's ordering changes. *This is the single most valuable thing to learn, and Option 6 buys it cheaply.* |
| **6** | `@quilt/k3s`'s `^0.1.0` peer floor is deliberate — it targets `quilt-core-os` 0.4.0's *intended* line | Evidence that `@quilt/core` was never meant to be the scoped name, strengthening Option 1's *intent* while leaving it *unsafe*. Then the answer is a third name, not a fourth option. |
| **7** | npm scope ownership turns out to be purchasable/transferable from Quilt Data | Option 2/3 revive. Until then they are wishes. |
| **8** | The cycle turns out to be enforced by `tsc` project references or an import that cannot be expressed one-way | F7 is wrong; the cycle moves to §5's step 1. `quilt-claw`'s 27 test files are the evidence either way. |
| **9** | The 2 vendored upstreams' `workspace:` specs are load-bearing for the build | The INTEGRATION-NOTE's "do not touch" list must gain an exception. *UNVERIFIED — I could not locate them; see INTEGRATION-NOTE §6.* |

---

## 10. What this ADR deliberately does not say

- It does not say which name is right. That is §7, and §7 belongs to Casey.
- It does not claim `@quilt` is *definitely* unpublishable. V1 has not been run with a
  credential that can answer it. F1 is strong evidence, not proof.
- It does not treat the 257 `quilt-*` repos as 257 systems. They share cores and many have
  thin tests (C7). Family size is a count.
- It does not propose touching the 5,280 other repos, the 2 archived ones, or the 6 unscoped
  `workspace:` users. `INTEGRATION-NOTE.md` §5 names what must not be touched.

---

## Appendix A — verification log

Every check, its command class, and its result. Run 2026-10-18, read-only throughout; no
repository was modified, created, pushed, or published.

| # | Check | Result |
|---|---|---|
| A1 | `GET /orgs/quilt` | 200 · org · created 2019-04-03T13:31:09Z · 38 public repos |
| A2 | `GET /orgs/quilt/repos` | 38 repos, all Ewasm/crypto; zero `quilt-*` from the sketchbook |
| A3 | `GET /orgs/luciddreamer` | **404** |
| A4 | `GET /users/luciddreamer` | **200** · type User · id 131851688 · created 2023-04-26 · 0 public repos |
| A5 | `GET /orgs/cocapn` / `GET /users/cocapn` | 404 / 404 — gone, consistent with a rename |
| A6 | `GET /users/SuperInstance/repos?type=owner` (full pagination) | **5,280** repos; **257** `quilt-*`; 0 archived; Aug 47 / Sep 145 / Oct 65 |
| A7 | `registry.npmjs.org` × 17 package names | 14 scoped → all **404**; `quilt-core` → **0.11.0, 18 versions** |
| A8 | `quilt-core@0.11.0` tarball, extracted | MIT, **0 deps**, `model/controller/join/join-rectangle/types`, repo `niko-dellic/quilt` `packages/core` — **C1** |
| A9 | `registry.npmjs.org/@quilt%2fquilt` | 0.4.2 · sole maintainer `kevinklin <kevin@quilt.io>` — **C4** |
| A10 | `SuperInstance/quilt-core-os` tree | **32 blobs** · `build.yml`, `SECURITY.md`, `CODEOWNERS`, `dependabot.yml`, 3 install/recovery docs — briefing confirmed |
| A11 | `SuperInstance/quilt` `packages/*` | 5 packages: core 0.3.0, sdk 0.2.0, mcp 0.3.0, tui 0.3.0, cli 0.3.0 — **C2** |
| A12 | publish workflows | `quilt-fleet/release.yml`, `quilt-pincher/publish.yml`, `agent-grid|cost-analysis|monitoring-dashboard/publish.yml` = **5** |
| A13 | `quilt-claw` shallow clone | 80 blobs · **27** `test/*.test.js` · **182** `test()` · **no lockfile** |
| A14 | `workspace:*` install repro, empty dir | `EUNSUPPORTEDPROTOCOL · Unsupported URL Type "workspace:": workspace:*` |
| A15 | `plato-client` tree | **no `package.json`**; `client.cpython-310.pyc`; `cocapn_plato-0.1.0-py3-none-any.whl` — **C6** |
| A16 | `engine/engine.js` across 7 repos | **28,145 bytes, identical in all seven**; briefed 97,486/71,216 **not reproduced** — **C7, UNVERIFIED** |
| A17 | `npm access list packages @quilt` | 403 — **but also 403 for `npm profile get`**, so the token cannot answer scope ownership either way. **V1 remains open.** |

