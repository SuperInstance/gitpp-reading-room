# Adjudication sweep — every clause the readers actually disagree on

Only *disputed* clauses are scored. Agreeing clauses need no judge, and
asking Jev about them would be asking it to grade homework. So this reads
`result.json` and adjudicates exactly what split.

The clause text is **verbatim from the spec**, not paraphrased, and that
turned out to matter: `p10` scored 0.680 / 0.310 when I fed it my own
summary of §1.1 and **0.870 / 0.230** when fed the actual sentence,
because the real text adds "A record missing any of id, parents, kind,
effects, reason is malformed" — a rule my paraphrase had dropped. An
adjudicator fed a paraphrase judges the paraphrase.

Calibration is the gate from `calibrate.py`, unchanged: 0.990 true /
0.010 false / 0.030 out-of-scope. A verdict is only reported when the
spread is ≥ 0.25; below that the clause is called **UNDETERMINED** rather
than given a winner.

| clause | reading | score | verdict |
|---|---|---|---|
| §5.1 vs §5.2 | class-absolute | **0.960** | **class is absolute** (spread +0.930) |
| §5.1 vs §5.2 | ancestry-first | 0.030 | |
| §5.5 orphans | descendant's effects apply | **0.790** | **descendant applies** (spread +0.690) |
| §5.5 orphans | whole branch excluded | 0.100 | |
| §1.1 id | malformed → excluded | **0.870** | **excluded** (spread +0.640) |
| §1.1 id | processed normally | 0.230 | |
| §1.1+§5 kind:null | spec does not determine it | **0.760** | **UNDETERMINED** (spread +0.480) |
| §1.1+§5 kind:null | ranks lowest | 0.280 | |

## What the sweep settles

**§5.1 vs §5.2, decisively — 0.960 against 0.030.** The widest spread in
the set, and the calibration table shows this instrument produces 0.010
when the text points the other way. Ancestry-first was never a defensible
reading. `reader2-js` is that reader, and it is in this repo, written by
the spec's author. This is a **wording bug in §5.1**, not an open question:
the parenthetical is the clause's only mention of ancestry and it sits as
an exception, so it invites being generalised into "class beats ancestry".

**§5.5 — 0.790 against 0.100.** The text is clear, and `reader3-c` is
the one that diverged. Worth stating plainly: the single divergence I
found *without assigning it* — reader 3 simply read the clause
differently — is the one where the independent implementation is wrong.
**Divergence is a trigger for adjudication, not evidence.** A harness
that reported "readers disagree" and stopped would have filed a
correct-looking defect against a clause that the text already settles.

**§1.1 id — now decided at 0.870, having been left open earlier.** My
first pass scored this 0.680 / 0.310 on paraphrased text and I wrote it
up as "too close to call". Feeding the verbatim sentence flipped it. The
earlier "open" verdict was an artefact of my own summary, not a property
of the clause. **Do not adjudicate a clause you have not quoted.**

**kind: null is the only genuine hole.** 0.760 that the text does not
determine it, against 0.280 for the concrete answer — and the "winner's"
score is the *refusal to answer*, not a choice. That is the signature of a
real gap rather than a hard question: the instrument is confident that
the text is silent.

Three readers now concur on this clause from three directions — reader3
and reader4 by reading, reader2 and reader3 by executing, and Jev by
adjudicating — and the only thing any of them produced is a reason to
**write the rule**.
