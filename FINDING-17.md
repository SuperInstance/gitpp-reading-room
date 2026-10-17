# The §5.4 fix changes no port, and that is the most useful result it could have had

§5.4's "smallest-id-first" admitted two readings. The spec now states
the sequential one. Measured against the four maintained ports:

```
  m (no parents)   z (no parents)   mm (parent m)

  spec v0.4.6                    ->  [m, mm, z]
  gitpp-js                         ->  [m, mm, z]
  gitpp-c                          ->  [m, mm, z]
  gitpp-pl                         ->  [m, mm, z]
  gitpp-cs                         ->  [m, mm, z]
```

**All four already conformed.** No port change was needed, and none
was made.

That is a materially different result from the other three clause
fixes, and the difference is worth stating:

| clause | ports before the fix | after |
|---|---|---|
| §5.1 class precedence | js was wrong | js changed |
| §5.1 unclassifiable records | c was wrong | c filed, not yet changed |
| §5.2 losers order | js and pl were wrong | both changed |
| **§5.4 order** | **all four were right** | **spec states it** |

Three of the four fixes changed behaviour, and one changed only the
*description* of behaviour that was already correct. A spec fix that
turns out to require no port change is the cheapest kind there is,
and it is also the kind that most needs saying out loud — because the
failure mode of a spec fix is to leave the text vague and hope the
ports happen to agree.

## And the one reader that does not conform stays that way

`reader3-c` implements the **tier** reading. It is the independently
written implementation, and it was not "fixed" to agree.

That is deliberate, and it is the most important decision in this
round. reader3-c is the only implementation in the project not
authored by the spec's author. If I changed it to conform, the
experiment would have exactly one independent reader left, and it would
be an independent reader that agrees with everything — which is the one
thing an independent reader must not be.

So its divergence stands, and it is now a **measurement** rather than
an ambiguity:

> Before v0.4.6: reader3-c's `order` differed, and the spec did not
> say whether it was right. **After v0.4.6: reader3-c's `order` differs,
> and the spec says it is wrong.**

The first is an open question. The second is a defect with a citation.
That is the entire difference between a specification and a hope, and it
is exactly what the other three fixes bought for their respective
clauses.

## The pattern across four clause fixes

| clause | finding | resolution |
|---|---|---|
| §5.1 class vs ancestry | Jev 0.030 / 0.960 | spec states class-first |
| §5.1 unclassifiable kind | 3 readers concur | spec states "lowest" |
| §5.2 losers order | generated corpus, 56 splits | spec states the order |
| §5.4 Kahn order | generated corpus, 28 splits | spec states sequential |

**Every one was found by a different method, and every one needed a
sentence rather than an algorithm change.** Three of the four were
found by *generating* the shape that separates the readings; the first
was found by having two readers disagree and adjudicating.

The generalisable finding is in the method table, not the spec:

> A specification is improved by finding the clause where two defensible
> readings exist and **writing down which one is meant**. The clause
> does not need to change what implementations do — it needs to change
> what they can be *accused of* doing.

That is a small change to the text and a large change to what the text
can be used for.
