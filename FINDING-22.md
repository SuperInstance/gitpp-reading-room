# A mutation corpus that finds UNLANDED fixes, and a harness that was testing a snapshot

The second half of this session ran the mutation corpus for real — all
131,499 mutants, five ports — and it found two things nobody had
reported. One of them was a bug. The other was a bug in the *harness*.

## 1. The mutation pass was testing a snapshot, not the port

`gitpp-cross-port` carries `CLleaf.c`: a leaf-only wrapper for the C
port. Its header said:

> Self-contained, no link to gitpp.c.

And it was telling the truth. `CLleaf.c` was **345 lines** of SHA-256,
JSON parser and canonicaliser, forked from `gitpp-c` at some point in
the past and never updated again.

The consequence is worse than a stale test fixture:

> The mutation corpus exists to be **independent of the corpus
> author's imagination**. A frozen copy satisfies neither half of that:
> it is not the port, and it is not fresh either.

Two things followed, and they point in opposite directions:

- The `escape-injection` class reported **14,611 disagreements**. That
  was the *copy* being stale — the C port's escape fix
  (`gitpp-c#4`) had already landed, and the harness was still testing a
  snapshot from before it.
- And meanwhile **the real port's leaf was never exercised at all**, so
  a bug in it would have been invisible. The pass that exists to be
  independent was testing the very thing it was supposed to be
  independent *of*.

The fix is to stop copying. The C port already supports being included
— its own test suite uses exactly that pattern:

```c
#define main gitpp_main_never_called
#include "../gitpp-c/gitpp.c"
#undef main
```

so `CLleaf.c` now does that and drives the port's own
`parse_record/1` and `record_leaf/2`. **345 lines became 74**, and
every fix to the port now arrives here automatically.

Measured, over the first 250 mutants:

| | agree | disagree |
|---|---|---|
| the stale copy | 222 | **28** |
| the real port | 250 | **0** |

> **A fix to a port cannot be missed by a copy of it — which is the
> whole point of not having a copy.**

## 2. The corpus found a fix that was written, reviewed, and never landed

With the copy fixed, every remaining disagreement was the Perl port's,
and all of them were the `escape-injection` class: 14,611 of them.

The cause was `gitpp-pl`'s `_string()`, which escaped the seven C0
controls that have a two-character short form and let the **other
eighteen** through RAW — a byte no other port emits, and one that is
not legal JSON at all.

That defect was diagnosed and fixed **months ago**, on a branch, with a
74-assertion regression suite, correct against the reference:

```
{"id":"a","kind":"task","parents":[],"effects":{"c":"a\u0001b"},"reason":null}
  this port, before : 2059a2b499ab997719e1e00f379f2fffe895c1ea7a865b22a8c80adb1a39191d
  reference (doctor) : f178bc60f931c55d98f085f5afc6a30fa0f86ef7e79545b535859b7201a43c19
```

**No PR was ever opened.** So `main` kept the bug, and the mutation
corpus kept reporting 14,611 disagreements — and the report read as a
*live defect* rather than as a *fix that never landed*.

> **A mutation corpus does not only find bugs. It finds unlanded
> fixes.**

A corpus reporting a class of disagreement, and a branch that already
fixes it, are the same fact seen from two sides. That is worth
remembering whenever a mutation report comes back red: the first
question is not "which port is wrong", it is **"has somebody already
fixed this and not landed it?"**

## 3. The sixth port: three more spec violations

The Elixir port (`patchwork-quilt/gitpp-beam`) was added as the fleet's
only *independent* implementation, and it found three defects in
itself that the other five ports do not have.

### An orphan's descendants were auto-orphaned, and the effect was silently lost

§5.5 says two things that are easy to get backwards:

> "A record is an orphan if any of its parents is not in the applied set."
> "The orphan's descendants are NOT auto-orphaned (the fold runs on what stands)."

No single rule satisfies both:

| rule | what it gets wrong |
|---|---|
| A — "any parent id is missing from the set" | WRONG on an orphan's descendants |
| B — "anything Kahn never emitted" | WRONG on an orphan's descendants |
| U3 — all three clauses together | correct on both |

The port implemented B, and it was the worst kind of bug — it did not
crash, it returned a **confident, well-formed, wrong answer**:

```json
[{"id":"orph","kind":"task","parents":["ghost"],"effects":{"y":99}},
 {"id":"child","kind":"task","parents":["orph"],"effects":{"z":7}}]
```

| | `order` | `orphans` | `state` |
|---|---|---|---|
| py, js, cs, pl, c | `["child"]` | `["orph"]` | `{"z": 7}` |
| **ex (before)** | `[]` | `["child","orph"]` | `{}` |

The child's effect was **silently lost**. And the ancestor set had to
change too: the old code did `Map.fetch!(acc, p)`, which **raises**
when the parent is an orphan — so the case did not merely lose the
effect, it would have crashed.

The port now carries the same **13 discriminating cases** the C, C# and
Perl ports carry, plus the §5.5 counterexample, so a port that passes
them agrees with the fleet.

### `Canon.value_key(:null)` raised `FunctionClauseError`

`:json.decode` on OTP 27 maps a JSON null to the **atom** `:null`, not
to `nil`. `Canon.encode/1` had a clause for it. `Canon.value_key/1` did
not. The two are parallel paths, and the same fix had reached one of
them and not the other — which is the shape of half the defects in this
project.

It was reachable from a plain `derive/1`, because
`state_and_conflicts/3` calls `distinct_values?/3` for **every**
candidate key, not only for contested ones. An **uncontested** null
value was enough to crash the whole fold.

### A bignum became a double one ulp away from the correctly rounded value

OTP 27's bignum → double conversion — via `v * 1.0`, via
`:erlang.float/1`, and via `v / 1` alike — rounds **toward zero**
instead of to nearest-even:

```
v = 30363914016017520000
  BEAM     v * 1.0  ->  3.0363914016017523e19
  CPython           ->  3.036391401601752e+19
```

The same JSON text therefore encoded to different bytes on two
platforms: a **leaf divergence**, not a formatting one. It was found by
the mutation pass — 5 mutants out of 131,499 — and it is invisible to
a corpus that never carries a large integer literal.

The fix goes through the **decimal string**, which is exact for a
bignum, and parses it with `binary_to_float` — which is `strtod`, and
`strtod` is correctly rounded. The exact decimal digits are the bridge
between a BEAM that cannot round correctly and a C library that can.

The same conversion was used in `value_key/1`, where it matters
**more**: a value that differs by one ulp is a value the fold treats as
a *different* value, so a fold that disagrees with the reference about
whether two values are the same is a divergence **no leaf test would
ever see**.

## 4. A test script that was a live test wearing a unit test's name

`gitpp-gate`'s package.json said:

```json
"test": "node test/gate.test.mjs"
```

and `test/gate.test.mjs` calls the **real judge** and fails closed
without `TYPESAFE_AI_TOKEN` (exit 2, "TYPESAFE_AI_TOKEN not set").
Meanwhile the six **offline** tests that came with the branch —
`test/effects-gate.test.mjs` — were **never executed by anything**.

So the branch's own tests did not run, and the one script that did run
could not run in a CI job without a secret. The scripts are now split
(`test` offline, `test:live` networked, `test:all` both) and the repo
has CI for the first time, with two jobs on purpose: the offline suite
must not need the network, because a flaky third party must not be
able to turn a real defect into a red build — and a green offline suite
must not be mistaken for evidence that the integration works.

## The rule this adds

> **Before believing a mutation report, check whether the fix already
> exists on a branch.**

The report says *these ports disagree*. It does not say *these ports
are wrong*. The two are the same fact seen from one side, and the
corpus has no way to tell them apart — only the git history can.

## And the standing caution

Two of the four findings here were **in the harness**, and one of those
two meant the harness had been testing a *copy* of the thing it was
supposed to be testing.

> **A harness that tests a copy is worse than no harness, because it
> reports a number.**
