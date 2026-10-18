# INTEGRATION-NOTE — from "three packages cannot publish" to "they can"

Companion to `ADR-001-scoped-or-unscoped.md`. That ADR decides *where the namespace goes*.
This note is the *ordered path to a working install*, and it deliberately does not wait for
that decision.

**Doctrine:** *form follows force.* The force here is seven dependency lines in three
repositories. That is the whole of the plumbing problem.

---

## 0. The finding that makes this short

The publish CI is not failing at publish. **It is failing at `npm install`**, before the
publisher is ever invoked. Reproduced from an empty directory:

```
$ npm install          # deps: { "@quilt/core": "workspace:*" }
npm error code EUNSUPPORTEDPROTOCOL
npm error Unsupported URL Type "workspace:": workspace:*
```

`workspace:*` is a **monorepo-internal protocol**. It resolves inside a workspace root and
nowhere else. `quilt-ai`, `quilt-evolve` and `quilt-claw` each live in their **own** repo,
with **no `workspaces` array** — so `workspace:*` points at nothing.

There is a fix already in use, in this same account, by a package that works:
`SuperInstance/quilt` → `packages/tui` depends on `"@quilt/core": "file:../core"`.
`file:` is the protocol npm **rewrites at publish time**, which is exactly the property
needed: resolvable locally now, correct to consumers after publish.

**That is why this path is seven lines and not a migration.**

---

## 1. Scope of the change — exactly what is edited

Three repositories. Seven dependency lines. Nothing else in the sketchbook is touched.

| # | Repo | Manifest | Line(s) | From | To |
|---|---|---|---|---|---|
| 1 | `quilt-ai` | `package.json` | deps | `"@quilt/evolve": "workspace:*"` | `"file:../quilt-evolve"` |
| 2 | `quilt-ai` | `package.json` | deps | `"@quilt/core": "workspace:*"` | `file:` — **see §2, do not guess** |
| 3 | `quilt-evolve` | `package.json` | deps | `"@quilt/ai": "workspace:*"` | `"file:../quilt-ai"` |
| 4 | `quilt-evolve` | `package.json` | deps | `"@quilt/core": "workspace:*"` | `file:` — **see §2** |
| 5 | `quilt-claw` | `package.json` | deps | `"@quilt/core": "workspace:*"` | `file:` — **see §2** |
| 6 | `quilt-claw` | `package.json` | deps | `"@quilt/ai": "workspace:*"` | `"file:../quilt-ai"` |
| 7 | `quilt-claw` | `package.json` | deps | `"@quilt/evolve": "workspace:*"` | `"file:../quilt-evolve"` |

Versions, both `0.1.0`. `quilt-claw` is 210 KB / 27 test files / 182 `test()` calls /
**no lockfile**. `quilt-ai` 88 KB, `quilt-evolve` 6,159 KB.

### 1.1 The decision that must be made before line 1: are they co-located?

`file:` means **a relative path on disk**. It only works if the three repos sit side by side.

```
   the sibling layout that file: requires        the git layout that exists today
   ────────────────────────────────────          ────────────────────────────────
   work/                                          quilt-ai/        quilt-evolve/   quilt-claw/
    ├── quilt-ai/     ← file:../quilt-evolve       quilt-evolve/    quilt-claw/    …
    ├── quilt-evolve/ ← file:../quilt-ai           (no package.json at this level)
    └── quilt-claw/    ← file:../quilt-ai
```

`quilt-claw` **already assumes this**: it has no `package-lock.json` and no committed
`node_modules`. It has been expecting siblings.

**The order of operations depends on this answer:**

- **(a) CI clones all three side by side into one job** — then `file:` works as written.
- **(b) Each repo is built alone in its own job** — `file:` cannot work, and the fix is
  `publishConfig` + a published dep (Stage 5), or a single `workspaces` monorepo (Stage 6).

> **Do not edit anything until this is answered.** It is a one-question check against the
> existing workflow files:
> ```bash
> curl -s -H "Authorization: token $GITHUB_TOKEN" \
>   https://api.github.com/repos/SuperInstance/quilt-claw/contents/.github/workflows \
>   | jq -r '.[].name'
> # quilt-claw has ci.yml only (verified). Read it.
> #   path: work/quilt-claw          -> branch (a), proceed as written
> #   path: checkout single repo     -> branch (b), go to Stage 5
> ```

---

## 2. Stage 0 — settle the `@quilt/core` target, or park those three lines

Three of the seven lines point at `@quilt/core`, which **does not resolve from anywhere**:

- npm: `registry.npmjs.org/@quilt/core` → **404**.
- In-account, `SuperInstance/quilt` `packages/core` → `@quilt/core` **0.3.0** — a yaml+zod
  spreadsheet-cell model.
- In-account, `quilt-core-os` → declares **`quilt-core`** (unscoped) 0.4.0 — an edge daemon.
- On npm, `quilt-core` (unscoped) **0.11.0** → **`niko-dellic/quilt`**, a third party's MIT
  pane-layout library, **zero dependencies**.

**Do not point any `file:` at the unscoped `quilt-core` on npm.** That is the supply-chain
trap in ADR-001 §3 Option 1. Verify before believing the name matches:

```bash
curl -s https://registry.npmjs.org/quilt-core \
  | jq -r '.versions["0.11.0"] | "\(.repository.url) | \(.description) | deps=\(.dependencies|length)"'
# expect: git+https://github.com/niko-dellic/quilt.git
#         Framework-independent pane layout model, ... | deps=0
```

**Recommendation:** in Stage 1, replace the three `@quilt/core` lines with **`file:` stubs
pointing at a local `core` directory**, or — if none exists yet — leave those three lines
as `workspace:*` for now and do **only** the four `@quilt/ai` / `@quilt/evolve` lines
(1, 3, 6, 7). Those four are unambiguous and unblock `quilt-claw`'s 182 tests on their own.

> **Do not fabricate a `core` package.** A stub that does not implement the API `@quilt/ai`
> and `@quilt/evolve` expect will turn an *install* error into a *typecheck* error, and you
> will have spent the signal that Stage 1 exists to buy.

---

## 3. Stage 1 — `workspace:*` → `file:` (the enabling change)

**Blocked by:** Stage 0's `@quilt/core` answer, and §1.1 branch (a).
**Unblocked by:** nothing. Independent of ADR-001 entirely.

```bash
export GIT_SSL_NO_VERIFY=1
git clone --depth 1 https://x-access-token:$GITHUB_TOKEN@github.com/SuperInstance/quilt-ai.git     work/quilt-ai
git clone --depth 1 https://x-access-token:$GITHUB_TOKEN@github.com/SuperInstance/quilt-evolve.git work/quilt-evolve
git clone --depth 1 https://x-access-token:$GITHUB_TOKEN@github.com/SuperInstance/quilt-claw.git    work/quilt-claw
cd work/quilt-ai     && npm pkg set dependencies.@quilt/evolve="file:../quilt-evolve"
cd ../quilt-evolve   && npm pkg set dependencies.@quilt/ai="file:../quilt-ai"
cd ../quilt-claw     && npm pkg set dependencies.@quilt/ai="file:../quilt-ai" \
                                dependencies.@quilt/evolve="file:../quilt-evolve"
```

**Verify (a stranger can repeat this):**
```bash
cd work/quilt-claw
npm install            # MUST NOT print EUNSUPPORTEDPROTOCOL
npm test               # expect 182 test() calls across 27 files — PASS or FAIL, but it must RUN
```

**Expected result: `npm install` succeeds.** The cycle (`ai ⇄ evolve`) is *not* an install
failure — `file:` is a filesystem path and a cycle is just two paths pointing at each other.

**Receipt this stage must leave behind:**
1. A passing (or honestly failing) `quilt-claw` CI run, logged.
2. The `npm install` output with **no** `EUNSUPPORTEDPROTOCOL` line.
3. A commit per repo whose message records: *"workspace:* → file:; reason: these packages
   are separate repos, workspace: resolves only inside a workspace root."* The commit is the
   durable part. The CI run evaporates; the reasoning does not.

> **The test result is the point of this stage.** The briefing measured 182/182 passing and
> this is where that becomes checkable rather than asserted. If they **fail** here, ADR-001
> §9 row 5 fires and the problem is larger than plumbing.

---

## 4. Stage 2 — confirm the cycle is not the blocker

`@quilt/ai ⇄ @quilt/evolve` is real. **It is not why publish fails.** npm permits cyclic
dependencies between separately-published packages; it only bites under a single-workspace
build (`npm run -ws build` deadlocks on topological order) or a naive publish-order script.

**Do not break it yet.** Verify it is survivable:
```bash
cd work/quilt-ai   && npm run build && echo "ai builds"
cd ../quilt-evolve && npm run build && echo "evolve builds"
```
Both build in isolation despite each depending on the other ⇒ the cycle is a **type**
constraint, not a **schedule** constraint, and is safe until the packages are co-located.

**Receipt:** both build lines captured. Note in the PR: *"cycle retained deliberately —
revisit if these move into one workspaces monorepo (Stage 6)."*

---

## 5. Stage 3 — what the publish CI would need

Applies to the **5** repos with a publish workflow. Verified list:

| Repo | Workflow | Internal deps | State |
|---|---|---|---|
| `quilt-fleet` | `.github/workflows/release.yml` | **none** | blocked by **name only** |
| `quilt-pincher` | `.github/workflows/publish.yml` | **none** | blocked by **name only** |
| `agent-grid` | `.github/workflows/publish.yml` | none (`{}`) · peer react 18\|19 | blocked by **name only** |
| `cost-analysis` | `.github/workflows/publish.yml` | `recharts ^2.12.0` | blocked by **name only** |
| `monitoring-dashboard` | `.github/workflows/publish.yml` | `recharts ^2.12.0` | blocked by **name only** |

**Read that table carefully: none of these five has an internal dependency.** Not one. Their
CI fails because the **scope** cannot be written to — the `@quilt/core` problem does not
touch them, and Stage 1 does not touch them. They are a **separate problem wearing the same
face.**

A publish workflow needs, minimum:
1. `NODE_AUTH_TOKEN` from an npm secret, scoped to the right **org**.
2. `npm whoami` / `npm access list packages <scope>` succeeding first — the check that
   currently fails.
3. `registry-url: https://registry.npmjs.org` in `setup-node`.
4. For `@quilt/*` specifically: proof the `@quilt` scope is writable (ADR-001 **V1**).
5. `npm publish --access public` for scoped packages.

**Verify:**
```bash
npm whoami --registry=https://registry.npmjs.org/
npm access list packages @quilt     --registry=https://registry.npmjs.org/
npm access list packages @luciddreamer --registry=https://registry.npmjs.org/
```
> The build-environment token returns **403 for every scope — including `npm profile get`
> for itself** (ADR-001 A17). It therefore cannot answer this question, in either
> direction. **A human credential must run V1.** Do not read the 403 as "not ours."

**Receipt:** the output of those three commands, plus a note naming which scopes are
writable. For `@luciddreamer`, also record the **user-vs-org** collision (ADR-001 C5):
`/orgs/luciddreamer` → 404 while `/users/luciddreamer` → **200** (type `User`,
created 2023-04-26). You cannot create that org later without resolving the user collision.

---

## 6. What must NOT be touched

**Read this section before opening any editor.**

| Do not touch | Why | Measured |
|---|---|---|
| **The 2 vendored upstreams** using `workspace:` — `@deepseek-ai/dsh-root`, `@vitejs/vite-monorepo` | Third-party vendored trees. Rewriting their protocol spec is editing someone else's source. | **UNVERIFIED — I could not locate them.** Neither a repo named `dsh-root`/`vite-monorepo` (both 404) nor a code search for `@deepseek-ai/dsh-root` in `SuperInstance` (0 results) found them. They are presumably `vendor/` subtrees inside other repos. **Find them before Stage 1 and leave them alone** — the same `sed` must not sweep them in. |
| **The 6 unscoped `workspace:` users** | Out of scope; none of the 14 scoped packages depend on them. | briefing-supplied, **UNVERIFIED** — spot checks of `quilt`, `quilt-core-os`, `quilt-swarm`, `quilt-nn` found **0** `workspace:` uses each. Their repos are `private` or unscoped. |
| **`SuperInstance/quilt` `packages/*`** | Holds a **second, different** `@quilt/core` (0.3.0, yaml+zod cell model) plus `@quilt/sdk` 0.2.0, `@quilt/mcp`/`@quilt/tui`/`@quilt/cli` 0.3.0. Rewriting it to point at the edge daemon would silently redefine a name something already means. | ADR-001 C2, C3 |
| **`@quilt/tui`'s `file:../core`** | **It already works.** It is the proof the protocol choice is correct. Leave it as the reference implementation. | verified |
| **`@quilt/k3s`' `peerDependencies` `^0.1.0`** | A deliberate floor, not plumbing. Changing it is a product decision (ADR-001 §9 row 6). | verified |
| **`cocapn-browser-agent`'s two `file:vendor/*` deps** | Already resolvable — the only internal edges in the graph that work. | verified |
| **The other 253 `quilt-*` repos** | Not involved. Seven share a byte-identical `engine/engine.js` (28,145 B), so a careless bulk edit would hit all seven at once. | ADR-001 C7 |
| **`plato-client`** | Not a JS package at all — no `package.json`; it is Python. | ADR-001 C6 |

**The sweep hazard, concretely:** a repo-wide `sed -i 's/workspace:*/file:../&/g'` is wrong
twice over — it corrupts the 2 vendored upstreams, and it invents paths for `@quilt/core`
that point nowhere. **Edit the three named files by hand.**

---

## 7. Verification matrix

One row per stage. A stranger with only this note should be able to run all of it.

| Stage | Command | Pass criterion | Receipt |
|---|---|---|---|
| **Pre** | `curl -s https://registry.npmjs.org/quilt-core \| jq -r '.versions["0.11.0"].repository.url'` | prints `niko-dellic` — **not** `SuperInstance` | the line, pasted |
| **Pre** | `curl -s -H "Authorization: token $GITHUB_TOKEN" .../repos/SuperInstance/quilt/contents/packages/core/package.json` | prints `@quilt/core` `0.3.0` | the manifest + Casey's one-line verdict |
| **1** | `cd work/quilt-claw && npm install` | no `EUNSUPPORTEDPROTOCOL` | install log |
| **1** | `cd work/quilt-claw && npm test` | **182** `test()` calls across **27** files execute | test output + pass/fail count |
| **1** | `grep -c 'workspace:' work/*/package.json` | **0** in `quilt-ai`, `quilt-evolve`, `quilt-claw` | grep output |
| **2** | `npm run build` in `quilt-ai` and `quilt-evolve`, separately | both exit 0 | two lines |
| **3** | `npm whoami && npm access list packages <scope>` | no 403 | command output |
| **4** | `npm publish --dry-run` in each of the 5 repos | tarball lists only `files[]`, no `workspace:` string | dry-run output ×5 |
| **5** | `npm view <pkg> version --registry=https://registry.npmjs.org/` | `404` → version string | before/after |

**Stage 4 — `--dry-run` is the safe rehearsal.** It resolves and packs without publishing.
It catches the two things that actually break here: a `workspace:` spec surviving into the
manifest, and `files[]` not covering `dist/`. Run it in all five.

---

## 8. The whole path

```
  STAGE 0   answer two questions            ~2 min   no repo touched
              · do we hold npm @quilt?                    (ADR-001 V1)
              · is npm quilt-core ours?                   (ADR-001 V2)
                    │
                    ├─ V1 yes ──────────────► naming path (ADR-001 §7) — PENDING, Casey's
                    │
                    └─ V1 no  ──────────────► ADR-001 Option 5 (scope we control)
                                        │
  STAGE 1   workspace:* → file:           7 lines, 3 repos, no external dependency
              quilt-ai · quilt-evolve · quilt-claw
              ★ UNCONDITIONAL — do not wait for the naming decision
                    │
                    ▼
            quilt-claw installs. 182 tests RUN.       ◄── the signal Stage 1 exists to buy
                    │
  STAGE 2   prove the cycle survives                    (optional, cheap, do it)
                    │
  STAGE 3   5 publish workflows: name, not deps        (independent of 1 and 2)
                    │
  STAGE 4   npm publish --dry-run × 5                  (safe rehearsal)
                    │
  STAGE 5   publish ── only after ADR-001 §7 is answered
```

**Critical path: Stage 1.** Seven lines, three repositories, zero external dependency, and
it buys the one thing nothing else buys — the ability to *see* whether the implementation is
sound instead of inferring it.

**Total blast radius: 3 files in 3 repos, none of which anything else depends on**, because
nothing is published and no external consumer exists.

---

## 9. What is deliberately NOT in this note

- **Which scope to use.** That is ADR-001 §7, and it is Casey's. Stages 0–4 are
  name-agnostic on purpose.
- **How to break the `@quilt/ai ⇄ @quilt/evolve` cycle.** Stage 2 establishes whether it
  *needs* breaking. If those packages move into one `workspaces` monorepo it becomes
  mandatory; until then it is optional churn.
- **Renaming `@quilt/core`.** Blocked on ADR-001 V3 — and it is a product decision, not a
  packaging one. It becomes **ADR-002**.
- **Touching the 6 unscoped `workspace:` users or the 2 vendored upstreams.** §6.
- **Publishing anything under `@quilt/*` before ADR-001 is answered.** The failure mode is a
  permanently spent version number, and npm names are effectively irreversible.

---

## 10. Open questions for Casey, in the order they block work

1. **Do the three CI jobs clone the repos side by side?** (§1.1) — blocks Stage 1's form.
2. **Is npm `@quilt` ours to write to?** (ADR-001 V1) — blocks every publish.
3. **Is `SuperInstance/quilt` `packages/core` 0.3.0 the same product as `quilt-core-os`
   0.4.0?** (ADR-001 V3) — blocks `@quilt/core` forever; this is ADR-002's input.
4. **Is the `quilt` GitHub org yours?** (ADR-001 V4) — blocks Option 3, which is otherwise
   the right destination.
5. **Where are the 2 vendored upstreams?** (§6) — blocks "do not touch" from being
   enforceable rather than aspirational.
6. **Is the GitHub user `luciddreamer` you?** (ADR-001 C5) — a user, not an org, holds the
   name. Blocks creating that org later.

