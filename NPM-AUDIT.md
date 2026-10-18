# NPM-AUDIT — the published surface, measured

*2026-10-18. Companion to [FINDING-24.md](FINDING-24.md), which tells the
story; this file is the data.*

> **REFRAMED 2026-10-18 — see [ATLAS-01.md](ATLAS-01.md).**
> `SuperInstance` is a sketchbook, not a production fleet, and this audit
> was first written against the wrong model. Two consequences:
>
> 1. **"248 packages are declared publishable and absent from the
>    registry" is not a defect list.** A sketch that was never published
>    is a sketch. What §2 actually produced is a *map* of the published
>    boundary, and the atlas is the right shape for it.
> 2. **"The 19 whose CI runs `npm publish`" are not broken publish
>    paths.** They are **graduation attempts** — the scope names the
>    destination organization, the publish step is the attempt to leave
>    the sketchbook, and the package never arrives because the
>    destination org is not ready. Five of them are `@quilt/*` and
>    `@luciddreamer/*`; the `quilt` org exists but holds an unrelated
>    2019 Ewasm project. That is a naming and org-access decision, not
>    a code defect. See ATLAS-01 §3c.
>
> The **data** below is unchanged and is the measurement. The **frame**
> was wrong, and the frame is what turned a map into a backlog.

Method: the package list came from two independent sources, because
neither is complete on its own.

1. **Every repository's `package.json`**, via the GitHub GraphQL API,
   `object(expression: "HEAD:package.json")` over all 5,284 repos.
2. **npm's search index** — which turned out to be *missing* packages
   that are demonstrably published (`gitpp-js`, `gitpp-cs`, `gitpp-pl`,
   `gitpp-c`, `gitpp-cross-port`, `substrate-vectors` were all absent
   from it).

Union: **573 package names.** npm search alone found 117.

---

## 1. The shape of the fleet

```
repos                                   5,284
  default branch main                   3,238   (61.3%)
  default branch master                 2,020   (38.2%)
  archived                                  16
  forks                                    821
  own + live                            4,447
  pushed on 2026-10-18 alone                70
```

`SuperInstance` is a **user** account. `GET /api/orgs/SuperInstance`
returns **404**; `GET /api/users/SuperInstance` returns 200 with
`public_repos: 5,279` — itself five short of the paged list.

---

## 2. Publish state of the 573 declared names

| | count |
|---|---|
| confirmed on npm | **208** |
| **404 on the registry** | **365** |

Of the 540 repos that declare a package name:

| `private` field | repos | of those, not on npm |
|---|---|---|
| `true` | 152 | 119 (correct — not meant to be published) |
| `false` | 8 | **2** |
| absent | 357 | **246** |

**248 packages are publishable by default and are absent from the
registry.** npm's default for a missing `private` field is publishable,
so these are packages the repos *say* are publishable and that no
consumer can install.

### The 19 whose CI runs `npm publish` and whose package is not on npm

These are the sharpest of the 248: the repository's own workflow tries to
publish a package the registry does not have. Something in that path is
broken — a wrong name, a wrong scope, a token that lacks permission, or
a publish that failed and was never noticed.

```
@superinstance/symphony-runtime          repo=symphony-runtime           1,019 files
@superinstance/dodecet-encoder           repo=dodecet-encoder             133 files
@superinstance/cascade-router            repo=CascadeRouter                51 files
@superinstance/composite-headspace       repo=composite-headspace          40 files
@superinstance/constraint-tminus-bridge  repo=constraint-tminus-bridge     21 files
@superinstance/fleet-bridge              repo=fleet-bridge                 23 files
@superinstance/i2i-bottle-agent          repo=i2i-bottle-agent             18 files
@superinstance/plato-agent-connect       repo=plato-agent-connect           6 files
@superinstance/ui                        repo=UI                           14 files
@quilt/fleet                             repo=quilt-fleet                  65 files
@quilt/pincher                           repo=quilt-pincher                44 files
@luciddreamer/agent-grid                 repo=agent-grid                   27 files
@luciddreamer/cost-analysis              repo=cost-analysis                17 files
@luciddreamer/monitoring-dashboard       repo=monitoring-dashboard         17 files
dscode                                   repo=dscode                      455 files
jeviter                                  repo=jeviter                      53 files
constraint-flow                          repo=constraint-flow              47 files
agent-harness-generator                  repo=agent-harness-generator     883 files  (private:true)
ordewell-monorepo                       repo=ordewell4quilt              537 files  (private:true)
```

The other 238 have no publishing CI at all — they are not "broken
publishes", they are repos that carry a package.json and were never meant
to go out under that name. That is a legitimate state and is **not**
counted against them.

---

## 3. `repository` field health, for the 208 on npm

| | count |
|---|---|
| points at a GitHub repo | 191 |
| points somewhere else | 2 |
| **no `repository` field at all** | **11** |

The eleven:

```
@superinstance/cargo-line-tycoon-substrate   codex-monorepo      coev
fleet-dashboard                               git-agent           git-storage
murmur                                        open-webui          paperclip
si-runtime                                    superpowers
```

**Ten of them have an obvious home** — a repository declares the exact
same package name. But **eight of those ten have a version that does not
match the repository's**, and inspection shows why the mismatch is not a
bug to be corrected:

| package | npm version | repo version | what the npm tarball actually is |
|---|---|---|---|
| `open-webui` | 0.1.125 | 0.9.6 | 316 files — the real open-webui source |
| `paperclip` | **20.5.0** | — | **1 file: `package.json`. Nothing else.** |
| `superpowers` | 0.0.2 | 5.0.6 | 3 files — `lib/superpowers.js` |
| `coev` | 1.0.0 | 0.1.0 | 5 files — a webhook client |
| `murmur` | 0.0.2 | 1.0.0 | 5 files — Murmur3 for Node |
| `git-agent` | 1.0.2 | 2.0.0 | 68 files — a real package |
| `si-runtime` | 0.0.0 | 0.1.0 | 453 B |
| `codex-monorepo` | 1.0.0 | — | 232 B |

**So "no repository field" is sometimes the correct state.** `paperclip`
and `superpowers` are published under a name that a repository happens to
also use, but at versions the repository has long moved past, and their
tarballs are a different lineage. Repointing them would create a false
provenance claim — the exact error this audit exists to prevent. They are
left alone, deliberately, and the reason is recorded here.

`@superinstance/cargo-line-tycoon-substrate` is the only one of the
eleven with **no** repository declaring that name at all.

---

## 4. The empty-export class

Scanning every published tarball for a JavaScript entry point whose
entire body is an empty export:

**6 packages export nothing at all.** (A 7th, `git-agent`, was a false
positive: it has 68 files and 22 real modules, and the scan happened to
read an empty comment-only type stub first.)

```
@superinstance/plato-room-nav          v0.1.2   "Spatial routing and exit resolution for PLATO rooms"
@superinstance/plato-fleet-graph       v0.1.2   "Inter-agent relationship mapping and fleet topology analysis"
@superinstance/plato-tile-validate     v0.1.2   "Schema checking and deadband gates for PLATO tile quality"
@superinstance/plato-semantic-sim      v0.1.2   "Embedding-based tile comparison and deduplication for PLATO rooms"
@superinstance/plato-inference-runtime v0.1.2   "Lightweight tile scoring and Q&A evaluation for PLATO rooms"
mycelium-ai                           v0.0.1   "Self-improving cognitive memory engine with ..."
```

All five `plato-*` packages have the **identical** body:

```js
module.exports = {};
```

21 bytes. Normalised, all five hash to the same md5 (`3fefec45`). They
even ship a matching `index.d.ts`:

```ts
export {};
```

**Five packages. Five distinct, specific capability descriptions. One
empty object.**

### But the capability is real — it is just not in JavaScript

`SuperInstance/plato-room-nav` contains:

```
src/lib.rs                    13,055 B   16 public functions
src/plato_room_nav/                       a Python package
dist/plato_room_nav-0.1.0-py3-none-any.whl
Cargo.toml      Cargo.lock      pyproject.toml      ARCHITECTURE.md
```

and `src/lib.rs` opens:

> *Room navigation and pathfinding engine. Finds shortest routes between
> rooms, discovers connected rooms, and provides spatial awareness for
> PLATO agents.*

**which is precisely what the npm package's description promises.** The
same holds for the other four.

So the finding is not "these capabilities were never built". It is:

> **The capability exists, in Rust and Python, in the repository — and
> the npm package that advertises it installs and exports nothing at all.**

A consumer who installs `@superinstance/plato-room-nav`, finds
`module.exports = {}`, and concludes the feature does not exist. The
evidence that it does exists is in a file the npm tarball does not carry
and the `repository` field does point to.

### The wider stub family

Every published package that unpacks to under 2 KB was listed and
inspected. Thirty-four qualify. Besides the six above, they are mostly
one-function packages (`cocapn-lighthouse` 752 B, `keeper-beacon` 893 B,
`flywheel-engine` 930 B, `barracks` 1,006 B) and legitimate
meta-packages (`canon-suite`, 1,600 B, which re-exports three other
packages). The five `plato-*` entries are the only ones that are
**identical to each other**.

---

## 5. Tarballs that hold source the repository does not

Every published tarball was compared against its repository's tree,
matching on path and then on basename-and-size. Eight packages have
source in the tarball that the repository does not contain:

| package | missing from the repo | tarball size |
|---|---|---|
| **`@superinstance/luciddreamer`** | **11 Python modules + index.js — the repo is luciddreamer.ai, a 3-file marketing site** | **347,624 B** |
| `@superinstance/cocapn-vessel` | 9 files incl. `src/worker.ts` (32,868 B) | 481,983 B |
| `@superinstance/deadband` | 4 Python modules + index.js/d.ts | 22,627 B |
| `@superinstance/cocapn-plato` | index.js/d.ts + an example | 49,738 B |
| `@superinstance/polyvocoder` | 3 TypeScript files | 31,679 B |
| `@superinstance/jev-diffusion` | index.js + test/test.js | 24,199 B |
| `@superinstance/madlibs-gan` | index.js | 20,094 B |
| `@superinstance/peanut-gallery` | index.js | 16,580 B |

**`@superinstance/luciddreamer` was recovered** — see
[RECOVERY-LUCIDDREAMER.md](RECOVERY-LUCIDDREAMER.md).

The other seven are **not** recovered, and the reason is recorded rather
than papered over:

- `cocapn-plato` and `deadband` — the repository *does* contain the same
  modules at a different path with different content. The tarball is a
  different lineage, not a missing copy. Recovering it would have
  **regressed** the repository. The versions show it: `deadband`'s repo
  says `0.2.0` while the tarball is `0.3.1` — the tarball is *newer*,
  so this is a version-drift question, not a recovery question, and it
  needs a decision rather than a copy.
- `cocapn-vessel`, `polyvocoder`, `jev-diffusion`, `madlibs-gan`,
  `peanut-gallery` — the tarball and the repository are different
  lineages with the same name. Same conclusion: named, not acted on.

---

## 6. The publish trap, reproduced

`@superinstance/canon-claim@0.1.1` was published from a directory and
its `index.js` was **byte-identical** to
`/tmp/deadpass/_superinstance_canon-claim/package/index.js` — a staging
directory from an earlier session, regenerated the same morning.

Root cause: `npm config get cache` returns `/workspace/.home/.npm`, the
over-quota NFS that silently produces zero-byte files. npm's cache, and
pacote's `tarball:` cache inside it, does not behave there.

**The procedure that works, and is now the rule:**

```
npm pack                                  # -> pkg-1.2.3.tgz
tar xzf pkg-1.2.3.tgz                     # inspect it
npm publish pkg-1.2.3.tgz                 # publish THAT artifact
curl -sL <the registry tarball> -o c.tgz  # then verify the BYTES
```

Two more measured traps:

- **`latest` can point at a version whose tarball is not yet
  downloadable.** A package whose `latest` cannot be installed is a
  broken package, however healthy the metadata looks. `canon-claim`
  needed `latest` moved back to the newest version that was both
  downloadable and byte-verified.
- **Publishing a tarball takes noticeably longer to become downloadable
  than publishing a directory.** `luciddreamer@0.1.2` was published from
  a tarball and took over four minutes to propagate; `canon-claim@0.1.2`,
  published from a directory, was live in under a minute.

---

## 6a. Correction: the canon shape narrative

`FINDING-24.md` §7 originally said the Live Canon service *changed* the
shape of `/api/canon`'s response. **It did not.** The service has
returned `{papers:[...], count:N}` since its earliest commit
(`3c230ebd`, 2026-09-08), and the clients were written on 2026-09-04 —
four days before the service existed — against a shape it never had.

The fix that shipped is still correct; the *reason* changed. And the
correction turned up something larger: **`/api/canon/claim` and
`/api/canon/drill` have never existed either**
(`grep -cE "claim|drill" worker.js` → 0), so `@superinstance/canon-claim`'s
two headline functions — the ones the package is named for — have never
worked against the live service, and its old self-test expected
`claim('trust ladder')` to return F168.

Full write-up: [CORRECTION-canon-shape.md](CORRECTION-canon-shape.md).

## 7. What this audit did NOT do

- It did not publish any of the 248 declared-publishable packages. That
  is 248 decisions about scope, name, and whether the code is finished —
  a day's work, and a call for the owner, not an auditor.
- It did not repoint the eight version-mismatched packages. Doing so
  would assert a provenance the bytes do not support.
- It did not recover the seven remaining tarball-only sources, for the
  reasons in §5.
- The 365 registry 404s were confirmed by direct lookup, but **a 404
  does not distinguish "never published" from "garbage-collected"**. npm
  gives no way to ask. The 19 in §2 with publishing CI are the ones
  where the distinction is provable from the repository side.

---

## 8. Rules this audit added

| # | rule |
|---|---|
| 41 | **A package's exports are the claim.** Five packages advertised five distinct capabilities and all five shipped `module.exports = {};`. |
| 42 | **The capability may exist in another language.** The `plato-*` repos hold the Rust and Python; the npm bridge exported nothing. The fix is not to declare the capability absent. |
| 43 | **An empty `repository` field is sometimes correct.** Eight of the eleven were version-mismatched lineages; repointing them would be a false provenance claim. |
| 44 | **A package name that 404s may never have been published, or may have been garbage-collected.** npm cannot tell you which. A repository whose CI runs `npm publish` is the only place the difference shows. |
| 45 | **A tarball is the only copy of its source often enough to build a routine around it.** Scan every tarball against its repo tree; the mismatches are the recoveries. |
| 46 | **npm's search index is not a package list.** It was missing six packages that are demonstrably published. Take the union of the registry and the repos. |
