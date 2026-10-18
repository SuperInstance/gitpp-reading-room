# CORRECTION — my TypeScript 7 story was imprecise, and a census found the real mechanism

**Date:** 2026-10-18
**Raised by:** `research/TS7-CENSUS.md` — a **complete** census, 5,298 of 5,298 repos
**Status:** the finding stands; my *explanation* of it did not

---

## What I said

In `AI-Writings#86` and in this repo's memory I wrote:

> *"The repo pins `typescript: ^7.0.2` in devDependencies. **TypeScript 7
> REMOVED the `node10` module resolution**, which `"moduleResolution":
> "node"` aliases to."*

**The census, having run real `tsc` 7.0.2 against every affected config,
says the precise version is different — and that the mechanism I named was
never the mechanism that bit anyone.**

## What the census found

**5,298 of 5,298 repos scanned. A complete census, not a sample** (53 REST
calls + 27 GraphQL calls).

| | count |
|---|---|
| repos shipping a root `tsconfig.json` | 275 |
| nested configs, in 142 more repos | 305 |
| **configs using a value TS 7 rejects** | **169** |
| **ARMED** — `typescript` ≥ 7 resolvable *today* | **3** |
| latent (pinned below 7) | 165 |
| indeterminate | 1 |

**The three armed ones are all first-party, and all verified by running
real `tsc` 7.0.2:** `SuperInstance-papers` and `quilt-fleet` have **red
CI**; `fabric-mcp` has **no checks at all**.

## The three things that changed the answer

### 1. Both of my instances were already fixed at HEAD — and git says why

In `quilt-evolve`, the TS 7 bump and the config fix landed in **one
commit** (`6734a96`, *"un-break the build"*). **No commit ever had `^7.0.2`
and `moduleResolution: node` together.** The broken state existed only in a
**working tree** — invisible to any HEAD census, and invisible to
`git bisect`.

**So the defect I found was real, but it was never committed.** The
commit-message claim *"TypeScript 7 removed the node10 module resolution"*
describes a version boundary that no repo in the fleet was actually on.

**And the `node16` fix I shipped is still correct** — the census says so
explicitly.

### 2. The real invisibility mechanism is not the config at all

`continue-on-error: true` on the **build** step in `SuperInstance-papers`
produces exactly my signature — **build: SUCCESS, test: FAILURE** — by
**swallowing the build failure**.

And `quilt-core-os`'s own template carries:

```
npx tsc --noEmit || echo "no tsc config found"
```

**A type-check gate that cannot fail.** The fleet's own template, in the
fleet's own idiom.

**And `ignoreDeprecations: "6.0"` — which I quoted as part of the problem
— causes nothing, and 0 repos in the fleet carry it.**

### 3. "It builds for me" is now resolved, and it is a new defect class

> **TypeScript 7 emits all 32 output files AND exits 2.**

**The artifacts are correct. Only the exit code betrays the failure.** A
human and an IDE see a working build; **CI sees the failure.** That is why
the bug was so hard to see, and it is a distinct shape from everything else
in this audit: **a build that succeeds and reports failure.**

## Two more corrections to the story

- **`{node, node10, classic}` is exactly the failing set — but all 38
  use the `node` spelling.** **Grepping for `node10` would have found
  ZERO.** A census that greps for the memorable token would have missed
  every instance.
- **`node --test test/` is genuinely broken but there are 0 instances.**
  However, **both glob forms silently pass with zero tests, affecting 58
  repos.** My earlier claim that this shape was "already at zero" was
  about the wrong variant.

## And two explicit exclusions, not zeroes

- `MCPMempool-quilt`'s tsconfig is **`{{`-doubled braces** — unparseable.
- `superinstance-agent`'s package.json is **missing a comma** — it
  **cannot install at all.**

**And 28 repos ship a tsconfig with NO `typescript` dependency**, depending
on an ambient `tsc` — **the exact condition that makes local and CI
disagree.**

## The coverage gaps the census names itself

HEAD-only; tsconfigs deeper than two levels or with non-standard filenames
were not probed; `supabase`'s `catalog:` spec is indeterminate; and CI
status was read but **not the logs**, so the census cannot say *why* the
other 12 red repos fail.

**A census that names its gaps is worth more than one that rounds them off.**

## The corrected rules

1. **A commit message that explains a version boundary is a claim, not a
   finding.** Run the compiler at that version and say what it does.
2. **The mechanism of invisibility is often not the thing you fixed.**
   Here it was `continue-on-error` on the build step, and a type-check
   gate written as `tsc || echo "not found"`.
3. **A build that emits correct artifacts and exits non-zero is a
   distinct defect class** — humans and CI will disagree about whether it
   worked, and the artifacts will look fine.
4. **Grep for the SPELLING, not the memorable token.** All 38 instances
   say `node`; none says `node10`.
5. **A defect that only ever existed in a working tree is invisible to
   git, to bisect, and to a HEAD census — and it is still a real defect
   that will come back the next time someone types the command.**
