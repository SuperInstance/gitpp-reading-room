# CORRECTION — the 12.36×, the "zero noise", and the witness machine's three parts

**Date:** 2026-10-18
**Raised by:** `research/PRICE-TABLE.md` and `research/LEDGER-CONVERGENCE.md`
**Status:** three of our own claims did not survive measurement

---

## 1. The 12.36× did not reproduce, and it is not ours

The qthe tavern gift (`GIFT-FROM-QTHE-TAVERN.md`, merged in PR #4) reported:

> *"Batching a reader's whole battery into one call added zero measured
> noise — and costs 12.36x less."*

**Measured again, properly: 9.461×** (TypeSafe, 30 calls → 1, 5 reps each:
9,631 → 1,018 input tokens, `sd = 0.0`, 30.5× faster on wall clock). On
MiniMax, which has no native batching, **5.294×** — and **57.7% of that
saving is just the system prompt being re-sent 30 times** (1,566 of 2,714
tokens).

**And `12.36` appears NOWHERE in AI-Writings** — two unrelated hits.

So the figure is **smaller than reported, and the number as quoted was not
ours**. The direction is right and the magnitude was inflated.

## 2. "Adds zero measured noise" did not reproduce either

Batched-arm max within-arm stdev **0.0679** vs **0.0508** unbatched.
Batched answers are **not** bit-identical across repeats, and the drift
reaches **0.12**.

**What does hold, and it is the part that matters:** both arms agreed with
the rule engine **143/150** — **the same VERDICT, not the same number.**
That is a much better result than the original claim and a much more
useful one: *batching preserves the decision, not the score.*

## 3. The witness machine is not the three files we said it was

The house's own rule, recorded here and in our memory, was:

> *"The house's witness machine is `recorder.py` + `corpus.yml` +
> `dispatch-ledger.csv`. FNV-1a-64 chained, tamper-evident, replayable,
> CI-enforced."*

**Two of the three are wrong.**

| we said | the truth |
|---|---|
| `recorder.py` | **true** — but it is still `witness-chain/0` on `main` (fnv1a-64, `json.dumps(sort_keys=True)`, no SHA-256, no JCS, no domain separation, no signature, no version field) |
| `corpus.yml` | **there is no `corpus.yml`.** It is `corpus.py` |
| `dispatch-ledger.csv` | **it is a dispatch log, not a chain.** 167 CSV rows, **zero hash fields** |

**And the "replayable, tamper-evident" claim is false for the bot logs.**
Eleven bot logs carry **no `prev` fields at all**; 329 of 330 hashes
reproduce as `sha256(payload)` alone. **Deleting 50 records from
`rsi-gan` leaves it "112/112 intact." DELETION IS UNDETECTABLE.**

The ledger is nearly free, though: **338 B/record**, verify **132.9 µs/record**
against a **216,475 µs** witnessed answer — **0.061%**. Free is not the
problem. **Unchained is.**

## 4. The canonical verifier cannot read the reference implementation

`quilt-stone` claims *"one verifier for every repo's chain."* It has **zero
occurrences** of `recorder`, `quilt-claw`, `AI-Writings`, `MicroMoth`,
`witness-chain`, `RFC 8785` or `RFC 6962`.

**The house's canonical verifier and the house's reference implementation
are disjoint.** Two convergence artifacts that have never met.

**The anchor already exists, though.** `moth-ledger` and `doubt-ledger` do
**real Ed25519 over a root**. It is simply not wired to the reference chain
— and it is a better anchor than the HMAC stand-in `witness-spec` currently
uses.

## 5. A latent cross-language bug, and a false byte-compatibility claim

`quilt-claw` claims *"9 canonical-encoding vectors… all produce identical
bytes."* **7 of 13 probes diverge**, including every class the README
names. **No vectors file exists in the repo.** The honest caveat: on the
8 real records the two implementations agree 8/8 — **the bug is LATENT**,
which is exactly why nobody caught it.

## 6. The number the house's economics actually needed

**On a task whose predicates are exactly decidable, the paid observer got
29/30 and the rule engine got 30/30.**

**The paid model is not just more expensive than a rule engine. It is LESS
CORRECT, on a task where the rule engine is exact.**

That is the strongest argument the fleet has for the whole observer/
recombination economy, and it inverts the usual framing. The house's rule
was *"never price a cheaper different answer."* The measured version is
sharper:

> **Never pay a model to decide what code can decide. Pay it only where the
> predicate is NOT decidable by code.**

And the reason to witness rather than re-ask is now a number: in
`bots/rsi-gan/witness.log`, **263 prompt records, 67 distinct, 196 excess
re-asks — 74.5% of everything issued was redundant**, with the top prompts
asked **6×** each. The cause is in the code: `rsi_gan.py:168,202` calls
`jev_score` once per candidate *every round*, so the paid observer is
**re-asked instead of witnessed once**.

## 7. And two honest negatives

- The cost of a **propagated** bad verdict is **NOT MEASURED**. On the real
  `score-table` predicate (3-of-5-participants run) JEV answered 0.02 vs
  0.68 — **correct, 0/2 mis-verdicted**.
- The **local-model arm is NOT MEASURED.** No `$/token` figure was invented
  for a model the fleet does not run.

Neither changes the ratio's sign: a local model can only be cheaper than
$0, and the rule engine is free.

## The corrected rules

1. **Batching preserves the VERDICT, not the score** (143/150 agreement with
   the rule engine; within-arm drift to 0.12).
2. **The batching ratio is 9.461× on TypeSafe, 5.294× on MiniMax**, and most
   of the MiniMax saving is the system prompt re-sent.
3. **A ledger with no `prev` field is not a chain.** The house's own logs
   are deletable without detection.
4. **The witness machine is `recorder.py` + `corpus.py`,** and the dispatch
   ledger is not part of it.
5. **Never pay a model to decide what code can decide.**
6. **Witness once, re-derive forever** — 74.5% of the paid calls in
   `rsi-gan` were re-asks of the same 67 prompts.
