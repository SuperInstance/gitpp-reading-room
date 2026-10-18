# FINDING-24 — The npm layer: what is installed, what is claimed, what was recoverable

*2026-10-18. Every number here was measured against a live registry, a live
repository, or a rebuilt-from-source tree. Where a thing was proposed rather
than executed, it says so.*

---

## 0. The headline

The npm layer of this fleet was not "a set of broken packages." It was a set
of **claims that had drifted from their artifacts**, in three directions at
once:

1. **Packages whose description promised something the code did not do.**
   Two of them were 264- and 270-byte stubs that implemented none of the
   security properties or hardware access they advertised.
2. **Packages whose source existed nowhere but in their own published
   tarball** — and were therefore one npm garbage-collection away from
   ceasing to exist.
3. **A published package whose `repository` field pointed at a directory
   that no longer existed**, so the provenance chain a maintainer would
   follow to read the source ended in a 404.

And two things that were *live and healthy* turned out to be measured wrong:

4. **The canon service changed the SHAPE of its response** — not the data —
   and four published clients broke silently, reporting an outage that was
   not happening.
5. **A directory publish shipped a different directory's bytes.** The
   version was perfect. The code was from a stale staging tree. Both
   versions were installable. Only one of them was the fix.

The theme is the same one this series keeps arriving at, in a new register:

> **The registry is a claim. The tarball is the artifact. A version number
> proves nothing about the bytes.**

---

## 1. The census, re-measured today

Pulled from the GitHub API at 2026-10-18, `per_page=100`, 53 pages:

```
total repos       5,284
  archived           16
  forks             821
  own + live      4,447
total size         55 MB
```

Growth against the earlier censuses in this series:

| since | repos | delta | rate |
|---|---|---|---|
| 2026-10-01 | 5,113 | — | — |
| 2026-10-09 | 5,208 | +95 / 8 d | 11.9 / day |
| 2026-10-18 | 5,284 | +76 / 9 d | 8.4 / day |

**70 repositories were pushed on 2026-10-18 alone**, 159 in nine days, 340
in seventeen. A census is a snapshot of a moving target, and a two-week-old
census is already ~150 repositories stale.

---

## 2. `SuperInstance` is a **user** account, not an organization

This is a small fact with a large blast radius, because it is wrong in
every prior report in this series, including my own — which repeatedly
called the 5,284-repository fleet "the SuperInstance organization."

Measured:

```
GET /api/orgs/SuperInstance        -> 404 Not Found
GET /api/users/SuperInstance       -> 200
  type: User   login: SuperInstance
  name: Casey Digennaro
  public_repos: 5,279
```

`public_repos` is 5,279 against a 5,284-repo census. The difference of 5
is worth noting on its own: **the REST list and the account's own counter
disagree by five.** An account's `public_repos` is a cached counter; the
paged list is the truth. Anything that trusts the counter over the list is
off by five today and will drift.

**The practical consequence:** any tooling that enumerates the fleet through
the org endpoint sees *nothing*. It returns 404, and a 404 is easy to
mistake for "no repos" rather than "wrong endpoint." The fleet is a user
account.

---

## 3. The branch trap, fleet-wide

Default branches across all 5,284 repos:

```
main                  3,238   (61.3%)
master                2,020   (38.2%)
develop                   5
canary                    2
ci/add-github-actions     2
other                    17
```

**38% of this fleet defaults to `master`.** The `gitpp-*` family is
uniformly `main`, which is exactly why the trap went unnoticed for months:
every tool built during the spec work was developed against repos that
happen to agree with the assumption.

Measured specifically on the **47 npm package repos**: **18 (38%) default to
`master`.** The `cocapn`-era packages are `master`; the `gitpp`-era packages
are `main`.

This is not a footnote. It broke my own package resolver during this
session: `canon-graph` looked *unresolvable* because the resolver asked for
`main` and the repo's default branch is `master`. The package was live on
npm the entire time. The resolver did not report "wrong branch" — it
reported "not found," and I spent time hunting for a package that was
sitting in plain sight.

> **Check the default branch of every repo you touch. A resolver that
> hardcodes `main` will silently skip 38% of this fleet, and it will report
> them as missing rather than as skipped.**

---

## 4. The recovered SDK — the tarball was the only copy

`@superinstance/sdk` was classified as an orphan: no repository on the
account contained its source. Two candidate homes were checked and both
rejected on evidence:

- `SuperInstance/superinstance-sdk` is a **redirect** to
  `SuperInstance/SuperInstance-SDK1` — a different project
  (`superinstance-sdk1` v1.0.0, "a message in a bottle") with
  `src/types.ts` and **none** of the nine modules.
- `SuperInstance-SDK1` is titled `@superinstance/agent-core` and is not on
  npm at all.

The nine modules of `@superinstance/sdk` exist in **no repository on the
account**. The **published tarball was the only copy**: 515,659 bytes
containing the TypeScript sources (`src/*.ts`), the tests (`test/*.ts`),
three examples, `PRODUCT_SPEC.md`, the tsconfig and the lockfile.

Recovered to `SuperInstance/sdk` and verified:

```
npm run build   -> tsc exits 0
npm test        -> 113 tests, 29 suites, 0 failures
clean install   -> 25 exports
                   C = 1.584962500721156   (log2 3)
                   Fleet spawns an agent and reports conservation status
CI              -> success
published       -> @superinstance/sdk@0.4.0
```

**Why this was urgent rather than merely interesting:** npm garbage-collects
packages that nothing depends on, and when it removes one the tarball goes
with it. A tarball that is the only copy of 515 KB of tested source is on a
clock, and this one had been sitting since 2026-06-15.

> **A tarball can be the only copy of a source tree. Recover it to a repo
> before the registry decides it is unused.**

---

## 5. The recovered qgit

`@superinstance/qgit` — 7,934 bytes, the complete 142-line implementation
of 8 Quilt primitives (cell = commit, room = branch, fleet-clock tick).
Published as `0.6.1` from its own tarball; the `repository` field points at
the historical source location.

Research note worth keeping: the researcher classified qgit as
**SUPERSEDED** — `quilt-live-canon` runs the Quilt cell runtime deployed,
and `git-agent` (Python, 2026-10-02) does the git-native premise. The
verdict and the action are not in conflict: **preserve the tarball; do not
re-publish.** A working implementation that is superseded is still the only
copy of that implementation.

---

## 6. The eight orphans, verdicts

Researcher task, read-only, no repo touched and nothing published. The
question was: *is there enough surviving code to rebuild?*

| package | what it actually is | verdict |
|---|---|---|
| `@superinstance/vram-probe` | **a 6-line stub** — `process(input) => ({...input, processed:true, timestamp})`. Reads no GPU, no CUDA, no OOM logic. | **nothing to rebuild** — the description promises GPU VRAM probing and the code does none of it |
| `@superinstance/plato-input-sanitizer` | **the same 6-line stub**, class renamed. Zero PII scrubbing, zero injection filtering — **four claimed security properties, none implemented** | **nothing to rebuild** |
| `@superinstance/plato-neural` | 116 lines, real, but **circular**: 25% of the quality score is a literal count of the words `plato\|tile\|room\|fleet\|cocapn`. Measured: a PLATO answer scores 0.582, "reverse a linked list" 0.369, "capital of France" 0.364 | **not worth rebuilding** — it is a keyword counter, and the fleet has Jev reading the actual document |
| `@superinstance/cocapn-lighthouse` | 14 lines, two in-memory Maps, `port: 8900` stored and **never a listener** | **superseded** by `@superinstance/keeper-beacon` (a strict superset) |
| `@superinstance/plato-mcp-client` | 134 lines of genuinely good MCP client, but 100% thin — all eight tools live server-side | **orphan-backend** — a good client for a dead backend |
| `@superinstance/cocapn-plato-cli` | 17 lines, no `bin`, no arg parsing despite the name — six `fetch` calls to `localhost:8847` | **superseded** by `plato-cli` (Rust) |
| **`@superinstance/sdk`** | **2,224 lines across 12 modules, `dependencies: {}`** | **CANDIDATE -> recovered, section 4** |
| `@superinstance/qgit` | the complete 142-line implementation | **preserved, section 5** |

**One survivor, and it did not need rebuilding — it needed recovering.**

The two stubs are the sharpest result in the table, and they are sharp in a
specific way: **the descriptions were not merely aspirational, they were
false, and the false parts were the interesting parts.** A package called
`plato-input-sanitizer` that does no sanitizing is worse than no package,
because a consumer's security review reads the *name*.

> **A package's DESCRIPTION is a claim; the TARBALL is the evidence. Read
> the code before deciding what to rebuild — and before deciding what to
> trust.**

---

## 7. A live service changed its response **SHAPE**

`/api/canon` used to return a **map** of papers keyed by id:

```json
{ "paper-470.md": { "id": "...", "number": 470, "title": "..." } }
```

It now returns a **list** under a key:

```json
{ "papers": [ { "id": "...", "number": 470, "title": "..." } ] }
```

The data did not change. The **shape** did. And four published clients
read the response with:

```js
Object.keys(payload).length
```

which counts **top-level keys**, not papers. On the new shape that is `2`.
So `canon-graph`'s self-test reported:

```
expected at least 70 papers, got 2
```

**while all 71 papers were sitting there in `payload.papers`.** The
self-test was reporting an outage that was not happening, and it was
reporting it in the exact words an outage would use.

Measured today, three consecutive calls:

```
/api/canon -> 200, 71 papers, 71 papers, 71 papers
```

Fixed and published:

| package | change |
|---|---|
| `@superinstance/canon-graph@0.1.2` | `fetchCanon()` normalises a list, a `papers` payload, or a map into the map `renderGraph()` iterates |
| `@superinstance/canon-claim@0.1.2` | `paperCount()` counts **papers**, whichever shape the service uses |
| `@superinstance/canon-recs` | already handled both shapes — unchanged |
| `@superinstance/canon-suite` | a meta-package that re-exports the other three — inherits the fix |

The self-test also grew an assertion that names the distinction, because a
count of 2 when the canon holds 71 has to be reported as a **client** bug
and not as an empty canon — and to a client that does not normalise, the two
are indistinguishable:

```js
if (!sample || typeof sample.number !== 'number')
  throw new Error('the canon is not a map of papers: a value has no numeric `number`');
```

Clean install, verified end to end:

```
canon-graph -> 71 papers
canon-claim -> 71 papers
graph line   : paper-470 (F161)  F161 — Conservation Laws as Fences: The Physics of Working Animals
shape holds  : yes
```

> **A live service can change the SHAPE of its response without changing its
> DATA. Count the things you mean, not the keys you were handed.**

---

## 8. A publish can ship a **different directory's bytes**

This is the one that cost the most time, and the symptom was completely
silent.

I fixed `canon-claim`'s `paperCount()`, committed, pushed, and published
`0.1.1` from the directory. The published tarball's `index.js` md5 was:

```
bb89ea7416f42c81ed86acfccc977dcb
```

which is **byte-identical** to:

```
/tmp/deadpass/_superinstance_canon-claim/package/index.js
```

— a staging directory left over from an earlier session's dead-package
pass, regenerated at 02:33 the same morning. The version was right. The
bytes were from a different tree entirely. The tarball was also missing
every file the `files` field listed, because the tree it came from did not
have them.

**Two contributors, both real:**

1. **`npm config get cache` returns `/workspace/.home/.npm`.** `/workspace`
   is the over-quota NFS that silently produces zero-byte files and swallows
   writes. npm's cache — and pacote's `tarball:` cache inside it — does not
   behave. The stale entry was sitting right there:
   `pacote:tarball:file:/tmp/deadpass/_superinstance_canon-claim/package`.
2. **The `files` field was silently not honoured**, which was the only
   visible symptom in the tarball contents.

**The procedure that works, and is now the rule:**

> **Never `npm publish` from a directory. `npm pack` first, INSPECT the
> tarball, then `npm publish ./pkg-<name>-<version>.tgz`.**

```bash
npm pack                                        # -> pkg-1.2.3.tgz
tar xzf pkg-1.2.3.tgz
md5sum package/index.js                         # 0da907c128282edf48511df9f0b574d0
npm publish pkg-1.2.3.tgz                       # publish THAT artifact
```

And then **verify the published tarball's bytes, not its version** — a
version bump proves nothing about the content:

```bash
curl -sL "https://registry.npmjs.org/@superinstance/canon-claim/-/canon-claim-0.1.2.tgz" -o c.tgz
tar xzf c.tgz && md5sum package/index.js        # must match the working file
```

**Two more traps from the same session:**

- **`latest` can point at a version whose tarball is not yet downloadable.**
  npm staged 0.1.3, published it, moved `latest` to it — and the tarball
  404'd for minutes, so `npm i @superinstance/canon-claim` failed with
  `404 Not Found`. **A package whose `latest` cannot be downloaded is a
  broken package**, even though every metadata query says it is there.
  The fix used here: point `latest` at the newest version that is both
  downloadable and byte-verified, and deprecate the bad one with a reason.
- **npm's own `npm pack` output is the receipt.** `npm notice Tarball
  Contents` lists every file and its size. The 0.1.1 publish that should
  have carried 5 files carried 2 — and the notice said so, in the output
  that was filtered away.

> **Publish the tarball, not the directory. Verify the published tarball's
> bytes, not its version. And never let npm's cache live under the
> over-quota `/workspace` NFS.**

---

## 9. The live service status, measured

| URL | status |
|---|---|
| `live-canon.superinstance.dev/api/canon` | **200** — 71 papers, stable across 3 calls |
| `live-canon.superinstance.dev/api/canon/papers` | **upstream connect error** — remote connection failure |
| `live-canon.superinstance.dev/api/quilt` | **upstream connect error** |
| `live-canon.superinstance.dev/api/canon/claim` | **upstream connect error** |
| `live-canon.superinstance.dev/api/quilt/verify` | **200 but wrong** — see section 10 |
| `plato-mcp.cocapn.ai/mcp` | **523** on GET *and* on a real `tools/list` POST |
| `api.cocapn.ai/` | **523** |
| `plato.cocapn.ai/` | **no DNS** (000) |
| `cocapn.ai/` | 200 — static marketing page, no API |
| `superinstance.dev/` | **503** |

DNS for `plato-mcp.cocapn.ai` resolves. **The zone exists; the origin is
dead.** That is a downed deployment, not a lapsed domain — and it is
revivable by deploying the live Python `plato-mcp` without writing a line
of client code. Not done here: `CLOUDFLARE_API_TOKEN` is not available in
this environment, and `superinstance.dev` itself returns 503.

---

## 10. The live canon service is half-broken

Two distinct defects, both live, both measured.

**(a) Three of four route families are down.** The catalogue route works
and serves all 71 papers. Every other route returns the same upstream
connect error. The service is reachable; its upstream is not.

**(b) The verify endpoint reports an empty hash as a mismatch.**

```
GET /api/quilt/verify
{
  "reported_hash": "",
  "expected_hash": "0xe435d91d6d92a1d8",
  "byte_exact": false,
  "admitted_to_canon": false,
  "known_port": false,
  "verdict": "FAIL Hash mismatch. Expected 0xe435d91d6d92a1d8, got (empty). The port is NOT byte-exact."
}
```

`reported_hash` is **empty, not mismatched.** A field whose entire job is
to carry a hash carries nothing, and the check reports the absence as a
hash mismatch — the verdict even says *"(empty)"* while classifying it as
"Hash mismatch."

**And the service disagrees with its own README**, which advertises
`0x445185a3a99fd2e7` while the service's `expected_hash` is
`0xe435d91d6d92a1d8` — two different hashes in two places that are
supposed to be the same claim.

This is the fleet's recurring shape, one level up from where it has
appeared before: **a field that should carry a value carries nothing, and
the check calls it a mismatch instead of an absence.** Whether the drift is
intentional was not determined.

---

## 11. What is still open, and named

- **The live canon upstream is down.** `/api/canon/papers`, `/api/quilt`
  and `/api/canon/claim` all return "upstream connect error" while
  `/api/canon` serves 71 papers. Not diagnosed — this is the service's
  backend, not the clients.
- **`/api/quilt/verify` reports `reported_hash: ""`.** The field carries
  nothing and the check reports it as a mismatch. The README's hash and
  the service's `expected_hash` are also different values.
- **`plato-mcp.cocapn.ai` is 523 with a live client pointed at it.**
  Revivable by deployment; `CLOUDFLARE_API_TOKEN` is not available here.
- **`superinstance.dev` returns 503.**
- **`@superinstance/canon-claim@0.1.3`** is published and carries the
  correct bytes, but was still staging at the time of writing and is not
  `latest`. `latest` is `0.1.2`, which is downloadable and byte-verified.
  `0.1.1` is deprecated with a stated reason.
- **The fleet census drifts ~8–12 repos/day.** Any count in this document
  is a snapshot and should be read as one.

---

## 12. The rules this pass added

| # | rule |
|---|---|
| 32 | A package's **description** is a claim; the **tarball** is the evidence. Read the code before deciding what to rebuild — or to trust. |
| 33 | A tarball can be the **only copy** of a source tree. Recover it to a repo before the registry garbage-collects it. |
| 34 | A live service can change the **shape** of its response without changing its data. Count the things you mean, not the keys you were handed. |
| 35 | Check the **default branch** of every repo you touch. 38% of this fleet is `master`, and a resolver that assumes `main` reports them as missing. |
| 36 | **Publish the tarball, not the directory.** `npm pack` -> inspect -> `npm publish ./pkg.tgz`. |
| 37 | **Verify the published tarball's bytes**, not its version. |
| 38 | **`latest` pointing at a staged version is a broken install.** Verify the tarball downloads. |
| 39 | **Never let npm's cache live under the over-quota `/workspace` NFS.** It is silently write-broken. |
| 40 | **Put a commit message in a FILE**, never in a double-quoted `-m`. A backtick starts a command substitution. |
