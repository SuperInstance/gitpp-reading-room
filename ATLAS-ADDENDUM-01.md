# ATLAS-01 addendum — the quilt family, and what actually blocks graduation

*2026-10-18. Extends [ATLAS-01.md](ATLAS-01.md) §3c and adds §3f.*

---

## 3c-addendum. The graduation blocker is mechanical, and it is now identified

The atlas said graduation is "blocked on naming and org access." That is
true but it is not the whole thing, and the rest is now measured.

### `@quilt/claw` cannot install

```
$ npm install
npm error code EUNSUPPORTEDPROTOCOL
npm error Unsupported URL Type "workspace:": workspace:*
```

`@quilt/claw` declares:

```json
"dependencies": {
  "@quilt/core":   "workspace:*",
  "@quilt/ai":     "workspace:*",
  "@quilt/evolve": "workspace:*"
}
```

`workspace:*` is a **monorepo-internal** protocol. It resolves inside a
workspace and nowhere else — and `quilt-claw` is a standalone repository
with no workspace root, so there is nothing for it to resolve against.

The consequences are not subtle:

- `npm install` fails outright.
- `npx tsc --noEmit` fails — the type errors are all downstream of the
  unresolvable imports.
- **The 27 test files cannot run.**
- The publish CI cannot succeed.

**Measured across the whole sketchbook: 11 repositories use the
`workspace:` protocol** — 6 unscoped, 3 `@quilt/*`, and 2 vendored
upstreams (`@deepseek-ai/dsh-root`, `@vitejs/vite-monorepo`).

**Three of the eleven are graduation tickets:**

| package | repo | workspace deps |
|---|---|---|
| `@quilt/ai` | `quilt-ai` | 2 |
| `@quilt/claw` | `quilt-claw` | 3 |
| `@quilt/evolve` | `quilt-evolve` | 2 |

### And the dependency graph has a hole at the root, and a cycle

```
@quilt/ai      -> @quilt/evolve (workspace:*)  -> @quilt/ai (workspace:*)   <-- cycle
               -> @quilt/core   (workspace:*)
@quilt/evolve  -> @quilt/core   (workspace:*)
@quilt/claw    -> @quilt/core, @quilt/ai, @quilt/evolve
```

- **`@quilt/ai` and `@quilt/evolve` depend on each other.** Legal in a
  workspace; impossible standalone.
- **`@quilt/core` exists nowhere.** No repository declares that name, and
  it is not on npm (404).

**But the root is not missing — it is misnamed.** The repository
`quilt-core-os` declares:

```json
{ "name": "quilt-core", "version": "0.4.0",
  "description": "Quilt edge runtime — the TypeScript daemon that ships inside the quilt-core snap.",
  "main": "dist/daemon.js",
  "files": ["dist/**", "snap/local-bin/**", "snap/hooks/**", "README.md", "LICENSE"] }
```

and **`quilt-core` is on npm at 0.11.0 with 18 published versions** —
while the repository records `0.4.0`.

So the shape of the blocker is: **the root of the dependency tree exists
and is published, but under an *unscoped* name, while everything that
depends on it wants the *scoped* `@quilt/core`.** One naming decision
separates three graduation tickets from being publishable.

### One published package is installable, and half of it is unloadable

> **CORRECTED.** This subsection first claimed **two** published packages
> were "installable but unloadable". One of them was wrong. The
> correction is more useful than the error was.

Checking what the `files` list actually shipped against what `main`
points at:

| package | `main` | shipped by `files` | result |
|---|---|---|---|
| `motifcode@0.4.3` | `./src/index.ts` | `dist/motif.js`, `dist/build-info.json`, `builtin-plugins` | `require()` → **MODULE_NOT_FOUND**; `import()` → **ERR_MODULE_NOT_FOUND** — the named file is not in the tarball |
| `openroom@0.0.21` | *(absent)* → npm defaults to `index.js` | `dist/`, `README.md`, `LICENSE` | **NOT A DEFECT** — see below |

**`openroom` was my error.** It is a **CLI** package: `type: "module"`,
`bin: { "openroom": "./dist/cli.js" }`, no `main`. A CLI package does not
need a `main`, `require('openroom')` failing is correct behaviour, and
`npx openroom` works — the `.bin/openroom` shim is created on install.
I read "no `main`" as "the entry point is missing" when the manifest
had said all along that the entry point is a *binary*.

**`motifcode` is the real one**, and the shape is sharper than
"installable but unloadable":

```
npm i motifcode       -> "added 1 package, 0 vulnerabilities"
npx motifcode         -> the CLI WORKS (dist/motif.js ships)
require('motifcode')  -> MODULE_NOT_FOUND
import('motifcode')   -> ERR_MODULE_NOT_FOUND
```

**A package with two entry points has two things that can break, and the
obvious smoke test exercises the one that happens to work.** `npm i` plus
`npx motifcode` is green while the library import throws. The install is
the happy path and the *use* is the failure.

**Fixed in the repository** (`SuperInstance/Motifcode4quilt`,
`packages/cli/package.json`): `main` and `exports` now name
`./dist/motif.js` — which `bin` already pointed at and which the build
actually produces. The build was run and works:

```
node scripts/build.mjs
built packages/cli/dist/motif.js (365 KB)
```

**And it is not published, for a measured reason.** The published 0.4.3
ships five built-in plugins — `browser-web-testing`,
`frontend-quality`, `github-workflow`, `library-docs`, `mcp-builder` —
and **none of them exists in that repository**; the repo's `packages/`
has `agents, cli, core, eval, hooks, journal, protocol, replay, skills,
tools, tui` and no `builtin-plugins` directory anywhere. Publishing
0.4.4 from the repo would **remove five plugins from the registry**, so
the manifest fix is committed and the publish is held.

That is the same shape as the luciddreamer recovery, inverted: **the
published artifact (0.4.3) is AHEAD of the repository (0.3.3), and the
repository cannot rebuild what it published.**

The build script's own docstring already names the underlying problem:

> *"Every check that passed — typecheck, tests, lint — ran against
> TypeScript sources through `tsx`, so **nothing anywhere established
> that the thing users would install actually starts**."*

The same is true of the entry point: every check ran against
`src/index.ts`, so nothing established that the thing users would
**import** actually resolves.

**A correction to my own reading, twice over:** these packages carry
`workspace:*` specs, but in **`devDependencies`**, where npm correctly
ignores them for a consumer install. The `workspace:` protocol is
**not** what breaks them; the `main` field is. `workspace:` is a
*repository* problem, not a *published package* problem.

---

## 3f. The quilt family — the live edge of the sketchbook

```
quilt-* repositories                      259
  archived                                    0
  forks                                       3
  own + live                                256
  pushed in 2026-08 / 09 / 10       47 / 146 / 66
```

**Every one of the 259 was pushed in the last three months.** The quilt
family is where the sketchbook is currently growing.

### The largest, by repository size

| repo | size | files | tests | workflows | what it is |
|---|---|---|---|---|---|
| `quilt-gpu-lab` | 403 MB | 4,381 | 14 | 1 | — |
| **`quilt-playtest`** | 58 MB | **9,656** | **278** | 0 | "Engine investigation: 12 playtest patches against…" |
| `quilt-verilog` | 55 MB | 1,613 | 2 | 1 | Quilt in pure Verilog-2005 — 5+1 opcodes |
| **`quilt-crabbox`** | 42 MB | 2,636 | **169** | **20** | "Crabbox: warm a box, sync the diff, run the…" |
| `quilt-fiction` | 32 MB | 594 | 0 | 0 | the quilt as operational fiction |
| `quilt-research-canons` | 30 MB | 406 | 3 | 1 | canonical research artifacts |
| `quilt-cowboy-jev` | 18 MB | 1,955 | 3 | 0 | quilt-cowboy with JEV-Diffusion |
| `quilt-llvm` | 15 MB | 108 | 0 | 0 | a compiler infrastructure where… |
| **`quilt`** | 15 MB | 528 | 12 | 2 | "A spreadsheet where every cell is a live, ad…" |
| `quilt-subleq` | 13 MB | 16 | 0 | 0 | Quilt on Subleq, and Subleq on Quilt |
| `quilt-Countroller` | 11 MB | 37 | 1 | 0 | — |
| `quilt-esp32` | 11 MB | 116 | 0 | 1 | a Quilt reactive runtime for ESP32-class |

**`quilt-playtest` — 9,656 files and 278 test files — is the largest body
of verified work found in the sketchbook so far, and nothing in this
series has looked at it.** `quilt-crabbox` has 20 workflows, which makes
it the most continuously-built repo in the family.

### The `quilt` org

The `quilt` organization exists (38 repositories, created 2019) and holds
an unrelated Ewasm/crypto project — `ewasm-rt`, `turbo-geth`, `sheth`,
`ease`, `cargo-ease`. **None of the 259 `quilt-*` sketchbook repos are in
it.** The namespace and the org do not currently correspond, and the nine
`@quilt/*` npm tickets (ATLAS-01 §3c) are waiting on that being settled
before they can mean anything.

---

## The shape of it, restated

The sketchbook is not a pile of unrelated sketches. It has:

- a **root** (`quilt-core`, published, 18 versions) with a **naming
  mismatch** at the scope boundary;
- a **259-repo family** growing 8–12 repos a day, with 278-test-file and
  20-workflow members;
- a **graduation queue** of 15 packages whose scopes name their
  destination, five of which have a publish CI that cannot succeed;
- and a **dependency graph with a cycle** (`ai` ↔ `evolve`) and a
  monorepo-internal protocol (`workspace:*`) standing where a registry
  spec must go.

None of that is a mess. It is a sketchbook with a shape, and the shape
is now legible enough to pick a thread.
