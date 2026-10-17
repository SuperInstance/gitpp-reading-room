# The 29/29 figure was half fiction — and an agent found it

Three subagents ran against the v0.4.6 change. The one building
conformance cases reported something I had to verify before believing:

> **`run.mjs` has a live comparison bug. 25 of the 29 existing cases
> assert nothing.**

## Verified — it is real

`gitpp-fold-conformance/run.mjs:170`:

```js
const strip = (s) => s.replace(/,"has_setters":(true|false)/, "").replace(/,"setters":\{[^}]*\}/, "");
const cmp = expectsSetters ? (a, b) => a === b : strip;
```

`strip` is one-argument. Where a case's `expect` omits `setters`, `cmp(a, b)`
returns `strip(a)` — a truthy string that never inspects `b`. Only 4 of 29
cases declare `setters` (`d26`–`d29`).

**Proven by execution, not by reading.** I replaced `d05`'s expected state,
order, and orphans with nonsense:

```
state={DELIBERATELY_WRONG:1}  order=["NOPE"]  orphans=["BOGUS"]
```

and the harness printed:

```
PASS d05-class-precedence-override-beats-task
```

The identical corruption on `d26` correctly printed `FAIL d26`.

## Why the false green was so durable

`cmp` feeds **both** `matchSpec` and `agreeWithEachOther`. The bug made
both constant-true, so the suite was structurally incapable of reporting
a failure that did not involve `setters`. It printed `29 pass` and would
have printed it no matter what the ports did.

The cross-port agreement it appeared to demonstrate was also destroyed —
not the agreement, which was real, but the *checking of* it. Five ports
agreeing is worth nothing if the line that checks agreement ignores its
second argument.

## Fixed, and the fix exposed a second bug

With the comparison live, `d10` and `d20` failed — their **stored**
`order` was alphabetical, left over from an old normaliser that sorted
away topological order:

| case | stored | Kahn (§5.4) |
|---|---|---|
| `d10` | `[j, l, m, r]` | `[r, l, m, j]` |
| `d20` | `[c01…c10, root]` | `[root, c01…c10]` |

The **ports were right and the expectations were wrong** — the exact
inversion of the usual failure, and only visible because the comparison
finally worked.

Mutation-verified both directions:

| | result |
|---|---|
| `cmp` fix reverted, orders fixed | **29/29** — the false green |
| `cmp` fix in, orders reverted | 27/29 — 2 fail |
| both fixes in | **29/29** |

## A second harness defect, same family

The C wrapper defaulted to a hardcoded `/tmp/gitpp-c-derive` and the
C# wrapper referenced `/tmp/gitpp-cs/src/Gitpp.csproj`. With those paths
absent the suite reported **0/29** — a number that reads as catastrophic
failure rather than "the harness cannot find two of its four ports."

Both now resolve from the environment. Combined with the earlier
`compare.py` false-green in this repo, the pattern is consistent enough
to state as a rule:

**a harness that cannot produce a failure is indistinguishable from a
harness that is passing.** Every number this project reports should be
accompanied by a demonstrated failure, and the way to check that is to
corrupt an input and watch the number move.

## What this says about the earlier 29/29 claim

It was reported in this repo's own history, repeatedly, as evidence that
the fold semantics are pinned. What it actually demonstrated was that
**four ports agreed with each other** — which was true, and still is — and
that 25 cases matched a spec comparison that was not running. Both facts
are now separated:

- cross-port agreement: real, and still 29/29 after the fix
- spec conformance: was unverified for 25 cases, now verified

Nothing in the ports changed. The claim was weaker than it looked, and it
took a build-lane agent looking for unrelated conformance cases to find
it — the same pattern as the RSI-Anything audit, where the second and
third passes found what the first one missed.
