# gitpp-js tie-break diverges from v0.4.6 §5.2 — and my own 7 cases missed it

An audit subagent, given the seven v0.4.6-discriminating cases I had
already run, reported that **gitpp-js is 7/7**. I then had it construct
a case aimed at the §5.2 tie-break specifically. It found one, and it
finds a real divergence.

## The case

```json
[{"id":"a","kind":"task","parents":["c"],"effects":{"x":1}},
 {"id":"b","kind":"task","parents":[],"effects":{"x":2}},
 {"id":"c","kind":"task","parents":[],"effects":{}}]
```

All three are `task`, so class cannot decide. `a` and `b` are mutually
incomparable in ancestry, so ancestry cannot decide either. §5.2's stated
tie-break applies: **first in topo order**.

All three ports compute the same `order`:

| port | order | state | setters | correct? |
|---|---|---|---|---|
| `gitpp-js` | `[b,c,a]` | **`{"x":1}`** | `x→a` | **no** |
| `gitpp-c` | `[b,c,a]` | `{"x":2}` | `x→b` | yes |
| reader1-py | `[b,c,a]` | `{"x":2}` | `x→b` | yes |

`b` is first in `order`, so `b` must set `x`. gitpp-js sets `a`.

## The cause — `src/gitpp.js:310-323`

```js
if (survivors.length > 1) {
  survivors.sort((a, b) => {
    const aIntent = [...anc.get(a.id)].filter(x => {
      const r = applied.get(x);
      return r && (r.kind === "task" || r.kind === "override");
    }).length;
    ...
    if (aIntent !== bIntent) return bIntent - aIntent;
    ...
    return codePointCompare(a.id, b.id);
  });
}
```

This is a **three-tier heuristic**: most task/override ancestors, then
fewest witness ancestors, then codepoint.

`a` descends from `c` (a task), so `aIntent(a) = 1`. `b` is a root, so
`aIntent(b) = 0`. The comparator puts `a` first, and `a` wins.

That heuristic is not in the spec. It is a reasonable-sounding
"most-intent-ancestors" rule that §5.2 **replaced** — the repaired
§5.2 says the tie-break is topo order, full stop. The port implemented
the old tie-break faithfully; the spec moved and the port did not.

Worse: the fallback is `codePointCompare`, so even with all three
tiers tied, `a` beats `b` on codepoint — which happens to be
*opposite* to topo order here. **Two independent mechanisms in the same
comparator both point the wrong way.**

## Why my seven cases missed it, and why that matters more

Every one of my seven cases put the competing same-class candidates in
**a single Kahn tier**, where "first in topo order" and "smallest
codepoint" **coincide**. The tie-break only becomes observable when
same-class survivors are spread across tiers — which is exactly what
`c` (a parent with no effects) is for: it pushes `a` into a later tier
without setting any key.

So my 7/7 was not wrong, but it was **not testing the thing it appeared
to test**. I reported "gitpp-js already conforms to v0.4.6" on the
strength of cases that could not have detected a tie-break bug. That is
the same class of error this project exists to catch, committed by me,
about my own work.

The generalisable form: **a conformance case that cannot fail for the
defect you are testing is not a test of that defect.** Case 2 (two
same-class roots) *looks* like a tie-break test. It is not, because both
candidates are in the same tier.

## Two further findings from the audit

- **`gitpp-doctor` is not a repository** — it is the clone *directory* name
  in the workflow, so the~~
  **CORRECTED — see `CORRECTION-five-ports.md`: this was wrong.** `gitpp-doctor`
  is the clone *directory* name, not a repo. The five ports are c,
  `git.pp-doctor`, js, cs, pl.
  frequently-quoted "five ports agree" claim covers at most four. The C
  port's comments cite `gitpp-doctor v0.2.1` as having once shipped the
  inverted ancestry test, so its absence is not hypothetical. Any
  five-port figure should be restated as four until it is re-run.
- **gitpp-pl emits Perl warnings** on the null-kind path
  (`Use of uninitialized value $new_kind in hash element at Gitpp.pm:400`)
  while producing correct JSON. Cosmetic, but it means a null-kind record
  is treated as an anomaly by that port's own diagnostics.
