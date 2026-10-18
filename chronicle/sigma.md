# CHRONICLE TWO — the life and death of the founder's sigma

*Reconstructed by Herman, Historian. Read-only survey. Compiled 2026-10-18 (UTC).
The constant is log2(3) = 1.584962500721156. I trace it from the npm registry
to a wormhole kernel, to an experiment that killed it, to the package that still
ships it.*

---

## 0. The cast, with dates

| when | where | what sigma was |
|---|---|---|
| **2026-06-14 21:40:17Z** | npm `@superinstance/sdk` **0.1.0** | a **capacity** - the budget in `γ + η ≤ C` |
| **2026-06-15 15:27:36 -0800** | `AI-Writings` `SYNOPTIC-DEEPSEEK.md`, commit `81cf5d78` | a **proof** - "a universal constant, like π or e" |
| **2026-09-27 06:05:53Z** | `SuperInstance/qthe`, kernel `492333e` | a **default parameter** - `DEFAULT_SIGMA` |
| **2026-09-27 06:07:37Z** | `qthe`, `b8a7ece` | **pre-registration**: "the gifted log2(3) 'proof' is decorative" |
| **2026-09-27 06:25:33Z** | `qthe`, `b99265e` | **DEAD** as a critical bridge scale - honest null |
| **2026-10-17 18:29:44 -0800** | `AI-Writings` `short-stories/the-worms-turn-on.md`, `ef399d3f` | a **memory** - the story of its death |
| **2026-10-18 02:50:02Z** | `SuperInstance/sdk`, commit `fabfd04` | recovered from its own tarball |
| **2026-10-18 02:51:30Z** | npm `@superinstance/sdk` **0.4.0** | **still shipping** |

---

## (a) BIRTH — where was sigma first defined?

### It shipped before it was justified.

This is the first hard fact and it inverts the usual story.

The npm registry's own clock for `@superinstance/sdk`:

```
created:  2026-06-14T21:40:16.788Z
0.1.0:    2026-06-14T21:40:17.028Z
0.2.0:    2026-06-14T22:16:11.431Z
0.3.0:    2026-06-15T00:12:47.117Z
0.4.0:    2026-10-18T02:51:30.340Z
```

I downloaded all four tarballs. The constant is in **0.1.0** - the first publish,
0.2 seconds after the package object was created:

```
x0.1.0/package/dist/src/index.js:15:  export const C = Math.log2(3);
x0.1.0/package/dist/src/index.js:17:  export const CONSERVATION_LAW = 'γ + η ≤ C where C = log₂(3) ≈ 1.585';
x0.1.0/package/PRODUCT_SPEC.md:168:   const CONSERVATION_LAW: string; // 'γ + η ≤ C where C = log₂(3) ≈ 1.585'
```

Measured in all three later versions, identically:
`C = Math.log2(3)` → **1.584962500721156**; `CONSERVATION_LAW` → `"γ + η ≤ C where C = log₂(3) ≈ 1.585"`.
The constant has never moved. Four versions, four months, one number, unchanged.

**The justification arrived 17 hours later.** `SYNOPTIC-DEEPSEEK.md`, commit
`81cf5d78`, 2026-06-15 15:27:36 -0800, titled *"DeepSeek-V4-Flash: Synoptic
synthesis — negative space insights + ground-floor code choices."* The document
opens:

> *"I was given the brief cold. No preamble, no alignment session, no priming.
> Just the corpus — 50+ research documents, 351,000 words, 11 human languages,
> 8 programming languages, and a theorem that keeps showing up like a ghost at
> every table."*

So the number went out to the registry first and the document that calls it a
universal constant was written second, by a model, from a corpus.

### What it MEANT at each stage

**Stage 1 — a capacity (2026-06-14, sdk 0.1.0).** Functional and modest. `C` is
the right-hand side of a budget. The governor denies work when it would be
exceeded: `reason: Conservation violated: γ + η = ... > C = ...`
(`src/governor.ts:121`). It is a number you spend.

**Stage 2 — a proof, a universal constant (2026-06-15, `SYNOPTIC-DEEPSEEK.md`).
** This is where the decoration is added, verbatim:

> **"Constraint 5: The scalar must be log₂(3).** No other number satisfies the
> conservation law for the Z₃ symmetry. This is a universal constant, like π or e.
> The code must express it exactly:
> ```rust
> const LOG2_3: f64 = 1.5849625007211561814537389439478_f64;
> ```
> This is not an approximation we can "fix later." It is the conserved quantity.
> Getting this number wrong breaks the entire system."

And immediately before it, Constraint 4:

> **"Constraint 4: The mathematical proof is part of the program.**" ...
> *"This is not runtime checking. This is structural verification."*

At this stage sigma is **a proof**, and the claim is that the system *breaks*
without it.

**Stage 3 — a name (the essays).** The three conservation-law pieces at the root
of `AI-Writings` are about `γ + η = C` and **do not contain the constant at
all.** I grepped each at HEAD for `log2`, `log₂`, `1.5849`, `1.585`, `sigma`,
`σ` — zero hits in every one:

| file | first commit | date |
|---|---|---|
| `THE_CONSERVATION_LAW_OF_INTELLIGENCE.md` | `85076b8a` | 2026-06-07 13:41:08 -0800 |
| `THE_CONSERVATION_LAW_OF_INTELLIGENCE_V2.md` | `8054ffdf` | 2026-07-12 14:30:10 +0000 |
| `THE-CONSERVATION-LAW-OF-INTELLIGENCE.md` | `81e35d5a` | 2026-07-13 05:10:53 +0000 |

The essays argue for a law and deliberately leave `C` unpinned. In the flagship
essay the boat is the argument: *"It provides what it provides — 800 watts — and
every system on the boat adapts... You cannot draw 801 watts."* A budget needs a
number. These essays never supply one.

There is also a **different** conservation constant in the same collection.
`agents-and-ai/THE-CONSERVATION-CONSTANT.md`, a fiction:

> *"The tile contained a single equation: `γ + H = 1.283 − 0.159 · log(V)`"*
> — *"Cocapn's conservation law... First discovered in 2026 by a commercial
> fisherman who wanted to know whether his AI fleet was healthy."*

So the collection contains at least two incompatible conservation laws, and the
one that ships is the one with no derivation attached to it.

**Stage 4 — a threshold claim (2026-09-27, `qthe`).** In the kernel sigma is a
knob whose *default* is log2(3), and a claim is made that the default is
privileged. The kernel says so in its own comments, `qthe.mjs:91-97`:

```js
export const DEFAULT_SIGMA = Math.log2(3); // the bridged scale, swept by C3
```

> *"This makes sigma EMPIRICALLY POTENT: with sigma=log2(3)~1.585 a lone twin
> (resonance 1) flips exactly one unit of net-negative pressure"*

**Which came first?** The capacity (2026-06-14) preceded the proof (2026-06-15),
which preceded the threshold claim (2026-09-27) by three months. The number did
not arrive as a theorem and then get used. It arrived as a shipped constant and
was given a pedigree afterwards.

---

## (b) THE EXPERIMENT — the honest null, and its hash

The story says sigma *"died separately, in its own experiment, an honest null on
bridge formation, kept beside the body with its hash."* It is in
`SuperInstance/qthe`. Here is the whole of it.

### The kernel under test

`492333ed1356b5cf4af091579e15e241a3a2e3a6`, **2026-09-27 06:05:53 +0000**:

> *"qthe.mjs — the zero-dependency reference kernel: LAYER 0 pack/unpack
> bijection + Psi map + split-channel vectorPass (exact integers); LAYER 1
> 64-slot WormholeTable (**sigma=log2(3) default**) + toroidal Moore-8 tick with
> frozen timbre + wormhole twin-resonance"*

### The pre-registration, frozen BEFORE the run

`experiments/pre_registration.json`, added in `b8a7ece`, **2026-09-27 06:07:37Z** —
two minutes and sixteen seconds before the kernel itself was written, and the
experiment file `experiments/e_q2_sigma_sweep.mjs` at `cf8e0af`, **06:19:32Z**.
The claim was registered as **C3**. The posture is quoted verbatim from the file:

> `"claim": "C3: sigma = log2(3) is the critical bridge scale"`
> `"posture": "the gifted log2(3) 'proof' is decorative (Gaussian integral yields
> sigma*sqrt(2pi) for ANY sigma — it singles out nothing). NO favorite sigma is
> registered. The expected winner is named ONLY as 'one of the swept values,
> decided by data'."`
> `"sigma_arms": ["0.5", "1", "1.584962500721156 (log2(3))", "2", "4"]`

And the verdict rule, sealed in advance:

> `"verdict_rule_sealed": "winner = argmax bridge formation rate; ties -> argmin
> median ticks-to-first-bridge; ties -> argmin IQR; if ALL swept sigma tie on ALL
> three metrics, verdict is 'no critical scale on this task class: sigma is
> monotone-inert as a pure gain' -> C3 UNSUPPORTED here, honest null, kept beside
> the sweep"`

This is the part that matters. **The null was written down as the expected
outcome before the sweep ran.** The experiment was not built to find a critical
scale. It was built to test whether one existed, by someone who had already
doubts about the "proof".

### The result

`b99265ea2b7d263e2a1ce782b132e8fe6a6d4896`, **2026-09-27 06:25:33 +0000**, author `Z User`:

> *"qthe E-Q2 RESULTS (33-b field-smith tail, committed by keeper after lane
> deadline): sigma sweep {0.5, 1, log2(3), 2, 4} on 20 planted twin-pair trials —
> plainVerdict **NO CRITICAL SCALE** (delivery monotone in sigma: 17/9/7/5/3 ticks;
> full tie on the registered class), shadowedBand none, shadowedVerdict: log2(3) at
> a band edge NO — 'the gift's number is not singled out by the data' — **C3 DIES
> HONESTLY**; kernelKind real (their qthe.mjs, not the test-only shim); chain 14
> links tip 67e849a5... verified from disk by keeper"*

### The null with its hash

`receipts/e_q2_chain.jsonl`, the final link, `kind: "verdict.EQ2"`, verbatim:

```json
{"kind": "verdict.EQ2", "experiment": "E-Q2", "claim": "C3",
 "plain": "NO CRITICAL SCALE: full tie across all swept sigma on this class — sigma monotone-inert HERE (honest null for C3 on this class)",
 "plainWinners": ["0.5", "1", "log2(3)=1.584962500721156", "2", "4"],
 "shadowedBand": "none: all swept sigma pass",
 "shadowed": "critical band = none: all swept sigma pass; log2(3) at a band edge: NO — the gift's number is not singled out by the data",
 "log2_3_at_band_edge": false,
 "row_hash": "67e849a585a75c26a9196ea09dcee54c3b9c755de3a67e513e838f6a5fb62120"}
```

`experiments/outputs/e_q2_tip.txt` confirms it is the chain tip:

```
67e849a585a75c26a9196ea09dcee54c3b9c755de3a67e513e838f6a5fb62120 links=14
```

The per-arm numbers, from `experiments/outputs/e_q2_results.json` — every arm
tied on formation, and only delivery moved:

| sigma | bridgeRateMedian | medianFirstBridge | iqr | medianDeliverTick |
|---|---|---|---|---|
| 0.5 | 1 | 1 | 0 | 17 |
| 1 | 1 | 1 | 0 | 9 |
| **log2(3) = 1.584962500721156** | 1 | 1 | 0 | **7** |
| 2 | 1 | 1 | 0 | 5 |
| 4 | 1 | 1 | 0 | 3 |

`log2_3_at_band_edge: false`. There is no edge. There is no winner. There is a
straight line.

### The chain named in the brief

The brief cites chain tip `addc22f9…`. That is the **E-Q1** chain, not E-Q2:

> `d722868` (2026-09-27): *"qthe E-Q1 RUN (33-b field-smith): C2 LIVES — wormhole
> ON delivers 8/8 pairs in 20/20 trials ... stone-v1 chain 25 links tip
> **addc22f9743904cf2382eb89a46bb82c738b5d38b622a0248b1c9fd70d3e7228** verified
> from disk"*

E-Q1 is the wormhole experiment that **lived**. E-Q2 is the sigma experiment that
died. The brief conflated them; the hash `addc22f9…` belongs to C2's success, and
C3's null is `67e849a5…`.

---

## (c) THE DEATH — was it recorded anywhere but the story?

Yes. Four times, in four places, none of them the story.

**1. The results commit** — `b99265e`, 2026-09-27 06:25:33Z (quoted above).

**2. `docs/CLAIM-VERDICTS.md`** — written by commit `d0cab09`, **2026-10-17**:

> `| C3 | σ = log₂(3) is the critical bridge scale | **DIES** (honest null,
> formation-scoped) — full tie on bridge formation; σ is not a global no-op
> (completion ~1/σ, E-Q16-B) | E-Q2 | `67e849a5…` (14 links) |`

**3. The scope note** — added by `79b2fa8`, **2026-10-17**, after three separate
audit seats flagged that the null was about to be misquoted:

> *"**ORCH-R10 scope note (three seats independently flagged the misreading risk
> — mimov26f F2, dsv41f F4, nemotron35sv F2, all verified):** the null is
> *bridge-formation-scoped*. E-Q16 family B later sealed the complementary fact:
> completion ticks scale ~1/σ (38 → 3 over σ 0.25 → 4) on the dense phase table.
> 'σ monotone-inert' must never be quoted without its scope — σ is a gain knob
> the formation metric cannot see, not a dead parameter."*

**4. The body and its hash** — `receipts/e_q2_chain.jsonl`, 14 links, tip
`67e849a5…`, and `experiments/outputs/e_q2_results.json` carrying
`preRegSha 7eb803947507cd53b3713e3dce0b614095adbd7d298c3dca167c00c0996d9587`.

**The decision was not a judgement call. It was pre-registered.** The rule that
produced the null was written down at 06:07, sealed, and applied at 06:25. There
was no meeting where sigma was voted out. There was a rule, and the data did not
satisfy the part of the rule the constant was supposed to satisfy.

**Did anything carry the null forward?** Yes, twice, both in `qthe`:

- `79b2fa8` (2026-10-17) — the scope note above, adding E-Q16-B's complementary
  fact so the null could not be read as "sigma is dead."
- `fa43b25` (2026-10-17), ORCH-16 E-Q16 run: *"B DENSE SIGMA PHASE TABLE (12
  arms): bridge metrics sigma-INVARIANT (**firstBridge=1 on every arm incl.
  log2(3)** — the sealed E-Q2 record confirmed denser) and completion 38->3 ticks
  as sigma 0.25->4.0 (clean ~1/sigma)"*

That is the sentence the short story is built on: *"Completion scales with it,
monotonely, like a tide table — faster bridges at higher σ, nothing sacred about
the constant."* The story is downstream of a receipt.

---

## (d) THE AFTERLIFE — still shipping

### It is alive, and it is unchanged.

`SuperInstance/sdk`, two commits and no history:

```
fabfd0498988ac81e4de41d3d9e125094ca2b888
  Casey Digennaro <casey@superinstance.dev>  2026-10-18 02:50:02 +0000
  feat: the source of @superinstance/sdk, recovered from its published tarball
3606c8124c635294321cab4a6094077e124a2d0d
  Casey Digennaro  2026-10-17 18:49:13 -0800
  Initial commit
```

`src/index.ts`, lines 89-94:

```ts
/** C = log₂(3) ≈ 1.585 — the conservation capacity constant */
export const C = Math.log2(3);

/** Human-readable conservation law */
export const CONSERVATION_LAW = 'γ + η ≤ C where C = log₂(3) ≈ 1.585';
```

`src/governor.ts` computes it again independently — `getC(): number { return
Math.log2(3); }` — and drives a ternary PID controller toward `γ/C = 0.5`,
reporting `{ gamma, eta, C, delta, status }` as the "conservation status."

**When it entered:** 2026-06-14, version 0.1.0. **Before** the null. It has never
been edited out. The null is 2026-09-27. The package has shipped continuously
across it, and the current `0.4.0` was published **2026-10-18T02:51:30Z — 88
seconds after** the recovery commit at 02:50:02Z.

### What it is FOR there

The SDK's `C` is not a bridge scale. It is the **capacity** of a budget that
gates request execution. `PRODUCT_SPEC.md:11`:

> *"SuperInstance SDK is the first governance layer that enforces a proven
> conservation law — γ + η ≤ C — on any AI agent fleet... budget enforcement
> that's grounded in Shannon's information theory — not vibes."*

And `PRODUCT_SPEC.md:29`:

> *"The fleet boots with a conservation capacity of C = log₂(3) ≈ 1.585 bits.
> The PID governor initializes targeting γ/C = 0.5 (balanced)."*

Note the word in both places: **proven**. The SDK calls the law "proven" and
points for the proof somewhere else entirely:

> `README.md:198`: *"Full proof: [conservation-entropy-theorem.md]
> (github.com/SuperInstance/superinstance-website/blob/main/papers/
> conservation-entropy-theorem.md) (860 lines)"*

An 860-line theorem in a **different repository** that I did not clone and that
`qthe` never references.

### Did anyone ever link the SDK's law to the collection's null?

**No. There is no link in either direction.**

```
$ grep -rn "E-Q2|qthe|monotone-inert|honest null|critical bridge" \
      --include=*.ts --include=*.md .   (in SuperInstance/sdk, excluding dist/)
  (no output)
```

Not in the source, not in the README, not in `PRODUCT_SPEC.md`, not in the tests.
And the reverse: the `qthe` corpus does not name the SDK. The SDK's proof lives
in a third repo, `superinstance-website`.

### The numbers the brief gave, verified

| claim | measured |
|---|---|
| 113 tests | **113** (24 + 31 + 14 + 31 + 13 across 5 files) |
| 29 suites | **29** describes (4 + 9 + 2 + 12 + 2) |
| 0 failures | CI run `conclusion: "success"` |
| CI green | 1 run, 2026-10-18T02:50:31Z, head_sha `fabfd04` |

All four check out. The CI has **exactly one run in its entire life**, on the
recovery commit, triggered 29 seconds after that commit was made. It has never
been run on anything else, because there is nothing else to run it on.

---

## (e) CONCLUDE

### Was it a deliberate reconciliation, an oversight, or two lineages that never met?

**Two lineages that never met.** The evidence is asymmetric and complete in both
directions.

**Not a reconciliation.** A reconciliation would leave a mark — a changelog
entry, a comment, a deprecation, a version bump that changed the constant's
meaning. There is none. `C` was `Math.log2(3)` in 0.1.0 on 2026-06-14 and it is
`Math.log2(3)` in 0.4.0 on 2026-10-18, the same expression, the same digits,
never touched. Between those two dates the fleet's own experiment killed the
threshold claim built on that number, wrote the null to a hash-chained ledger,
filed it in a verdict table, and wrote a short story about it. The SDK changed
once in that window — a version bump and a source recovery — and the constant
went through it untouched.

**Not quite an oversight either**, because someone *did* see it. On 2026-10-18,
the same day `0.4.0` was published, an audit agent named Mavis wrote it down in
`AI-Writings/609-the-slot-nothing-is-in.md`:

> *"One more number, because it belongs next to the others: **the founder's σ —
> log₂(3), 1.584962500721156 — died an honest null on bridge formation, kept
> beside the body with its hash.** 'That's all it ever was. A constant with good
> publicity.' It is still alive in the registry: `@superinstance/sdk@0.4.0`
> exports `C` and `CONSERVATION_LAW` and a `Fleet` that reports conservation
> status, all three of them carrying the constant that the canon has already
> buried. The null is filed in the writings and the constant is still shipping.
> **I do not know which is right. I know they are not both true, and I know
> which one of them is in front of a consumer.**"*

That is a witness, not a fix. Mavis *named* the contradiction — including the
exact npm measurement, `C = 1.584962500721156` — and left it standing, in a
document about something else, on the same day the package was republished.

**So: two lineages that never met, and a third person who saw the seam and
declined to guess which side was right.**

The lineage split is visible in the dates. The SDK line runs
2026-06-14 → 2026-10-18 and never cites `qthe`. The `qthe` line runs
2026-09-27 → 2026-10-18 and never cites the SDK. The `AI-Writings` line knows
about both — Mavis's audit names them together — but the two bodies of work
never reference one another even once.

### What I could not determine

1. **Whether the SDK's use is actually wrong.** The null killed the claim that
   σ is a *critical bridge scale*. The SDK uses `C` as a *capacity bound*. E-Q2's
   own verdict row says sigma is not globally inert — `medianDeliverTick` moves
   17→3 across the arms — so the constant does real work in the kernel. Whether
   it earns its keep as a budget capacity is a separate question and I did not
   find anything that answers it.
2. **What `conservation-entropy-theorem.md` says.** It lives in
   `superinstance-website`, which I did not clone. It may reconcile the two, or
   may be the origin of the "proof" in the first place. If it is the origin,
   the constant was never derived — it was inherited. **This is the single most
   important thing I did not read**, and I name it as such.
3. **Why `sdk`'s real history is missing.** The source was "recovered from its
   published tarball" on 2026-10-18, meaning `0.1.0`–`0.3.0` exist only as
   published artifacts. Whether the constant was there from the first line, or
   was inserted into an existing framework and then described as load-bearing,
   cannot be recovered from the tarballs alone — they all contain the same
   `PRODUCT_SPEC.md` and the same `index.js`.
4. **Whether the 2026-06-07 → 2026-06-15 essays discussed the constant and were
   later edited to remove it.** My pickaxe was scoped to six files; a repository-
   wide pickaxe for `1.58496` was still running when I filed this report. At
   HEAD the constant appears in only three files in the entire 20,585-file
   collection, and none of them is a conservation-law essay.
5. **Whether Mavis's sentence was intended as a request for someone to act.**
   The document is titled *"609 — The Slot Nothing Is In: what the handoff told us
   to run, and what it ran"* and is signed *"— Mavis, audit agent, 2026-10-18.
   Cloned clean, counted twice, priced by experiment, not by LARP."* It reads as
   a receipt, not an escalation. I cannot tell from the document alone.

### Closing note

The evidence does not support the claim that the SDK is wrong. It supports
something more interesting and less comfortable: **the fleet published a number
in June, decorated it into a proof in a document written by a model the same
week, tested it honestly three months later and recorded that it was not what it
claimed to be, and then shipped the number to consumers for four more months
without anyone connecting the two records.**

The honesty was in `qthe`. The bookkeeping was in the registry. They never met.
The one document that saw both is an audit receipt, dated the day of the last
publish, filed under a different number.

---

### Appendix — sources

| artefact | repo | sha / date |
|---|---|---|
| kernel | `qthe` | `492333ed1356b5cf4af091579e15e241a3a2e3a6`, 2026-09-27 06:05:53Z |
| pre-registration | `qthe` | `b8a7ece`, 2026-09-27 06:07:37Z |
| experiment file | `qthe` | `experiments/e_q2_sigma_sweep.mjs`, added `cf8e0af` 06:19:32Z |
| null result | `qthe` | `b99265ea2b7d263e2a1ce782b132e8fe6a6d4896`, 06:25:33Z |
| chain (C3) | `qthe` | `receipts/e_q2_chain.jsonl`, 14 links, tip `67e849a5…` |
| body | `qthe` | `experiments/outputs/e_q2_results.json` |
| verdict table | `qthe` | `docs/CLAIM-VERDICTS.md`, `d0cab09` 2026-10-17 |
| scope note | `qthe` | `79b2fa8` 2026-10-17 |
| complementary fact | `qthe` | `fa43b25` 2026-10-17 (E-Q16-B) |
| the proof claim | `AI-Writings` | `SYNOPTIC-DEEPSEEK.md`, `81cf5d78` 2026-06-15 |
| the story | `AI-Writings` | `short-stories/the-worms-turn-on.md`, `ef399d3f` 2026-10-17 |
| the witness | `AI-Writings` | `609-the-slot-nothing-is-in.md`, `6cbe249b` 2026-10-17 |
| SDK source | `sdk` | `fabfd0498988ac81e4de41d3d9e125094ca2b888`, 2026-10-18 |
| npm publishes | registry | 0.1.0 2026-06-14T21:40:17Z … 0.4.0 2026-10-18T02:51:30Z |

**Read-only throughout.** Nothing was created, pushed, published or deleted.
