# CORRECTION — the package census was 3.6× overstated, and the cause will bite the next census

**Date:** 2026-10-18
**Raised by:** an independent re-measurement of the package census
**Applies to:** the figures quoted in the census PR body, in
`SuperInstance/quilt-ci`'s guard README, and in the `receipts` slide deck

---

## What was claimed

> **131 versions (28.8%)** of the 455 checked declare an entry point the
> package does not ship, and **12 (7.5%)** have a `require()` that provably
> throws.

## What was measured

| | claimed | measured |
|---|---|---|
| versions declaring an entry point they do not ship | **131 (28.8%)** | **36 (8.8%)** — **3.6× overstated** |
| versions whose `require()` throws | 12 (7.5%) | **40 packages** genuinely fail to load, across 15 packages; the other 37 "failures" were the audit's own harness **not running `npm install`** |
| versions shipping no `types` | 18 | **63** declare a `types` they do not ship; **142** ship none at all |

## THE CAUSE, and it is the part worth keeping

**npm has TWO packuments, and the abbreviated one is a trap.**

```
Accept: application/vnd.npm.install-v1+json   <- the ABBREVIATED one
```

**The abbreviated packument silently strips `main`, `types`, `exports` and
`files`.** So every package fetched that way looks like it is missing its
entry point. The 131 is an artefact of the transport, not a fact about the
packages.

**The same audit's own first pass returned 329 phantoms (73%) before it
switched to full packuments.** So this is not a mistake only I made — it is
the *default* behaviour of the easy query, and it is a mistake that always
looks like a dramatic finding.

> **The abbreviated packument cannot answer a question about entry points,
> because it does not carry them.** Read the FULL packument, or read the
> tarball. The tarball is the artefact the consumer gets, and it is the
> only honest witness.

## The thesis survives, and it is slightly stronger

**No package here tests the artifact it publishes.** That is unchanged. The
arithmetic was wrong, and a census whose whole job is to be re-derivable
has to have the arithmetic right — the more so because the two errors
pointed in OPPOSITE directions, and a reader who saw only one of them
would have drawn the wrong conclusion.

## Three other findings from the same re-measurement

**1. The obvious fix can break the build, and the tests will not notice.**

The naive repair for the known family (`rootDir: "."` + `include: [src,
test]` → emits `dist/src/index.js` while the manifest points at
`dist/index.js`) is to set `"rootDir": "./src"`. That fails three ways:
`TS6059`, a build that **exits 2** (which *blocks* the publish), and —
the important one — **`npm test` still exits 0 with `# tests 0`.**

**A test script that reports pass for an empty run is the same defect one
level down.** The build error is not the failure; the suite that cannot
tell it is empty is.

**2. A second cause nobody had flagged.** `dist/` is gitignored, with no
`.npmignore` and no `files` — so `quipu-math@1.0.2` **shipped no compiled
output at all**, just `src/index.ts`.

**3. Two of the three guard's own defects would have made adoption
catastrophic.** The `--strict` arm referenced an **undeclared variable**,
so every strict run raised a `ReferenceError` before the probe started and
failed on **every package, healthy ones included** — and the suite never
passed `--strict`, so nothing noticed. Separately, the guard's ignore list
excluded `dist/`, which **npm ships**, so it false-failed healthy packages.
Both are fixed in `SuperInstance/quilt-ci#3`, with tests.

## And the traps for whoever fixes the rest

- The `repository` field lies for **three** packages, not one:
  `adinkra-math` points at a **C** library, `kintsugi-math` and
  `quipu-math` at **Python** repos. **Resolve the source by looking for
  the repo that actually has a `package.json`.**
- `palaver-math` and `adinkra-math-npm` have a `package.json` **only on
  `master`**.
- `kintsugi-math-npm` and `quipu-math-npm` are both at `1.0.1` in-repo
  while `1.0.2` is published — **bump to 1.0.3 or you collide.**
- Two of the flagged packages were already fixed and were taken OFF the
  list: `quilt-sdk` (0.2.2+) and `quilt-core@0.3.0` (fixed since 0.3.1).
  **A wrong fix is worse than no fix.**
