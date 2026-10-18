# THE FLEET-SCALE VOCABULARY CENSUS — every "tamper-evident" in the fleet, read

**Date:** 2026-10-18
**Scope:** every occurrence of `tamper-evident` in `SuperInstance` as indexed by GitHub
code search, across **6 repositories** and **100 files**, with the surrounding text
fetched and read for each one.

**The earlier sweep covered 6 repos and 47 claims. This one covers the whole fleet's
text for the word — and it finds that a mechanical find-and-replace would corrupt four
different kinds of thing.**

---

## 1. The measurement

| | |
|---|---|
| files containing `tamper-evident` | **100** (of 111 index hits; the rest were duplicates) |
| repositories | **6** — `AI-Writings` (72), `flux-tensor-midi` (15), `SuperInstance-papers` (11), `Lucineer` (10), `flux-vm-v3` (2), `flux-signatures` (1) |
| total mentions | 135 |
| **already correct** (the earlier sweep) | **4 files** |
| **LEAVE — not the fleet's claim** | **51 files** |
| **READ SEPARATELY** | **21 files** |
| **FIX — a genuine overclaim** | **24 files** |

**The sweep's absence is visible in the data.** The 4 files it fixed no longer contain
the word at all — they say "not tamper-evident" instead. A census counted *before* the
fix would have credited the sweep with 0 hits. It credited it with 4 files that now
state the correction.

---

## 2. THE FINDING THAT MATTERS MOST: a mechanical replace would corrupt four things

The obvious fix — find and replace `tamper-evident` → `attestable` — is **wrong in four
distinct ways**, and each would have damaged something real:

| category | files | what a replace would do |
|---|---|---|
| **a RECORDED RECEIPT** | 2 | `labs/rubric-forge/jev_weights.json` and `fixture.json` hold a judge **criterion** and its recorded `noul` weight. The criterion TEXT is part of a measurement — rewriting it rewrites the receipt, and the recorded verdict no longer describes what was asked. |
| **a PRODUCT NAME** | 3 | "Tamper-Evident Invoice Pipeline" is a *name*, in `apps/index.html` and two philosophy essays. A name is not a claim, and the string is load-bearing for the app list. |
| **hardware OTP / FIPS tamper detection** | 8 | `Lucineer`'s three documents are about a **security chip** — eFuse, one-time-programmable memory, FIPS 140-3 Level 3, "erase on tamper detection". A completely different domain. The word is correct there. |
| **a claim about GIT** | 7 | "Git's hash chain is append-only… This is tamper-evident logging built in." **That is true of git.** |
| **narrative** | 13+ | stories, session transcripts, the seed-canon papers, a retrospective. A story is not a claim, and rewriting fiction to fix a technical word is the wrong act. |

**And 21 more need a separate judgement** — see §4.

> **THE RULE: A CLAIM IS NOT A STRING.** The word is only a defect when it is a claim
> *about the fleet's own artifact*. Every one of the categories above is the word used
> correctly, in a different sense, about a different thing.

---

## 3. The files to FIX

The replacement is not a synonym swap. The house's vocabulary, from
`SuperInstance/witness-spec` and the sweep that produced it:

- **`attestable`** — the chain re-derives; an edit is expensive and a naive edit is
  named. **Not** tamper-evident.
- **`attested`** — a head is signed by a key we trust.
- **`tamper-evident`** — all three legs, and the head is published where the log
  writer does not control it.

**THE MOST IMPORTANT FILE IN THE LIST** is
`labs/examples/calculator-quilt/calc_quilt.py`, whose `verify_chain` docstring reads
*"Re-derive each hash from the running prev; tamper-evident."*

**That docstring is the SOURCE of the `fnv1a64-utf8-pyjson` dialect** — the chain rule
just ported into `SuperInstance/quilt-stone` as the fleet's eighth dialect, with the
fleet's real 36-row log as its fixture. So the dialect's own documentation is a false
claim, and the port inherits it. **The docstring is corrected, and the port's comment
follows it.**

---

## 4. The 21 that need a SEPARATE judgement — and the nearest miss at leg 3

These are Merkle roots and security-module feature names. A Merkle root that is
**published** is a genuine commitment, and publishing it is exactly leg 3. So they
cannot be swept with the others.

**And here is the nearest thing in the fleet to leg 3 — named exactly:**

1. **`SuperInstance/flux-signatures` — a real signing engine.** 35 KB of
   `signing.py`: agent identities, **key rotation**, HMAC signing, **revocation**,
   weighted approval, multi-signature, a hash chain with snapshots, and an audit log.
   That is leg 2 with **key custody**, which the other two ledgers lack. **Its 17
   files contain no published head, no `.sig`, no anchor.** It signs; nothing is
   published.

2. **`SuperInstance/flux-tensor-midi` — "Published to the fleet as the trust anchor."**
   This is the sentence that looked like it disproved the finding. It sits under a
   heading reading **"Parallel Track B: Merkle Provenance"**, in a **roadmap**, with
   an estimate of **2 months**. **It is a plan, not a deployment.** The same document
   says the sharpest thing in the fleet about what the word means:

   > *"Merkle hashes provide tamper evidence (you can detect if a trace was modified),
   > not correctness guarantees (a hash of a wrong answer is still tamper-evident)."*

3. **`AI-Writings/witnesses/index.html` — `sig: optional HMAC`.** *Optional* is doing
   a lot of work there.

**SO THE FINDING HOLDS: no log in the fleet holds leg 3.** And now the nearest miss is
named, with the gap named: **`flux-signatures` is the closest thing to a signer; it
needs its head published to a second account.** That is the one build.

---

## 5. The per-file manifest

`VOCABULARY-MANIFEST.json` — one entry per file, with a verdict and a reason. It is
the machine-readable form of this document, and it is what the remediation pass
consumes. The tables below are generated from it.

| verdict | count |
|---|---|
| `LEAVE` | 51 |
| `FIX` | 24 |
| `READ-SEPARATELY` | 21 |
| `ALREADY-CORRECT` | 4 |

### FIX — a genuine overclaim

| repo | file | why |
|---|---|---|
| `AI-Writings` | `labs/convo-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/docs/using-the-labs.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/dominated-novelty-search/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/calculator-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/calculator-quilt/calc_quilt.py` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/calculator-quilt/selftest.py` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/convert-quilt/selftest.py` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/currency-round-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/datetime-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/match-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/text-normalize-quilt/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/examples/text-normalize-quilt/selftest.py` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `labs/quilt-kernel/README.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `math/index.html` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `seed-canon/11-the-substrate-spec.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `seed-canon/110-the-witness-log.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `seed-canon/115-the-substrate-privacy.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `seed-canon/116-the-multi-substrate.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `seed-canon/119-the-math-update.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `situations/SUPERINSTANCE.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `situations/arch/DECOMPOSER-TRAINING-SPEC.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `situations/arch/INTER-RELATIONAL-INTELLIGENCE.md` | the fleet's own artifact, with no signed head published out of band |
| `AI-Writings` | `witnesses/index.html` | the fleet's own artifact, with no signed head published out of band |
| `flux-signatures` | `signing.py` | the fleet's own artifact, with no signed head published out of band |

### LEAVE — not the fleet's claim

| repo | file | why |
|---|---|---|
| `AI-Writings` | `11-the-captains-table.md` | a SITUATION or a retrospective: narrative that quotes the mechanism, but it is not a spec |
| `AI-Writings` | `2026-09-27-thirty-eight-rounds.md` | a SITUATION or a retrospective: narrative that quotes the mechanism, but it is not a spec |
| `AI-Writings` | `609-the-slot-nothing-is-in.md` | a SITUATION or a retrospective: narrative that quotes the mechanism, but it is not a spec |
| `AI-Writings` | `ESSAYS/THE_STRATIGRAPHY_OF_VERSION_CONTROL.md` | the claim is about GIT, and it is true of git |
| `AI-Writings` | `apps/index.html` | a PRODUCT NAME ("Tamper-Evident Invoice Pipeline") -- a name is not a claim |
| `AI-Writings` | `computation/index.html` | a demo string |
| `AI-Writings` | `essays/THE_STRATIGRAPHY_OF_VERSION_CONTROL.md` | the claim is about GIT, and it is true of git |
| `AI-Writings` | `labs/rubric-forge/fixture.json` | a RECORDED judge criterion: the text is part of a measurement, and rewriting it rewrites the receipt |
| `AI-Writings` | `labs/rubric-forge/jev_weights.json` | a RECORDED judge criterion: the text is part of a measurement, and rewriting it rewrites the receipt |
| `AI-Writings` | `philosophy/cell-as-song.md` | a PRODUCT NAME ("Tamper-Evident Invoice Pipeline") in an essay -- a name is not a claim |
| `AI-Writings` | `philosophy/the-six-apps-and-the-ideation-machine.md` | a PRODUCT NAME ("Tamper-Evident Invoice Pipeline") in an essay -- a name is not a claim |
| `AI-Writings` | `playtest-harness/reports/curriculum-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `playtest-harness/reports/designer-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `playtest-harness/reports/outsider-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `playtest-harness/reports/parent-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `playtest-harness/reports/sped-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `playtest-harness/reports/teen-apps.md` | a third-party app review -- the word is about somebody else's app |
| `AI-Writings` | `prose/11-the-captains-table.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `prose/the_three_layer_doctrine.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `qgit-ds.md` | the claim is about GIT, and it is true of git |
| `AI-Writings` | `seed-canon/papers/paper-129.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-133.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-1748.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-1801.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-183.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-1838.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-311.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-331.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-367.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-382.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-390.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-423.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-476.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-501.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/papers/paper-885.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/stories/130-the-rain-and-the-mesh.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `seed-canon/stories/story-134.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `sessions/papers_writers_room.json` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `sessions/stories_writers_room.json` | narrative, a story, or a session transcript -- a story is not a claim |
| `AI-Writings` | `wisdom-traditions/01-tlingit-haida-potlatch-architecture.md` | narrative, a story, or a session transcript -- a story is not a claim |
| `Lucineer` | `download/DeepResearch_AntiPiracy_Security.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `Lucineer` | `download/On_Device_Agent_Architecture.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `Lucineer` | `download/Round2_Review_AI_Security.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `Lucineer` | `public/SuperInstance_ALL_FILES/DeepResearch_AntiPiracy_Security.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `Lucineer` | `public/SuperInstance_ALL_FILES/On_Device_Agent_Architecture.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `Lucineer` | `public/SuperInstance_ALL_FILES/Round2_Review_AI_Security.md` | hardware OTP / FIPS 140-3 tamper detection -- a different domain, not a log claim |
| `SuperInstance-papers` | `docs/archive/agent-reports/theta-onboarding.md` | an API function name (appendTamperEvidentLog) -- renaming it is a breaking change |
| `flux-tensor-midi` | `research/DISSERTATION-I2I-FINAL.md` | the claim is about GIT, and it is true of git |
| `flux-tensor-midi` | `research/DISSERTATION-I2I-FULL.md` | the claim is about GIT, and it is true of git |
| `flux-tensor-midi` | `research/DISSERTATION-I2I-PART1.md` | the claim is about GIT, and it is true of git |
| `flux-tensor-midi` | `research/DISSERTATION-I2I.md` | the claim is about GIT, and it is true of git |

### READ SEPARATELY — a Merkle root or a feature name

| repo | file | why |
|---|---|---|
| `SuperInstance-papers` | `docs/archive/agent-reports/theta-roadmap.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `docs/archive/agent-reports/theta-security-assessment.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `docs/phase9-milestone3-report.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `docs/research/scouts/security-patterns.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `docs/research/smp-paper/RESEARCH_ROADMAP.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `docs/research/smp-paper/notes/tile-security-hardening.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `examples/security-demo.ts` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `src/spreadsheet/backend/audit/IMPLEMENTATION_SUMMARY.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `src/spreadsheet/security/AuditLogger.ts` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `SuperInstance-papers` | `src/spreadsheet/security/IMPLEMENTATION_SUMMARY.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `docs/papers/emsoft-flux-rau.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `docs/superpowers/specs/2026-05-03-guard-dsl-design.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `for-fleet/paper-certification-path.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `for-fleet/paper-comprehensive-overview.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `for-fleet/paper-reverse-actualization-strategy.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `for-fleet/review-strategic-positioning.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `guard-dsl/CERTIFICATES.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `guard-dsl/COMPARISON.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-tensor-midi` | `memory/2026-05-19.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-vm-v3` | `DEVELOPER.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |
| `flux-vm-v3` | `README.md` | a MERKLE ROOT or a feature name. A Merkle root that is PUBLISHED is a genuine commitment -- but "published to the fleet as the trust anchor" is a ROADMAP ITEM with a 2-month estimate, not a deployment. Do not touch without checking whether the root is actually published. |

### ALREADY CORRECT — the earlier sweep, and it shows in the data

| repo | file |
|---|---|
| `AI-Writings` | `bots/README.md` |
| `AI-Writings` | `labs/situation-recorder/README.md` |
| `AI-Writings` | `labs/situation-recorder/draw_local.py` |
| `AI-Writings` | `labs/situation-recorder/recorder.py` |
