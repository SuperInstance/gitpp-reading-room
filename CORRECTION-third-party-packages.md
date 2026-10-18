# Correction: `motifcode` was never ours, and 36 of the published packages are other people's

*2026-10-18. A third correction to my own audit, found by the scout's
preservation sweep. Two of my findings were about **strangers' packages**
that I had attributed to the fleet.*

---

## 1. The `motifcode` finding was not a fleet finding

I published, in `ATLAS-01` and its addendum, that a package "installs cleanly
and then fails at `require()`":

> `motifcode@0.4.3` — `main: ./src/index.ts`, `files: ["dist/motif.js", …]`.
> `npm i motifcode` reports "added 1 package, 0 vulnerabilities" — and then
> `require('motifcode')` throws `MODULE_NOT_FOUND`.

**The measurements are all true. The subject is not ours.**

```
$ curl -s https://registry.npmjs.org/motifcode | jq '.versions["0.4.3"].repository'
git+https://github.com/TaewoooPark/Motifcode.git
$ ... | jq '.maintainers'
[{"name":"taewooopark"}]
```

**`motifcode` is `TaewoooPark/Motifcode`.** Its five `builtin-plugins` —
`browser-web-testing`, `frontend-quality`, `github-workflow`,
`library-docs`, `mcp-builder` — are **his**. The `main` field that names a
file the tarball does not ship is **his** defect.

**What that means for the work I did today:**

- **Nothing was lost.** I spent a scout's whole first job hunting for five
  "lost" plugins. They were never lost and never ours; the repository field
  had been pointing at their owner the whole time.
- **`SuperInstance/Motifcode4quilt` is Casey's own repository**, declaring
  `"name": "motifcode"` — a name he cannot publish under, because it is
  taken. That is a **namespace collision**, not a broken manifest, and it
  belongs in the same family as `quilt-core`: a name somebody else holds.
- The commit I made there today is still a correct improvement to *that
  repository* — but it was never going to ship, and I described it as
  though it would.

**And the general shape of the mistake:** I classified a package as "the
fleet's" because a repository in the account *declared its name*, without
asking who actually publishes it. The declaration and the publication are
different facts.

## 2. Thirty-six of the published packages are other people's, and that is provenance working

Across the 204 published packages the scout could resolve a repository for:

| repository field points at | count | what it is |
|---|---|---|
| `SuperInstance/*` | 146 | ours |
| **someone else** | **36** | **a vendored upstream — the field is CORRECT** |
| unresolvable | 22 | 11 genuinely have no field; the rest use npm's **short-form** `repository` (`"SuperInstance/polln"`, no URL), which my regex missed |

The 36 are not defects. `jam` → `mjackson/jam`, `makerlog` →
`aniftyco/makerlog`, `podcast-ai` → `spyshiv/podcast-ai`, `quilt` →
`ooyala/quilt`, `taskflow` → `intuitivcloud/taskflow`, `motifcode` →
`TaewoooPark/Motifcode`. **A vendored copy that points at its upstream is
provenance doing its job.** My audit framed "no repository field" and
"repository points elsewhere" as things to fix; for most of them the
correct reading is that they are *someone else's code, correctly attributed*.

## 3. The short-form `repository` field is legal npm and my regex missed it

npm accepts `"repository": "SuperInstance/polln"` — a bare `owner/name`, no
URL. My audit required a URL containing `github.com`, so every such
package fell into the "no repository field" bucket.

**Eleven of the 22 unresolvable entries are this.** The field is present
and correct; my parser was wrong.

## 4. The rules this pass adds

| # | rule |
|---|---|
| 72 | **A declared package name is not a claim of ownership.** Check the registry's `maintainers` before attributing a published package to the account. A repository that declares someone else's name is a namespace collision, not a broken manifest. |
| 73 | **A vendored upstream whose `repository` field points at its author is provenance WORKING.** Do not "fix" it. Only an unresolvable or *wrong* target is a defect. |
| 74 | **npm's `repository` field has a short form** — `"owner/name"`, no URL. Parse both, or you will classify every short-form package as having no provenance. |
| 75 | **Before hunting for lost content, check who owns it.** A five-package search for "lost" plugins found them in the publisher's own repo, where the `repository` field had said so all along. |
