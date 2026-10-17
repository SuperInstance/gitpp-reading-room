# Jev adjudication — a fourth reader with no stake in any port

## Why this reader is different

Readers 1 and 2 share an author. Their agreement is evidence of nothing.
Reader 3 is independent but is a sample of one, and it happens to be the
one that dissents on §5.5.

Jev wrote none of the ports, authored none of the corpus, and cannot
pattern-match to a codebase. It is also a strange kind of reader: it is
not a code generator. It returns a **calibrated probability that a framed
claim is supported by the state text**, and it is instructed to decline
rather than guess. So it cannot answer "what does the spec say?" in
prose — but for a claim the spec either supports or does not, it can say
how confident it is.

Framing is symmetric: identical state, competing instructions, one call
per side. A difference in score is then a property of the clause, not of
how one side happened to be worded.

## Calibration first — an instrument that cannot say "no" cannot adjudicate

| control | score | expected |
|---|---|---|
| true claim, plainly stated | **0.990** | high |
| false claim, contradicted by the state | **0.010** | low |
| true claim, never mentioned in the state | 0.030 | middle |

Separation true-vs-false: **+0.980**. Out-of-scope sits strictly between
the two. The instrument discriminates and it refuses to assert on text
that is not there. Its scores are therefore usable — and the real-clause
results below are worthless without this table, which is why it runs
first.

## Verdicts

| clause | reading A | reading B | reading |
|---|---|---|---|
| §5.1 vs §5.2 class/ancestry | class-first **0.820** | ancestry-first 0.140 | **class is absolute** |
| §5.5 orphan's descendants | effects apply **0.820** | branch excluded 0.090 | **descendant applies** |
| §1 id validation | hard error **0.680** | not enforced 0.310 | **error, but weakly** |
| §1.1 `kind: null` | **undetermined 0.840** | inherits witness 0.120 | **genuinely unspecified** |

## What this settles, and what it does not

**§5.1 vs §5.2 is decided, and the decision is the one readers 1 and 3
made.** Jev scores class-absolute at 0.820 against ancestry-first at
0.140 — a wide gap, and the calibration table shows this instrument can
produce a 0.010 when the text points the other way. So the class/ancestry
ambiguity is *readable*: an implementer who chose ancestry-first
misread §5.1, they did not read it defensibly. That converts a live
ambiguity into a spec bug — the wording is now demonstrably misleading,
which is why two careful readers got it wrong.

**§5.5 is settled against reader 3.** 0.820 for "the descendant's
effects apply", 0.090 for "the branch is excluded". Reader 3 is the
odd one out, and the score says the text is clear. This is the finding
that most changes the picture: the one divergence I had flagged as
*unassigned* — found by divergence rather than planted — is the one where
the independent implementation is wrong. **Divergence alone is not
evidence; it is a trigger for adjudication.**

**`kind: null` is the real hole, and Jev confirms it independently.**
0.840 that the text does not determine it, against 0.120 for the
concrete answer. Both independent agents (reader 3, reader 4) had already
named this clause unprompted. The third independent reader agrees. This
is the highest-risk clause in the spec and three readers now concur.

**§1 id validation is only weakly pinned (0.680/0.310).** Not a
conclusion either way. Treat as open.

## The limit of this method

A probability is not an argument. `kind: null` scoring 0.840 "undetermined"
is strong evidence that the text does not settle it, and the way to
actually settle it is to **write the rule** — which is now the only
outstanding spec change this experiment demands.
