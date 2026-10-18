# Correction: the name "quilt" is held three times over, and I attributed a stranger's package to ourselves

*2026-10-18. This corrects [ATLAS-01](ATLAS-01.md) and its addendum, and
[ATLAS-02](ATLAS-02.md). The architecture was produced by `bruno-architect`,
the history by `herman-historian`, and the incident count by both. Every
correction below was measured against the live registry and the live
account, and each one changes a decision.*

---

## 1. The wrong claim

The addendum says:

> **the root exists but is misnamed.** The repository `quilt-core-os` declares
> `"name": "quilt-core"` (unscoped, v0.4.0) and `quilt-core` is on npm at
> **0.11.0 with 18 versions** — while everything depending on it wants the
> **scoped** `@quilt/core`. One naming decision separates three graduation
> tickets from being publishable.

**Three parts of that are wrong, and the third is dangerous.**

### 1a. The `quilt-core` on npm is not ours

```
$ curl -s https://registry.npmjs.org/quilt-core | jq '.versions["0.11.0"].repository'
git+https://github.com/niko-dellic/quilt.git   (directory: packages/core)
$ ... | jq '.maintainers'
[{"name":"niko.dellic","email":"nikodellic@gmail.com"}]
```

**It is `niko-dellic/quilt`, `packages/core` — a MIT-licensed browser
pane-layout library** shipping `model.ts` / `controller.ts` / `join.ts` /
`join-rectangle.ts` / `types.ts`, with **zero dependencies**. A description
that has nothing to do with a maritime edge runtime.

`SuperInstance/quilt-core-os` is *"the TypeScript daemon that ships inside
the quilt-core snap"* — `better-sqlite3`, Apache-2.0, Jetson and
Raspberry-Pi install docs, snap hooks.

**These are not the same artifact.** The shared `0.4.0` inside a
`0.1.0 … 0.11.0` series was a coincidence of numbering — and it is exactly
the coincidence that produced my error. I read "the repo declares this
name" and "a package with this name is published" and concluded they were
the same thing, without checking the maintainer.

**`quilt-core-os` has never been published and never can be under that name
without the owner's cooperation.** The cheapest-looking option on the
table is a trapdoor: accepting it wires a stranger's code, and its
lifecycle scripts, into the Quilt runtime, at the root, beneath a mutual
dependency.

### 1b. `@quilt/core` *is* declared on the account — I said it was not

```
SuperInstance/quilt  (monorepo, 15 MB, 528 blobs, 5 packages)
  packages/core   @quilt/core  0.3.0   yaml + zod          <- the root
  packages/tui    @quilt/tui   0.3.0   deps: "@quilt/core": "file:../core"
  packages/mcp    @quilt/mcp   0.3.0   deps: "@quilt/core": "*"
  packages/cli    @quilt/cli   0.3.0   deps: "@quilt/core": "*", "@quilt/mcp": "*", "@quilt/sdk": "*"
  packages/sdk    @quilt/sdk   0.2.0   ajv, ajv-formats
```

**I wrote "the root exists nowhere" and it was in the account the whole
time**, in the `quilt` monorepo rather than in any `quilt-*` repo.

### 1c. The name is held three times over

```
                 the name "quilt" is held three times over
                 ──────────────────────────────────────────

  quilt (GitHub org)        quiltdata (GitHub org)      SuperInstance (user)
  created 2019-04-03        created 2016-02-07         5,280 repos
  38 repos · Ewasm/crypto   87 repos · data catalog    257 × quilt-*
          │                        │                           │
          │ grants no             │ maintains                 │ 257 repos
          │ npm scope             │ @quilt/quilt@0.4.2         │ own no
          │                       │ (kevin@quilt.io)          │ name
          ▼                        ▼                           ▼

  npm: @quilt/*  ───────────► OCCUPIED (quiltdata)
  npm: quilt-core ──────────► OCCUPIED (niko-dellic)
  npm: quilt ───────────────► OCCUPIED (ooyala, "File stitcher")
```

**So "one naming decision" is not the shape of it.** There are three
holders, and one of them holds the npm scope that the graduation ticket
names. Separately there is a **product** decision hiding inside a
packaging one: `@quilt/core` 0.3.0 is a yaml+zod **spreadsheet-cell
model**; `quilt-core` 0.4.0 is an **IoT edge daemon**. They are not
variants of one another, and unifying them has not been decided.

And one more that reads like a 404 but is not: `luciddreamer` is
**`GET /users/luciddreamer` → 200**, a real GitHub **User**
(`id 131851688`, created 2023-04-26, 0 public repos). A real person holds
the name. That is a different problem from an unclaimed namespace, and it
cannot be created later without resolving the user collision.

---

## 2. The correction that is *good* news: the fix is already in the account

The three split-out repos (`quilt-claw`, `quilt-ai`, `quilt-evolve`) use
`workspace:*`, which is monorepo-internal and cannot install standalone:

```
npm error code EUNSUPPORTEDPROTOCOL
npm error Unsupported URL Type "workspace:": workspace:*
```

But **inside the `quilt` monorepo the same dependency is already written
three different ways**, and one of them works:

| package | how it depends on `@quilt/core` | installs standalone? |
|---|---|---|
| `packages/tui` | `"file:../core"` | **yes — `file:` is rewritten by npm at publish time** |
| `packages/mcp` | `"*"` | no — resolves to the registry, which 404s |
| `packages/cli` | `"*"` | no — same |
| `quilt-claw` | `"workspace:*"` | **no — the protocol is refused outright** |
| `quilt-ai` | `"workspace:*"` | no |
| `quilt-evolve` | `"workspace:*"` | no |

**`file:` is the protocol that does the job**: resolvable locally now,
correct to consumers after publish. The fleet already solved this inside
the monorepo and the three repos that were split out of it did not get the
same treatment. **The path is seven dependency lines, not a migration** —
and it is independent of the naming decision, which can take as long as it
needs to take.

---

## 3. Two more corrections from the same pass

**The 2026-08-24 incident was sixty repositories wide, not eight.** I
recorded eight. The account holds **59** `recovered-copy-20260824-*`
repositories, plus a family of `rc-20260824-*` — sixty created in one
sitting on the evening of 2026-08-24. The scout named eight because those
are the eight whose *original* still sits next to them and can be
diffed; the other fifty-one are copies of repos that were not opened.
**My atlas understated the incident by seven and a half times.**

**The shared quilt core is 28,145 bytes, not 97,486 + 71,216.** The
substance holds — `quilt-arcade`, `quilt-arena`, `quilt-quant`,
`quilt-dba`, `quilt-mesh`, `quilt-learn` and `loom-core` share a
**byte-identical `engine/engine.js` of 28,145 B** — but the exact byte
pair I published **does not reproduce at HEAD** of any of the seven. It
was probably an older commit. *The `quilt-` prefix is a naming family,
not 39 systems* — that stands, and now for a measured reason.

**And it is 14 publishable manifests, not 15.** `SuperInstance/plato-client`
contains **no `package.json` at all** — it is a Python package
(`src/cocapn/__pycache__/client.cpython-310.pyc`,
`dist/cocapn_plato-0.1.0-py3-none-any.whl`). The npm name exists only as
a vendored JS shim inside `cocapn-browser-agent/vendor/`.

---

## 4. What this does to the atlas

The atlas's graduation section is replaced by
[ADR-001-scoped-or-unscoped.md](ADR-001-scoped-or-unscoped.md), which
states the decision as **PENDING and owned by Casey**, with the exact
question to answer:

> **Does Casey hold write access to the npm `@quilt` scope — and
> separately, is the `quilt` GitHub org his to write to?**

Every branch of that answer selects a different destination, and the
interim position is the same in all of them: **no package is published
under any `@quilt/*` or `quilt-*` name until that question is answered.**

The sequencing, from [INTEGRATION-NOTE.md](INTEGRATION-NOTE.md):

1. **`file:` instead of `workspace:*`,** in `quilt-claw`, `quilt-ai` and
   `quilt-evolve` — regardless of what the naming decision turns out to
   be, and regardless of the cycle. Reproduced from scratch: the publish
   CI is not failing at publish, it is failing at `npm install`, before
   the publisher is ever invoked.
2. **Do not break the cycle first.** npm permits cyclic deps between
   separately-published packages; it only bites under a single-workspace
   build or a naive publish-order script. Breaking it now is work that
   may be undone by the naming decision.
3. **Do not attempt Option 1** (publish under the unscoped `quilt-core`).
   It is not a shortcut, it is a trapdoor.

---

## 5. The rule this pass adds

| # | rule |
|---|---|
| 65 | **A package name on npm is not evidence of ownership.** Check the **maintainer**, not the name. `quilt-core@0.11.0` and `quilt-core-os`'s declared name are the same string and two different artifacts, and the version-series overlap was a coincidence of numbering. |
| 66 | **A name can be held several times over, and by people who are not you.** Before designing a graduation path, enumerate *every* holder of the name: the GitHub org, the npm scope, the unscoped package, and any account of that name. Three of them turned out to be holding "quilt". |
| 67 | **A 404 is not always an empty namespace.** `GET /users/<name>` can be 200 while `GET /orgs/<name>` is 404. A real *user* holds the name; you cannot create the org later without resolving the collision. |
| 68 | **When a name is taken, `file:` is the protocol that still works.** npm rewrites `file:` at publish time; `workspace:` cannot install standalone and `*` resolves to a registry that 404s. The fleet already had the right answer inside its own monorepo. |
| 69 | **A count is only as good as the thing it counted.** "Eight backup repos" was eight *diffable* ones out of sixty. "The shared core is 97,486 bytes" was a commit that is not HEAD. Measure the population, not the sample that was convenient to verify. |
