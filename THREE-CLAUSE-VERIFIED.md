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
