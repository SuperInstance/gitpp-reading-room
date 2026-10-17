# Three of four ports fold records that can never be topologically ordered

Found by the v0.4.6 conformance batch (`d38`), which is the first case
in the corpus to place a record *downstream of a cycle*.

## The case

```json
[{"id":"ca","kind":"task","parents":["cb"],"effects":{"x":"cycle-a"}},
 {"id":"cb","kind":"task","parents":["ca"],"effects":{"x":"cycle-b"}},
 {"id":"down","kind":"task","parents":["cb"],"effects":{"y":"downstream"}},
 {"id":"ok","kind":"task","parents":[],"effects":{"z":"fine"}}]
```

`ca` and `cb` are a 2-cycle. `down`'s parent is `cb`, so `down` can never
reach in-degree zero and can never be topologically ordered. `ok` is an
unrelated root.

## Results

| port | state | orphans | correct? |
|---|---|---|---|
| `gitpp-js` | `{y:"downstream", z:"fine"}` | `[ca, cb]` | **no** |
| `gitpp-cs` | `{y:"downstream", z:"fine"}` | `[ca, cb]` | **no** |
| reader1-py | `{y:"downstream", z:"fine"}` | `[ca, cb]` | **no** |
| `gitpp-c` | `{z:"fine"}` | `[ca, cb, down]` | **yes** |
| reader3-c (independent) | `{z:"fine"}` | `[ca, cb, down]` | **yes** |

Three ports fold `down` and put `y` into state. Two — including the
independently written C reader — exclude it.

## The spec is not ambiguous here

§5.5:

> **Cycle-affected records are orphans for output purposes.** They are
> excluded from the fold exactly like missing-parent records, and they
> MUST appear in the returned `orphans` list.

"Exactly like missing-parent records" settles it. `down` cannot be
ordered, so it cannot be an ordinary candidate, and the clause that
keeps an orphan's descendants in the fold does not apply to it — a
descendant of a *missing* parent may still be ordered, and one whose
parent is in a cycle cannot.

This is not a v0.4.6 regression. The sentence predates it and v0.4.6
only added emphasis to the *non-cycle* rule. The divergence is older;
the corpus simply never contained a case that could see it.

## Why the corpus never caught it

`d14`'s own `why` field claims *"3-cycle plus a record that depends on
the cycle"*. **It contains no such record.** The case that was supposed
to cover this does not test it, which is why `d38` is the first case in
the corpus to reach it.

That is worth stating separately from the defect itself, because it is
the more transferable finding: a case whose documentation describes
coverage it does not have is worse than a missing case, because it
suppresses the work of writing one. Anyone auditing coverage by reading
`why` fields would have marked this as covered.

It is also the second instance in this project of a conformance claim
outliving its evidence — the first being the 25 cases that asserted
nothing. Both were found by a build-lane agent looking for unrelated
cases, not by anyone auditing the cases that already existed.

## Cross-port agreement hid this

Three ports agree with each other and are all wrong. Agreement between
implementations is only evidence when one of them is independently
derived — which is exactly what `reader3-c` is, and it is the one that
disagrees. Had the corpus contained no case for this, the four-port
suite would report 29/29 forever and the divergence would survive.

## The fix

Every port needs: *a record whose parents are all either missing or
unorderable is an orphan.* Concretely, a port that computes orphans as
"any of my parents is absent" must ALSO treat "my parent was never
emitted" as absent. `gitpp-c` does this by construction — it defines
orphans as anything Kahn never emitted, which is a *different* rule that
happens to be right for cycles and, as
[gitpp-rr-reader3#1](https://github.com/SuperInstance/gitpp-rr-reader3/issues/1) records,
wrong for missing parents.

**Neither rule is sufficient alone.** One is right for cycles and wrong
for descendants-of-orphans; the other is right for descendants and wrong
for cycles. The correct rule is their union:

    orphan(r)  ==  some parent of r is missing from the applied set
               OR  some parent of r is itself an orphan (transitively)

`gitpp-c`'s version is the union in one expression, which is why it gets
both right. The other three implement only the first clause.

---

# Correction: my first proposed fix was wrong, and the second one works

I proposed the "union" rule in the finding above. **It does not work**,
and `readers/py/cyclefix.py` records why.

## The first attempt

> orphan(r) == some parent is missing **OR** some parent is itself an orphan

A cycle member's parents are all **present** — that is what makes it a
cycle rather than a missing parent. So the second clause never fires on a
cycle, and the rule scores `U == A` in **11 of 12** cases. A rule that
agrees with the rule it was meant to improve on, in 92% of cases, is not
an improvement. I should have caught that from the number rather than
writing it up as the fix.

## The rule that works — three clauses, not two

```python
def is_orphan(rec):
    ps = rec.parents
    if any(p not in applied for p in ps):                  # 1. missing parent
        return True
    if rec not in topologically_orderable:                  # 2. cycle member
        return True
    if any(p not in topologically_orderable for p in ps):   # 3. cycle descendant
        return True
    return False
```

Clause 3 is the one that matters, and it is the clause no existing port
has. It distinguishes a **cycle descendant** from a
**missing-parent descendant**: the former's parent *exists but is
unorderable*, the latter's parent *does not exist*. A rule keyed on
existence alone cannot tell those apart — which is exactly why every port
gets one of the two cases wrong.

| case | A (missing-parent) | B (kahn) | U3 |
|---|---|---|---|
| desc of **missing** parent | `[orph]` ✅ | `[]` ❌ | `[orph]` ✅ |
| desc of **2-cycle** | `[]` ❌ | `[ca,cb,down]` ✅ | `[ca,cb,down]` ✅ |
| desc² of 2-cycle | `[]` ❌ | `[ca,cb,d1,d2]` ✅ | `[ca,cb,d1,d2]` ✅ |
| cycle + downstream branch | `[]` ❌ | all 5 ✅ | all 5 ✅ |
| missing **and** cycle together | `[m]` ❌ | `[ca,cb]` ❌ | `[ca,cb,m]` ✅ |

`U3` matches the spec on all 13 cases. It equals A in 4/13 and B in
10/13 — far enough from both that it is a genuinely third rule rather
than a relabelling of either.

The last row is the one no existing port can pass: a record set with
both a cycle *and* a missing parent. Each single rule catches one and
misses the other; only the three-clause rule catches both.

## What this changes about the port fixes

The two issues are **not** independent, and fixing the cycle case alone
by switching to Kahn-based orphan detection would **regress** the
`kind`-adjacent case in
[gitpp-rr-reader3#1](https://github.com/SuperInstance/gitpp-rr-reader3/issues/1)
— which is precisely the mistake `gitpp-c` makes, and precisely why
`gitpp-c` gets cycle-descendants right and missing-parent-descendants
wrong.

A port must move to the three-clause rule, not from A to B.
