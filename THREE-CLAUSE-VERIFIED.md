# The three-clause orphan rule is now implemented in three ports — and I was wrong about which case bites

A subagent implemented the three-clause orphan rule in `gitpp-js`,
`gitpp-cs`, and `gitpp-pl`. It also corrected a claim I made, and the
correction is more interesting than the fix.

## The correction

I wrote:

> That last row is decisive: a record set with BOTH a cycle and a
> missing parent. Each single rule catches one and misses the other.

**"Each single rule" was wrong**, and the agent is right to have
challenged it. The three ports were not implementing pure rule A or
pure rule B. All three implemented the **hybrid**:

    orphans = ruleA(missing parent)  ∪  DFS(cycle members)

The hybrid passes `missing AND cycle` — which is why those three ports
already scored 9/13 including that case. My "no single-rule port can
pass" line described two rules none of the ports actually used.

## What actually bites

The cases that discriminate the hybrid from the three-clause rule are
the **cycle-descendant** ones — a record whose parent exists but is
unorderable:

| case | hybrid (A ∪ cycle-members) | U3 (three-clause) |
|---|---|---|
| desc of 2-cycle | `[ca, cb]` ❌ | `[ca, cb, down]` ✅ |
| desc² of 2-cycle | `[ca, cb]` ❌ | `[ca, cb, d1, d2]` ✅ |
| desc of self-loop | `[a]` ❌ | `[a, k]` ✅ |
| cycle + downstream branch | `[a,b,r,t]` ❌ | `[a,b,leaf,r,t]` ✅ |
| desc of **missing** parent | `[orph]` ✅ | `[orph]` ✅ |

Before: **9/13**. After: **13/13** on all three ports.

The agent's commit messages state the load-bearing cases accurately
rather than repeating my claim, which is the right call — a fix whose
justification rests on the wrong example is a fix that will be
misunderstood the next time it is read.

## Verified independently, from the pushed branches

I cloned `fix/cycle-descendant-orphans` fresh and ran the cases against
the pushed code rather than trusting the agent's report.

**`gitpp-js`** — 6/6, including `desc of missing → ["orph"]` and
`desc of 2-cycle → ["ca","cb","down"]`.

**`gitpp-pl`** — the two decisive cases:

```
cycle-descendant:  orphans=['ca','cb','down']  state={"z": 5}
desc of MISSING:   orphans=['orph']            state={"y": 2}
```

Both correct. **Those two together are the proof the rule is needed**:
one requires orphaning a descendant, the other requires *not* orphaning
a descendant, and they differ only in whether the parent exists. No rule
keyed on ancestry alone gets both, and no rule keyed on existence alone
gets both. The two conditions have to be distinguished — which is clause
3.

## Regression, per the agent, mutation-checked by me where it mattered

| port | 13 cases | existing suite |
|---|---|---|
| `gitpp-js` | 9/13 → **13/13** | 12/12 → **26/26**; pinned root 17/17 → **31/31** |
| `gitpp-cs` | 9/14 → **14/14** | conformance unchanged 27/1 |
| `gitpp-pl` | 9/13 → **13/13** (28 assertions) | 28/0 → 28/0; CI smoke 30/30 |

`state`, `order`, `conflicts`, `setters`, leaf encoding and closure
roots were untouched, confirmed by the live `poc-quilt-1@task-c` root.

## A separate, unfixed defect the agent found

`gitpp-cs` fails `L1/signed-zero`: the runner emits `0.0` where §3 wants
`0`. It fails **identically before and after** this change — it is a
number-encoding gap in the output path, not an orphan issue, and it is
out of scope here.

Worth flagging for two reasons: it is a real spec violation, and the
C# README claims 28/28. The agent also found that its own harness is
*stricter* than the official `runner.mjs`, which silently ignores
`expected_order` and `expected_orphans_includes` — so part of the
discrepancy is harness laxity rather than a worse result. That is the
same shape as the `run.mjs` `cmp` bug: **a runner that does not check a
field it is given.**

Not fixed here. Worth its own branch.

---

# Second correction: my "clause 3" is redundant, and I sold it as essential

While verifying my own fix I mutation-tested it by deleting clause 3 —
and **the 13 cases still passed**. That is not a passing mutation; it
means clause 3 was doing nothing.

## Why

I defined `orderable` as Kahn over the full set with in-degree counting
only parents that **exist**, and then wrote:

```
1. some parent id is missing
2. the record is not orderable            <- catches cycle members
3. some parent is not orderable           <- what I claimed was essential
```

Clause 2 already implies clause 3, because Kahn non-emission is
**transitive**: if `cb` never becomes emitted, then `down` (whose only
parent is `cb`) never reaches in-degree zero, so `down` is also not
orderable. The same holds for every descendant, at any depth.

Observed directly:

```
desc of 2-cycle     orphans=['ca','cb','down']   order=[]
desc of self-loop   orphans=['a','k']            order=[]
cycle + branch      orphans=['a','b','leaf','r','t']  order=[]
desc of missing     orphans=['orph']            order=['kid']
```

Note the last line: `kid` IS orderable and IS emitted, because its
parent `orph` is excluded from the *applied* set rather than being
unorderable. That is clause 1 doing its job.

## So the correct rule is TWO clauses

```
orphan(r)  ==  some parent is missing from the applied set
            OR r cannot be topologically ordered
```

with `orderable` computed by Kahn over the applied set, in-degree
counting only parents that exist. This is what `gitpp-js`, `gitpp-cs`
and `gitpp-pl` now implement, and the agent's independent derivation
reached the same place by a different route.

## Why I got it wrong

I reasoned that a cycle descendant's parent "exists but is unorderable"
and concluded a separate clause was needed to notice that. True — and
irrelevant, because the *descendant itself* is equally unorderable, and
the two-clause rule already checks that. I described a distinction the
implementation did not need, because I was reasoning clause-by-clause
instead of asking whether one clause subsumes another.

The empirical tell was available and I did not use it: **a mutation
that does not change the result is not a mutation.** The same instinct
that caught the broken `equivalence.py` search — "if the test cannot
fail, it is not testing the thing" — applied directly to my own rule, and
I had to be told by my own mutation before I checked.

## The part that was genuinely load-bearing

The rule still needs clause 1 *and* clause 2 together, because they are
not substitutes:

| | cycle descendant | missing-parent descendant |
|---|---|---|
| clause 1 only (any parent missing) | misses it | ✅ |
| clause 2 only (not orderable) | ✅ | would orphan `kid` too — **wrong** |
| **both** | ✅ | ✅ |

Clause 2 alone over-orphans: applied to the whole record set, `kid` is
unorderable precisely *because* its parent was excluded, so it must be
computed against the **applied** set, not the raw one. That ordering
dependency is the subtle part, and it is where the earlier failed
derivations in `gitpp-rr-reader3#1` and `gitpp-c` went wrong.
