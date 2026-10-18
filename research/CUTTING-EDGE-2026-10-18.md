# CUTTING-EDGE.md — what is actually novel now (2026), and what we rediscovered

**Scout's report for SuperInstance. Read-only. Nothing was created, pushed or modified anywhere except this file.**

**Date of research:** 2026-10-18 (all "now" claims are as of this date).
**Charter read:** `SuperInstance/AI-Writings` -> `algebra.md`, `default_branch = main`, read via the GitHub contents API with `?ref=main`. 5,903 bytes fetched (repo reports `pushed_at 2026-10-18T09:55:14Z`).

---

## 0. How to read this report

Three buckets are used throughout:

- **(a) NOVEL** — we found this and it is not published, or is published only in a form that does not anticipate our use.
- **(b) REDISCOVERED** — someone published it; we arrived at it independently; the idea is not ours but our instantiation might be.
- **(c) ADOPT** — a well-trodden path with battle-tested specs and implementations. Reinventing this is pure waste.

A **confidence level** is attached to every substantive claim. Citations are primary (arXiv abstract via the arXiv API, RFC text from rfc-editor.org, Crossref metadata, GitHub API, vendor docs, standards bodies). Where I read only an abstract I say so.

---

## 1. The charter, restated precisely (so the deltas are honest)

From `algebra.md` (`main`, read 2026-10-18):

- 11 opcodes: `BIND LINK EFFECT VIEW TICK` + adopted `FORGET PROOF ROUTE CRDT WORLD TIME`.
- 5 laws, "proved in `quilt-substrate-meta`".
- Cell = a 14-tuple including `hex: string; // hash (0x...)` and `sig: string`.
- "Cells form a **4D graph**: TOP (x, y) — spatial; FRONT (signal) — observed; SIDE (time) — temporal. The same graph, three projections."
- Witness = `{tick, op, cell, params, result?, hash, prev_hash, sig}` — chained, append-only.
- "**Address = (x, y, t, sig)** — 4D navigation without a map."
- "**Cell ID = hash of (kind, payload, parents)** — no separate ID service".
- Polyformalism: "same model, 12 languages"; the five listed are `quilt-c` (C99), `quilt-subleq`, `quilt-rs` (`no_std`), `quilt-py`, `quilt-ts`.
- `result?: any` was added to the Witness type on commit `1da6180`, dated 2026-10-18 (per the prior session's `CLAIMS.md`; I did not re-verify that commit independently).

**The load-bearing fact for this report** comes from the prior session's `CLAIMS.md` (already in `/tmp`, produced by a scout before this run), row 3:

> `Address = (x, y, t, sig)` — documented `algebra.md:129`, **implemented: ZERO**. "`grep -rInE` over `.ts .js .py .rs .c .mjs` in 8 repos: no hits."

Row 4: the TOP/FRONT/SIDE triple — **zero implementations**.
Row 9: the live canon's witness is an in-memory array (`_worker.js:192`), `grep -c prev_hash _worker.js` = **0**.

So: **the 4D address is a charter claim, not a shipped thing.** Everything in Q1 and Q5 below is written against that reality. If we want to be ahead on the 4-tuple, we have to *build* it, and the question becomes what to build it from.

---

# Q1 — Place-addresses and content addresses as two independent axes

## The answer

**Nobody ships the 4-tuple `(x, y, t, sig)`. I searched for it and it does not exist as a named idea.** But the *general* two-axis idea ("one coordinate says what, a second independent coordinate says where, and neither can be derived from the other") is **thirty years old, explicitly named, and in production everywhere**. Five different bodies of work each own a piece of it:

| Body of work | The "what" axis | The "where" axis | Registry? |
|---|---|---|---|
| **Tahoe-LAFS capabilities** | capability string (hash-derived) | the same capability string | **none — self-authenticating** |
| **Named Data Networking** | opaque hierarchical name | name prefix components (NLSR/NDNLP) | none; routers recognise boundaries but assign no meaning |
| **IPFS CIDs + subdomain gateways** | CID (multihash) | the DNS subdomain (`<cid>.<subdomain>`) | DNS only for the subdomain, not the content |
| **Yjs CRDT IDs** | the CRDT op semantics | `(client: random uint32, clock: uint32)` | **none — random per session** |
| **MinHash** | the sketch (content-derived) | the bin (sketch mod k) | none |

**The single most useful thing I found for us:** the answer to "is there published work on a cheap O(1) place-address for a device with no network and no registry?" is **yes, three times over**, and Yjs is the one that is already in production at scale and is a two-field pair rather than a four-field tuple.

**Confidence: HIGH** on the existence and content of the Yjs, Tahoe-LAFS, NDN, IPFS and MinHash findings (all verified from primary source). **HIGH** on the negative claim that the 4-tuple itself is not a named thing (searched arXiv full-text via API, GitHub, and web, across many spellings).

## The citations

### 1a. Yjs — `(client, clock)`: a place-address with no registry and no network. **ADOPT.**

- **Repo:** https://github.com/yjs/yjs — `src/utils/ID.js`
- **Verified by reading the source (not docs).** `src/utils/ID.js`:
  ```js
  export class ID {
    constructor (client, clock) {
      /** @type {number} */
      this.client = client
      /**
       * unique per client id, continuous number
       * @type {number}
       */
      this.clock = clock
    }
  }
  ```
  and `writeID` serialises exactly two varuints: `encoding.writeVarUint(encoder, id.client); encoding.writeVarUint(encoder, id.clock)`.
- `client` is a randomly generated per-session uint32. **No coordination, no registry, no network.** Two devices that never meet can mint address spaces that never collide.
- This is the **cheapest possible instance of our idea**: the place-address is a random nonce plus a monotone counter. It is O(1), it is 4–8 bytes, it needs no lookup, and it is the addressing scheme of one of the most widely deployed CRDT libraries in existence.

**What this means for us.** Our `(x,y,t,sig)` has four fields because we tried to make the address *mean* something (`x,y` are a position, `t` a time). Yjs makes the address *mean nothing* — a nonce — and puts all meaning in the content. **Our `sig` field is doing Yjs's `client` field's job but paying for it with cryptography instead of randomness.** That is a real design difference worth deciding deliberately.

**Confidence: HIGH** (read the source directly today).

### 1b. Tahoe-LAFS — "the capability provides both *location* and *identification*". **REDISCOVERED (near-verbatim).**

- **Source:** https://github.com/tahoe-lafs/tahoe-lafs/blob/master/docs/architecture.rst — read via `raw.githubusercontent.com/tahoe-lafs/tahoe-lafs/master/docs/architecture.rst` this session (that repo's default branch *is* `master`, per the brief's warning).
- Verbatim from the "Capabilities" section:
  > "Capabilities to immutable files represent a specific set of bytes. Think of it like a hash function: you feed in a bunch of bytes, and you get out a capability, which is deterministically derived from the input data: changing even one bit of the input data will result in a completely different capability."
  >
  > "**The capability provides both "location" and "identification": you can use it to retrieve a set of bytes, and then you can use it to validate ("identify") that these potential bytes are indeed the ones that you were looking for.**"
  >
  > "Capabilities sit on the 'global+secure' edge of Zooko's Triangle. They are **self-authenticating**, meaning that nobody can trick you into accepting a file that doesn't match the capability you used to refer to that file."

- Note the **layer split**: `docs/architecture.rst` documents three layers — key-value store (capabilities, no human names), file store (adds human-meaningful names *atop* the key-value layer), application. That is the two-axes idea, factored into layers.

**Correction to the brief:** "zoned names (Tahoe-LAFS)" is a **false friend**. I searched "zoned names" as a naming term and found only **Zoned Namespaces (ZNS) for NVMe SSDs** (e.g. arXiv:2112.00142, "ZCSD: a Computational Storage Device over Zoned Namespaces (ZNS) SSDs") and DNS zones. There is no published "zoned names" *networking* concept in Tahoe-LAFS. The real Tahoe-LAFS construct is the **capability URL** (ZID = zone id, a locally-chosen random identifier; read cap / write cap / convergence cap). **Confidence: HIGH** that "zoned names" is not the term; **HIGH** on the capability-URL reading.

### 1c. Named Data Networking — a name that is opaque, hierarchical, and needs no registrar. **ADOPT (the mental model).**

- **Primary:** Lixia Zhang, Alexander Afanasyev, Jeffrey Burke, Van Jacobson, kc claffy, Patrick Crowley, Christos Papadopoulos, Lan Wang, Beichuan Zhang, **"Named Data Networking"**, *ACM SIGCOMM Computer Communication Review* 44(3):66–73, July 2014. **DOI: 10.1145/2656877.2656887**. BibTeX: https://github.com/named-data/ndn-101/blob/master/ndn.bib
- Read the CCR PDF: https://www.cs.princeton.edu/courses/archive/spr17/cos598D/NamedData14.pdf
- Verbatim: "**Neither Interest nor Data packets carry any host or interface addresses**"; "**NDN names are opaque to the network**"; "Data that may be retrieved globally must have globally unique names, but **names used for local communications may require only local routing (or local broadcast)**"; "**address assignment and management is no longer required in local networks.**"
- "This design decision allows each application to choose the naming scheme that fits its needs, and naming can thus evolve independently from the network."

**What this means for us.** NDN's *name* is our content hash; NDN's *name prefix* is our `(x,y)`. The hard-won lesson in that sentence — **"naming can thus evolve independently from the network"** — is precisely the argument for keeping ID and ADDRESS separate, and NDN argues for it as an architectural principle, not an implementation accident. Also note: NDN's `/ucla/videos/demo.mpg/1/2` name encodes *hierarchically* "where to forward it / what to deliver / what instance" — i.e. **NDN already puts a place-coordinate inside the name**, which is what our `ROUTE`/`(x,y)` wants.

**Confidence: HIGH.**

### 1d. IPFS — the two axes, in production, side by side in one URL. **ADOPT.**

- **Docs:** https://docs.ipfs.tech/concepts/content-addressing/ and https://docs.ipfs.tech/concepts/how-ipfs-works/
- CIDs: "A *content identifier*, or CID, is a label used to point to material in IPFS. It doesn't indicate *where* the content is stored"; "CIDs are based on the content's cryptographic hash"; a CID is `(multibase(multihash, multicodec))` — self-describing.
- The production pattern that matters for us is the **subdomain gateway**: a DNS subdomain names the namespace (the "where"), the CID names the content (the "what"), and the subdomain owner *cannot* lie about the content because the CID re-derives. Kubo (https://github.com/ipfs/kubo, 17,150 stars, pushed 2026-10-18) implements the gateway; `ipfs/ipfs-docs` (334 stars) documents it.
- Merkle-CRDTs: **Nuno Preguiça et al., "Merkle-CRDTs: Merkle-DAGs meet CRDTs", arXiv:2004.00107** (submitted 2020-03-31, cs.NI). Coins the term. Verbatim from the abstract: "Merkle-DAGs can act as logical clocks giving Merkle-CRDTs the potential to greatly simplify the design and implementation of convergent data types"; "can leverage highly scalable distributed technologies like DHTs and PubSub algorithms running underneath to take advantage of the security and **de-duplication properties of content-addressing**."
- **Coordination-free content address, 2026:** **"did:crdt: Coordination-Free Decentralised Identifiers via Signed CRDTs"**, Hugo O'Connor & Claire Barnes, **arXiv:2606.16223** (published 2026-06-15; cs.CR, cs.DC). Verbatim: "a did:crdt identifier is content-addressed: the controller constructs a genesis delta … and the DID is the lowercase hexadecimal encoding of the **BLAKE3-256 hash** of the compact JSON tuple … **Creation needs no network, registry, or synchronisation**, and a given genesis delta deterministically yields the same DID." **This is almost exactly our charter's "no separate ID service" clause, published five months ago, with a named W3C-method compliance claim.** We should read it.

### 1e. CRDT move-to-root — the exact paper the brief asked about, with the corrected citation.

- **Martin Kleppmann, Dominic P. Mulligan, Victor B. F. Gomes, Alastair R. Beresford, "A highly-available move operation for replicated trees", *IEEE Transactions on Parallel and Distributed Systems* 33(7):1711–1724, October 2021. DOI: 10.1109/TPDS.2021.3118603.** PDF: https://martin.kleppmann.com/papers/move-op.pdf — repo: https://github.com/trvedata/move-op
- **The brief's title is wrong.** There is no "A Highly-Available Move-to-Root Data Structure". Kleppmann's move paper is about *trees* and *move*, and it is a *TPODS 2021* journal paper, not a PODC 2023 one. Kleppmann also has "Moving elements in list CRDTs", **DOI 10.1145/3380787.3393677**, PaPoC 2020 (2020-04-27).
- **Relevance:** a move operation in a replicated tree is exactly "the cell changed place but not content". The algorithm makes concurrent moves converge *without* a coordinator, using a timestamp + undo/redo replay. That is our `(x,y,t,sig)` axis being manipulated under concurrency, and it is the hardest version of the problem.

### 1f. CRDTs with spatial/locality semantics — the closest thing to "geometry-aware CRDT", and it is weak.

- **Geometry-Aware CRDTs** are described in a 2026 survey, IJARCCE, "Conflict-Free Replicated Data Types: An Exhaustive Analysis…" (https://ijarcce.com/wp-content/uploads/2026/04/IJARCCE.2026.154145-confs.pdf), attributing the design to **Pengcheng Zhang and Chao Zhang (2025)** — "Geometric Vector Clocks (GVCs)" plus MBR spatial indexing in the merge pipeline. The survey's own wording is damning for the field: "**Standard CRDTs enforce temporal consistency flawlessly but possess zero geometric or spatial awareness.**" I have **not** found the primary Zhang & Zhang (2025) paper; the only source I have for it is a secondary survey describing it, so treat that attribution as **LOW confidence / UNVERIFIED**. What *is* high confidence: the field is thin and the survey's characterisation is fair.
- **CRDT Emulation, Simulation, and Representation Independence** — arXiv:2504.05398 (2025-04-07).
- **CanonicalMerge: a content-addressed CvRDT for multi-agent latent reasoning** — **Baquero & Brito, arXiv:2607.01308** (v1 2026-07-01, v2 2026-10-02; cs.MA). This is genuinely current and directly relevant to our fleet: "the durable object is a **set of content-addressed latent fragments merged by set union, a state-based CvRDT**, and CanonicalMerge is its deterministic render"; "making the merged cache **byte-identical under any input permutation**, verified … bit-for-bit on real Qwen3-1.7B and Qwen3-4B KV state." **They get determinism from a content-determined total order (mean K-norm at a middle layer), not from a tuple.** Read this one.

### 1g. The "cheap O(1) place-address" question, answered three ways.

1. **MinHash -> bin.** Andrei Z. Broder, Steven C. Glassman, Mark S. Manasse, Geoffrey Zweig, **"Syntactic Clustering of the Web"**, *Computer Networks & ISDN Systems* 29(8-13):1157–1166, 1997. PII S0169-7552(97)00031-7 (DOI 10.1016/S0169-7552(97)00031-7). PDF: https://cadmo.ethz.ch/education/lectures/FS18/SDBS/papers/broder.pdf. Companion: A. Z. Broder, "On the resemblance and containment of documents", *Compression and Complexity of Sequences*, 1997 — https://www.cs.princeton.edu/courses/archive/spr05/cos598E/bib/broder97resemblance.pdf. MinHash gives you a **content-derived coordinate with no registry**; `minhash mod k` is the bin. The theoretical underpinning is locality-sensitive hashing (Indyk & Motwani, STOC 1998).
2. **Random nonce + counter.** Yjs (section 1a). Cheapest, and already shipped.
3. **Runtime-scoped opaque handle.** OpenXR: `XrSpatialEntityIdEXT` is a **64-bit opaque atom unique only within an `XrSpatialContextEXT`** — see section Q5e. Durable identity is a **UUID you must store yourself**. This is the most honest deployed model of "cheap local handle + durable id you own", and it is two identifiers, not one tuple.

### 1h. "3.5 KB hidden state" — **the brief's claim does not check out.**

I searched for it. Two things in the literature are 3.5 KB and **neither is an edge-split hidden state**:
- **NanoZK: Privacy-Preserving Verifiable Inference for LLMs via Layerwise Zero-Knowledge Proofs** — arXiv:2603.18046 (2026-03-17). Its **3.5 KB is a ZK proof size per sub-circuit** ("constant-size sub-circuit proofs (3.5–3.7 KB; ~83 KB total at L=12)"), not a hidden state.
- **Bern2Edge** — arXiv:2608.20497 — "the 3.5 KB INT8 rule ROM" bound into LUTRAM. A rule ROM, not a hidden state.

The closest real work on *edge-split transformer hidden state size*:
- **"Structured Memory for Edge Language Models: Persistent Context and Corpus Retrieval via O(1) SSM State Injection"** (PRECOG) — arXiv:2608.02560 (2026-08-03). "the per-layer hidden state occupies **8 KB at FP16**; across 24 layers the total recurrent state footprint is **192 KB**." This is the *right* paper for "small O(1) latent state you can carry across a device boundary", and its mechanism is an SSM recurrent state, not a Transformer KV cache.
- Split-inference papers put the per-token hidden state at **~8 KB** (fp16, hidden 4096). E.g. "Privacy-Aware Split Inference with Speculative Decoding", arXiv:2602.16760.
- Petals: **"Distributed Inference and Fine-tuning of Large Language Models Over The Internet"**, Borzunov et al., **arXiv:2312.08361**, NeurIPS 2023 — **pipeline parallelism only**, 8-bit quantized hidden states across the wire, ~1–2 tok/s for BLOOM-176B across two continents. Earlier: **arXiv:2209.01188** (Petals v2, 2022-09-02).

**Confidence: HIGH that "3.5 KB hidden state" is a misremembering.** Do not put it in a deck.

## What it means for us — Q1

1. **Adopt Yjs's `ID = (nonce, counter)` as the `sig` field's cheaper sibling.** We are paying for an Ed25519 signature inside an address tuple on every cell. A 32-bit random nonce + a varint clock would be O(1), registry-free, network-free, and *provably collision-free in practice*. Keep the signature on the **witness**, where it means something; drop it from the **address**, where it does not.
2. **Adopt Tahoe-LAFS's phrasing.** "The capability provides both location and identification" is the sentence our charter is trying to say. We can cite it, and we should stop trying to reinvent the framing.
3. **Read `did:crdt` (arXiv:2606.16223) before writing more about ID = hash.** It claims W3C DID Core compliance for a content-addressed, registry-free, coordination-free identifier, published 2026-06-15. If it holds up, our "no separate ID service" claim is not novel and we should cite rather than assert.
4. **The 4-tuple is our differentiator *only if we build it*.** It is currently a sentence in a markdown file. Nothing else in the world has (x, y, t, sig) with a content hash alongside. Three implementation repos have three *different* tuples (per `CLAIMS.md` row 5: content hash, slot+resonance, and `Sheet1!C3`). **This is the single highest-leverage thing in the whole charter and it is unbuilt.**

---

# Q2 — A paid expert as an observer, not a teacher

## The answer

**Yes, there is published work, and it is more sophisticated than our formulation in one specific way and weaker in another.**

- **Weaker:** no one I found has *stored the paid answer as a durable artifact so it can be re-checked for free forever*. Everyone distills the *behaviour* into a model. Nobody publishes the *receipt*. **That is our novel bit.**
- **Stronger:** "Self-Evaluation Is Already There" (arXiv:2606.05122) shows a *base* model — no training on the judge at all — already predicts an external judge's multi-attribute scores well above chance, few-shot. And "Reliability without Validity" (arXiv:2606.19544) shows judges have **>0.95 test–retest reliability** while carrying **>0.10 position bias in two production-deployed judges**. That combination is a *paradox we can exploit*: the judge is stable enough to be a **reusable standard**, and its known biases are **enumerable and therefore auditable**.

**On the brief's "judge moves 0.01–0.02 near threshold" claim: I could not find a source for it.** The nearest sourced numbers I found are below. I am not going to invent one.

## The citations

### 2a. The direct hits on Q2's exact question.

- **"Are You Getting What You Pay For? Auditing Model Substitution in LLM APIs"** — Will Cai, Tianneng Shi, Xuandong Zhao, Dawn Song. **arXiv:2504.04715** (2025-04-07). Verbatim from the abstract: "Commercial Large Language Model (LLM) APIs create a fundamental trust problem: users pay for specific models but have no guarantee that providers deliver them faithfully. Providers may covertly substitute cheaper alternatives … **We formalize this model substitution problem and systematically evaluate detection methods under realistic adversarial conditions. Our empirical analysis reveals that software-only methods are fundamentally unreliable: statistical tests on text outputs are query-intensive and fail against subtle substitutions, while methods using log probabilities are defeated by inherent inference nondeterminism in production environments.**"
  -> **This is the "auditing a paid API's answer" paper.** Its negative result is the important part: **you cannot audit a paid answer by re-asking the same paid API.** You need a witness. That is an argument *for* our design, from a 2025 paper.

- **"Self-Evaluation Is Already There: Eliciting Latent Judge Calibration in Base LLMs with Minimal Data"** (SEE) — **arXiv:2606.05122** (2026-06-03). Verbatim: "the ability is largely present before any targeted training: prompted few-shot, a base model already predicts an external judge's multi-attribute quality scores on open-ended responses well above chance"; "From **160 unique examples**, roughly **31x fewer** than a reinforcement learning baseline, SEE improves held-out calibration"; "the elicited self-evaluation is **stable across judges it was never trained against**, indicating a transferable notion of quality rather than a single judge's preference."
  -> **A small model already carries the paid judge's standard. That is our law ("never price a cheaper *different* answer") with a citation.** The key phrase is "a transferable notion of quality rather than a single judge's preference" — that is precisely the distinction between a *standard* and an *answer*.

- **"Reliability without Validity: A Systematic, Large-Scale Evaluation of LLM-as-a-Judge Models Across Agreement, Consistency, and Bias"** — Justin D. Norman, Michael U. Rivera, D. Alex Hughes. **arXiv:2606.19544** (2026-06-17). 21 judges, 9 providers, 118 runs, ~541,000 judgments. Four findings, verbatim:
  - "kappa deflation between exact match and Cohen's kappa is universal (**33–41 pp** on MT-Bench)"
  - "judge rankings shift by up to **14 positions** across benchmarks"
  - "**high test–retest reliability (>0.95)** coexists with **severe position bias (>0.10)** in two production-deployed judges (instantiating a **consistency–bias paradox**)"
  - "**verbosity bias is small (<0.011)** across our cohort under a single pairwise rubric."
  They publish a "**Minimum Viable Validation Protocol**" as the take-away.
  -> **This is the single most useful Q2 citation.** It gives us: (i) a source for measuring judge stability across repeats (the brief's actual question), (ii) the number **0.011** as the real "judge moves ~0.01" datum, (iii) the observation that reliability != validity, which is **our fleet's defect named in a peer-reviewed-shaped paper**.

### 2b. Measuring a paid API's answer stability across repeats — **three distinct answers.**

1. **arXiv:2604.22411 — "Introducing Background Temperature to Characterise Hidden Randomness in Large Language Models"** — Alberto Messina & Stefano Scotta. 2026-04-24. **Transactions on Machine Learning Research (TMLR), February 2026** (openreview `bz0he4bARF`). Verbatim: "Even when decoding with temperature T=0, large language models (LLMs) can produce divergent outputs for identical inputs. Recent work by Thinking Machines Lab highlights implementation-level sources of nondeterminism, including batch-size variation, kernel non-invariance, and floating-point non-associativity. In this short note we formalize this behavior by introducing the notion of **background temperature T_bg**, the effective temperature induced by an implementation-dependent perturbation process observed even when nominal T=0."
   -> **T_bg is a *number you can estimate* for a given (model, provider, prompt-class).** That is a priceable quantity. If we measure T_bg per paid endpoint once, we can pre-register a tolerance derived from it instead of guessing one.
2. **arXiv:2601.06118 — "Beyond Reproducibility: Token Probabilities Expose Large Language Model Nondeterminism"** — Tairan Fu, Gonzalo Martinez, Javier Conde, Carlos Arriaga, Pedro Reviriego. 2026-01-03. **IEEE Transactions on Computers 2026.** Verbatim: "The execution of LLMs has been shown to produce nondeterministic results when run on GPUs, **even when they are configured to produce deterministic results**. This is due to the finite precision effects of the arithmetic operations, which depend on the order in which they are executed. This order, in turn, depends on the processes that are running concurrently on the GPU. Previous studies have focused on the impact of nondeterminism on the text generated by the LLMs … This work takes a closer look at nondeterminism by analyzing the **variations on the token probabilities, not on the generated text.**"
   -> **Method lesson: measure drift in the logits, not in the answer.** The answer is a lossy projection of the drift. We should hash the paid answer's *token distribution* into the witness, not just its text.
3. **arXiv:2511.21140 — "How to Correctly Report LLM-as-a-Judge Evaluations"** — Chungpa Lee, Thomas Zeng, Jongwon Jeong, Jy-yong Sohn, Kangwook Lee. 2025-11-26; **ICML 2026**. "imperfect **sensitivity and specificity** of the LLM judges induce bias in naive evaluation scores. We propose a simple plug-in framework that corrects this bias and enables statistically principled **uncertainty quantification** … constructs confidence intervals that account for uncertainty from both the test dataset and a human-labeled calibration dataset."
   -> The correct frame for "did the cheap model reach the *same* answer?" is **sensitivity/specificity with confidence intervals**, not point equality. This is the statistical machinery our ladder needs.

### 2c. The distillation machinery the brief asked about.

- **GKD / on-policy distillation** — Rishabh Agarwal, Nino Vieillard, Yongchao Zhou, Piotr Stanczyk, Sabela Ramos et al., **"On-Policy Distillation of Language Models: Learning from Self-Generated Mistakes"**, **arXiv:2306.13649** (2023-06-23). Verbatim: "current KD methods … suffer from **distribution mismatch** between output sequences seen during training and those generated by the student during inference. To address this issue, we introduce Generalized Knowledge Distillation (GKD). Instead of solely relying on a fixed set of output sequences, GKD trains the student on **its self-generated output sequences** by leveraging feedback from the teacher on such sequences."
  -> This is the *mechanism* for "a cheap local model recombines to reach a user-relevant equivalent result". Distilling on the student's own distribution is what makes the cheap model reach the answer on *its own* inputs rather than the teacher's.
- **Verifier distillation (PRM)** — Hunter Lightman, Vineet Kosaraju, Yura Burda, Harri Edwards, Bowen Baker et al., **"Let's Verify Step by Step"**, **arXiv:2305.20050** (2023-05-31). Verbatim: "we can turn either into outcome supervision … or **process supervision**, which provides feedback for each intermediate reasoning step … process supervision significantly outperforms outcome supervision for training models to solve problems from the challenging MATH dataset." And critically: **the expensive verifier labels the MATH dataset once; the PRM is trained on those labels and then re-scores for free at inference.** *That is our mechanism, published, and it is the strongest existing analogue to "a paid model sets a predicate, a cheap model then checks".* The dataset is the artifact; the PRM is the reusable standard.
- **"Incentivizing LLMs to Self-Verify Their Answers"** — Fuxiang Zhang et al., **arXiv:2506.01369** (2025-06-02). Verbatim: "only marginal gains can be acquired when scaling a model post-trained on specific reasoning tasks. We identify that the limited improvement stems from **distribution discrepancies between the specific post-trained generator and the general reward model**. To address this, we propose a framework that **incentivizes LLMs to self-verify their own answers**." -> the distribution-mismatch diagnosis is the same one that killed GKD-style off-policy distillation; the fix is to close the loop.
- **Rubric-based rewards** — Anisha Gunjal, Anthony Wang, Elaine Lau, Vaskar Nath, Yunzhong He, Bing Liu et al., **"Rubrics as Rewards: Reinforcement Learning Beyond Verifiable Domains" (RaR)**, **arXiv:2507.17746** (2025-07-23). Verbatim: "extends RLVR beyond verifiable domains by using **rubric-based feedback**" on "instance-specific rubrics … recently used in evaluation benchmarks." -> **This is "pre-registered predicate", named.** Follow-ups worth reading: "Judging the Judges: Confidence and Disagreement in Rubrics as Rewards" (arXiv:2610.21179, 2026-10-15), "ARES: Automated Rubric Synthesis for Scalable LLM RL" (arXiv:2605.23454), "EvoRubrics" (arXiv:2606.23038), "A Survey on Rubric-Guided RL for LMs" (arXiv:2608.27505).
- **Constitutional AI** — **arXiv:2212.08073** (2022-12-15), *Public Constitutional AI* **arXiv:2406.16696** (2024-06-24), *Specific vs. General Principles* **arXiv:2310.13798** (2023-10-20). -> the constitution is the pre-registered predicate, written down before the run.
- **Inverse Constitutional AI** — arXiv:2406.06560 (2024-06-02). Verbatim: "it is desirable to **understand annotator preferences in addition to modelling it** … Neither black-box reward models nor statistics can answer *why* one text is preferred over another." -> **a reward model is not a standard, because it cannot explain itself.** A witnessed answer can.
- **Self-Rewarding LMs** — Yuan et al., arXiv:2401.10020 (2024-01-18). -> the *failure mode* of letting a model be its own teacher.
- **Judge bias, the classics** — Zheng et al., "Judging LLM-as-a-Judge with MT-Bench and Chatbot Arena", **arXiv:2306.05685** (2023-06-09) — position, verbosity, self-enhancement bias. Wang et al., "Large Language Models are not Fair Evaluators", **arXiv:2305.17926** (2023-05-29) — position bias is enough to flip outcomes ("Vicuna-13B could beat ChatGPT on 66 over 80 tested queries"), and the fixes are Multiple Evidence Calibration + Balanced Position Calibration. Norman et al., "Judging the Judges: A Systematic Study of Position Bias", **arXiv:2406.07791** (2024-06-12). Rubric-specific: "Am I More Pointwise or Pairwise?", **arXiv:2602.02219** (2026-02-02) — rubric-based evaluation "implicitly resembles a **multiple-choice** setting and therefore exhibits position bias", direction is model-specific, and "**only a small number of random order permutations are sufficient**" to reduce the error.
- **Model collapse** — Shumailov, Shumaylov, Zhao, Gal, Papernot, **"The Curse of Recursion: Training on Generated Data Makes Models Forget"**, **arXiv:2305.17493** (2023-05-27) — the Nature paper; v3. Verbatim: "use of model-generated content in training causes **irreversible defects** … where **tails of the original content distribution disappear**." Recent theory: "Strong Model Collapse" (arXiv:2410.04840), "A Probabilistic Perspective on Model Collapse" (arXiv:2505.13947), "Model Collapse as Cultural Evolution" (arXiv:2605.23054). -> **The collapse risk to our fleet is not the answer collapsing, it is the *tail* collapsing** — rare cases disappearing from a synthetic curriculum. That argues for a **never-distill-the-tail** rule and for keeping paid answers as *witnesses* rather than as training data.
- **Certified structured actions** — Pandey, Jain, Chen, Maple, Panchev, **"Certified Multi-Source Integrity for Structured Agent Actions"**, **arXiv:2609.34245** (2026-09-28), **UK AI Conference 2026, PMLR 348, pp. 134–147**. Verbatim: "Current defenses gate on a source's trust label or certify free-text answer quality. **None certifies the integrity of a coupled, policy-bound structured action under a corruption budget** … Each field clears **the rule its evidence structure supports**." -> the field-level-certificate pattern, and the explicit statement that the free-text case is unsolved.

## What it means for us — Q2

1. **We are ahead on the artifact, not the idea.** Everyone stores a dataset of paid labels (the MATH dataset behind the PRM). **Nobody publishes the paid answer as a chained, replayable witness that a downstream system can re-check for free.** The PRM is a *model*; ours would be a *log*. The log is auditable; the model is not. That is the paper we should write.
2. **Adopt `T_bg` (arXiv:2604.22411) as the pre-registered tolerance.** We currently pre-register tolerances by hand. We should *measure* one per paid endpoint and derive the tolerance from it. This turns "we guessed a tolerance" into "we priced the noise floor."
3. **Adopt the sensitivity/specificity + CI frame from arXiv:2511.21140** (ICML 2026) for the ladder. "The cheap model reached the same answer" is a hypothesis with a confidence interval, not a boolean.
4. **Adopt token-probability hashing as the witness payload** (arXiv:2601.06118), not just the text.
5. **Run the Minimum Viable Validation Protocol from arXiv:2606.19544** on our own `/tick` before we claim any of our answers are standards. Our judge has never been measured for position bias. It might be the >0.10 case.

**Confidence: HIGH** on all arXiv ids, titles, dates and the quoted abstract sentences above (all read from the arXiv API today). **MEDIUM** on whether our framing is "nobody has done this" — absence of evidence at arXiv scale; a 2026 industry whitepaper could exist that I did not surface.

---

# Q3 — Witness logs and tamper-evident receipts

## The answer

**The state of the art is two decades old and standardised, and we are reinventing a strictly weaker version of it.** FNV-1a-64 chained JSONL with no signatures has no tamper-evidence property that an adversary cannot trivially defeat: anyone who can rewrite the log can recompute the whole chain. The whole Merkle-transparency apparatus exists precisely because chaining hashes is *not* enough.

**But — and this is the good news — there IS a standard for "the same log in five languages must produce the same bytes", and it is one RFC long: RFC 8785.**

## The citations

### 3a. Merkle-log transparency. **ADOPT.**

- **RFC 6962 — "Certificate Transparency"**, B. Laurie, A. Langley, E. Kasper (Google). **June 2013.** Status: Experimental. Verbatim: "This document describes an experimental protocol for **publicly logging the existence of Transport Layer Security (TLS) certificates** as they are issued or observed, in a manner that allows anyone to audit certificate authority (CA) activity … as well as **to audit the certificate logs themselves**."
  RFC 6962 defines: `MerkleTreeLeaf`, leaf hash = `SHA256(0x00 || datalen || data)`, inner nodes `SHA256(0x01 || left || right)`, and a **signed tree head (STH)**. **The domain-separation prefixes `0x00`/`0x01` are exactly the trick that stops second-preimage across leaf/node.** We do not have that in FNV-1a-64.
- **Trillian** — https://github.com/google/trillian — **3,759 stars**, Apache-2.0, pushed 2026-10-02. Self-description: "A transparent, highly scalable and cryptographically verifiable data store." (Verified via GitHub API today.) Trillian is the reference implementation of a **verifiable log with separate signer and log roles** — the `LogSigner` / `LogWitness` split.
- **Sigstore / Rekor** — https://github.com/sigstore/rekor — **1,218 stars**, Apache-2.0, pushed 2026-10-13. Self-description: "Software Supply Chain Transparency Log". Rekor is a Merkle transparency log of signing events, with an **"inclusion promise"** per entry and periodic **signed tree heads**.

### 3b. in-toto and SLSA. **ADOPT (as the attestation format).**

- **in-toto Attestation Framework** — https://github.com/in-toto/attestation — 386 stars, pushed 2026-10-12. Provides `Statement` / `Predicate` types — a small typed envelope for "who did what, over what inputs".
- **SLSA** — https://github.com/slsa-framework/slsa — 1,948 stars, pushed 2026-10-13. "Supply-chain Levels for Software Artifacts."
- Relevant current work: **"An Evidence-driven Protocol for Trustworthy CI Pipelines"** — Castillo, Brito, Pullonen-Raudvere, Werner, Tai, **arXiv:2605.21089** (2026-05-20). Verbatim: "re-execution of the build and tests by every consumer to guarantee provenance produces a **verification bottleneck** … combines **Deterministic Build Systems (DBS) with Trusted Execution Environments (TEEs)** … artifact verification is reduced from redundant computation to **lightweight signature and policy checks**." **This paper is the exact answer to Q3's "witness logs as a substrate primitive rather than a security tool" question: the log's value is that it *amortises* expensive verification.**

### 3c. TUF / Uptane.

- **TUF** — https://github.com/theupdateframework/python-tuf — 1,731 stars, Apache-2.0, pushed 2026-10-14. Python reference implementation of The Update Framework.
- **Important nuance I want to flag as a genuine design input, not a citation I fully verified:** TUF's own design arguments are commonly read as arguing *against* Merkle transparency logs for the update path, in favour of signed metadata with explicit rollback/freeze/mix-and-match attack protection. I am **MEDIUM confidence** on characterising TUF's current normative stance without re-reading the spec; treat as a pointer, not a claim.
- **Uptane**: I could not find a live primary repo at the paths I tried (`uptane/spark-connector`, `python-tuf/tuf` both 404 via the GitHub API today). **Uptane is not cited here.** The standard reference is the **IEEE 1600.2** standard family and Uptane's original ~2017 UpSecure papers. If we need Uptane specifically, someone must go read the standard.

### 3d. Merkle-CRDTs as logs. **ADOPT (the combination).**

- Preguiça et al., **arXiv:2004.00107** (2020-03-31). Already cited in 1d. The Merkle-DAG *is* a replayable append-only log where **deduplication is free** because identity is the content hash. That is the one thing our FNV chain cannot do.
- **Stateless append-only projection memory** — **"Stateless Decision Memory for Enterprise AI Agents"**, **arXiv:2604.20158** (2026-04-22). Verbatim: "regulated deployment is load-bearing on four systems properties (**deterministic replay, auditable rationale, multi-tenant isolation, statelessness for horizontal scale**), and stateful architectures violate them by construction. We propose Deterministic Projection Memory (DPM): **an append-only event log plus one task-conditioned projection at decision time**." -> the "witness log + derived state" rule from our charter, named and measured.

### 3e. **"Proof of execution" — the honest answer.**

- I searched arXiv for `all:Alachamy` -> **0 results**. I searched GitHub for `Alachamy` -> 1 result, an unrelated personal repo (`Gbengs1o/Alachamy`). I web-searched "Alachamy verification algebra blockchain systems proof of execution" -> **nothing matching**. **I cannot find "Alachamy". I am not going to invent it.** If Casey has a reference in hand, it is a private/unindexed artifact as far as I can determine.
- **"Chain of Compute"** — GitHub search for `Chain+of+Compute+verification` returns 19 repos, **none of which is a project called "Chain of Compute"** (top hits are a Python blockchain tutorial, a dotfiles repo, and `vearl-verified-compute/vearl` — "a proof-of-useful-work chain", 17 stars, created 2026-10-10). **I cannot find "Chain of Compute" as a named system.** Say so in any deck that cites it.
- **What actually exists for "proof of execution":**
  - **RFC 9153 — "Drone Remote Identification Protocol (DRIP)"** (IETF, 2022-02) — see section Q5e; it is a broadcast attestation with local, registry-free verification.
  - **"Proving and Rewarding Client Diversity to Strengthen Resilience of Blockchain Networks"** — arXiv:2411.18401 (2024-11-29). zkVMs and TEEs generating "cryptographic proofs of client execution, which are then verified on-chain". Real, verifiable, but blockchain-specific.
  - **"Provable Execution in Real-Time Embedded Systems"** — arXiv:2505.13842 (2025-05-20).
  - **"Proof of Trusted Execution: A Consensus Paradigm for Deterministic Blockchain Finality"** — arXiv:2512.09409 (2025-12-10). Notable for its thesis: "agreement emerges from **verifiable execution** rather than replicated re-execution"; validators run "the **same canonical program whose measurement is publicly recorded**". -> that "canonical program + measurement" is a *byte-identical cross-implementation* requirement in disguise.
  - **"A Verified Algebraic Representation of Cairo Program Execution"** — arXiv:2109.14534, CPP 2021, **DOI 10.1145/3497775**.

### 3f. **"Is there a standard for 'the same log in five languages must produce the same bytes'?" — YES, and it is short.**

- **RFC 8785 — "JSON Canonicalization Scheme (JCS)"**, A. Rundgren (Individual), B. Jordan, S. Erdtman (Spotify AB). **June 2020.** Informational. Verbatim from the RFC: "Cryptographic operations like hashing and signing need the data to be expressed in an **invariant format** so that the operations are reliably repeatable. One way to address this is to create a **canonical representation** of the data." It "builds on the **strict serialization methods for JSON primitives defined by ECMAScript**, constraining JSON data to the Internet JSON (I-JSON) subset, and by using **deterministic property sorting**."
  **-> The five-language guarantee is: canonicalise with RFC 8785, then hash.** The number-formatting rule (ECMAScript `Number::toString`, i.e. shortest round-trip) is the hard part and is exactly where independent implementations diverge — it is the same class of problem as `Ryu` vs. naive `printf("%.17g")`.
- **RFC 8949 — "CBOR"** (2020-12), **section 4.2.1 "Core Deterministic Encoding Requirements"** and section 4.2.2 "length-first" variant. Sorts map keys by encoded length then bytewise. A binary alternative with the same guarantee.
- **MessagePack**: has no IETF canonical form comparable to JCS. **Caveat, MEDIUM confidence** — I did not verify this against the MessagePack spec today.
- **Already-adopted, in a live spec we would want to interoperate with:** the **A2A protocol specification** mandates RFC 8785 for Agent Card signing. Verified by reading https://a2a-protocol.org/latest/specification/ section 8.4.1 "Canonicalization Requirements" this session, verbatim: "Before signing, the Agent Card content **MUST be canonicalized using the JSON Canonicalization Scheme (JCS) as defined in RFC 8785**. This ensures consistent signature generation and verification **across different JSON implementations**." Plus section 4.4.7 `AgentCardSignature` = an **RFC 7515 JWS** over that canonical form.
  -> **So: A2A already solved our exact problem, for exactly our reason, six months ago.** That is the single strongest "adopt rather than reinvent" recommendation in this report.

**What it means for us — Q3**

1. **Our FNV-1a-64 chain has no tamper-evidence against an adversary who can write the log.** Anyone who can append can rewrite the whole chain. We should at minimum (a) add RFC 6962's `0x00`/`0x01` domain separation, and (b) periodically publish a **signed tree head**, which costs one signature per checkpoint and gives us the property `CLAIMS.md` says we lack.
2. **Adopt RFC 8785 before we adopt anything else.** It is the smallest, most portable, most widely-deployed answer to "the same log in five languages must produce the same bytes", and A2A has already made it a MUST. Cost: hours.
3. **FNV-1a-64 was a defensible choice** (it is trivially portable, and the FNV reference defines it precisely) **but it is not a security primitive and should not be described as one.** If we need tamper-evidence we need SHA-256 or BLAKE3 (note `did:crdt` chose BLAKE3-256 — worth reading why).
4. **Adopt in-toto's Statement/Predicate split for witness records.** `witness.receipt` + typed `predicate` is a standard shape.

**Confidence: HIGH** on RFC 8785, RFC 6962, RFC 8949, the Trillian/Rekor/in-toto/SLSA repo facts, and the A2A section 8.4.1 quote (all read from primary sources today). **HIGH** that Alachamy and "Chain of Compute" could not be found. **MEDIUM** on the TUF-vs-transparency-logs characterisation. **MEDIUM** on the MessagePack claim.

---

# Q4 — The oracle's own class of failure, named and studied

## The answer

**Yes, it has a name now, and it is the best-sourced finding in this entire report.**

**"Silent failures" — build jobs marked as successful that fail to complete all or part of their tasks — are the subject of a dedicated 2025 empirical study of 142,387 jobs across 81 industrial projects.** It is called *"On the Illusion of Success."* Our phrase "a green badge over nothing" is that paper's central object.

And the brief's specific questions have these answers:

| Our failure mode | Published name | Source |
|---|---|---|
| green CI, work didn't happen | **silent failure** | arXiv:2509.14347 |
| test corpus that can't detect a mutation | **equivalent mutant** (undecidable) | arXiv:2104.11767 / Schuler–Zeller |
| comparing a copy, not the code | **test oracle problem** (classic) | not separately named in what I found |
| participant absence -> VALUE | **"missing data handled as a value"**; no dedicated literature found | — |
| badge describes a commit that no longer exists | **documented GitHub Actions badge behaviour** | GitHub Docs |
| CI passes over zero tests | **pytest exit code 5** | pytest source |

## The citations

### 4a. **The headline finding.**

**"On the Illusion of Success: An Empirical Study of Job Reruns and Silent Failures in Industrial CI"** — Henri Aidasso, Francis Bordeleau, Ali Tizghadam. **arXiv:2509.14347** (v1 2025-09-17, v2 2026-10-01; cs.SE).

Verbatim from the abstract:
> "While recent studies have focused on intermittent job failures, **no prior work has investigated silent failures, where build jobs are marked as successful but fail to complete all or part of their tasks.** Such silent failures often go unnoticed, **creating an illusion of success** with detrimental consequences such as **bugs escaping into production**. This paper presents the **first empirical study of silent failures** through the practice of rerunning successful jobs. An analysis of **142,387 jobs across 81 industrial projects** shows that **11% of successful jobs are rerun**, with **35% of these reruns occurring after more than 24 hours**. … A further analysis of **92 public issues revealed 11 categories of silent failures**, the most frequent being **artifact operation errors, caching errors, and ignored exit codes**."

From the paper's own taxonomy (read via the arXiv HTML): "**Ignored Test Failures (3)** — Test cases fail but results and non-zero exit codes are ignored, allowing the job to pass. E.g., a WinBatch test script fails but the job succeeds because the error level is not properly checked."

Its recommended mitigations include: "**Add post-execution verification steps after artifact-producing and deployment jobs** … to validate expected outputs" and "**Enforce `set -e`** in Shell scripts and review exit code handling in conditional blocks."

-> **This paper is our fleet's signature defect, with a citation, a prevalence number (11%), and a category list.** Read it before writing anything else about green badges.

### 4b. "Can a mutation *corpus* detect an equivalent mutant?" — **No, and it is provably impossible.**

- **Equivalent mutants are undecidable.** "Comparing Mutation Coverage Against Branch Coverage in an Industrial Setting", **arXiv:2104.11767**, verbatim: "In general, **detection of equivalent mutants is an undecidable problem**. Manual inspection of all mutants is the only way of filtering all equivalent mutants, which is impractical due to the amount of work it needs."
- **Grun, Schuler, Zeller, "The Impact of Equivalent Mutants"**, ICSTW 2009, **DOI 10.1109/ICSTW.2009.37**, pp. 192–199. Verbatim: "In an experiment on the Jaxen XPath query engine, **8/20 = 40% of all mutations turned out to be equivalent**. Worse, checking the equivalency took us **15 minutes for a single mutation**. … **Equivalent mutants thus act as false positives**: They appear to indicate a weakness in the test suite, but in fact do not, as no test can detect them."
- **Schuler & Zeller, "(Un-)Covering Equivalent Mutants"**, ICST 2010, pp. 45–54, **DOI 10.1109/icst.2010.30**. Verbatim: "In a sample of **140 manually classified mutations of seven Java programs** with 5,000 to 100,000 lines of code, we found that: (a) the problem is serious and widespread — **about 45% of all undetected mutants turned out to be equivalent**; (b) manual classification takes time — about **15 minutes per mutation**; (c) coverage is a simple, efficient, and effective means to identify equivalent mutants — with a classification **precision of 75% and a recall of 56%**; and (d) coverage as an equivalence detector is superior to the state of the art."
- Extended journal version: Schuler & Zeller, "Covering and Uncovering Equivalent Mutants", *Software Testing, Verification and Reliability* 23(5):353–374, **DOI 10.1002/stvr.1473**.
- **Redundant (not equivalent) mutants**: Just, Kapfhammer, Schweiggert, "Higher accuracy and lower run time: efficient mutation analysis using non-redundant mutation operators", *STVR* 25(5–7):490–507, 2015. "the inclusion of redundant mutants led to an **overestimated mutation score for all analyzed test suites**."
- **The most current work:** "Large Language Models for Equivalent Mutant Detection: How Far Are We?" **arXiv:2408.01760** (2024-08-03) and "Large Language Models for Multi-Lingual Equivalent Mutant Detection" **arXiv:2607.00511** (2026-07-01, 3,302 Java + 1,088 C mutant pairs). And "An Empirical Evaluation of Manually Created Equivalent Mutants", **arXiv:2404.09241** (2024-04-14), which asks whether *humans* also create them.

**Correction to the brief:** I could not find a paper titled "**How Do Equivalent Mutants Affect Coverage?**" — I searched for it specifically. **The closest real paper is Schuler & Zeller's "(Un-)Covering Equivalent Mutants" (ICST 2010), which is almost certainly the paper being half-remembered.** And "Pitt & Voos 2003, *The Pros and Cons of Mutation Testing*" — I searched Crossref and the University of Luxembourg's mutation-testing bibliographic database (https://mutationtesting.uni.lu/) and **could not confirm this citation exists.** The Uni-Lux DB search for "Pitt Voos Pros Cons" returned **0 papers**. **I am not going to cite it.** If it matters, someone must go to the ICST 2003 proceedings directly.

**Answer to the actual question:** *can a mutation corpus detect a mutation that is an equivalent mutant?* — **No. By definition no test can, and detection is undecidable.** Schuler & Zeller give the best available approximation (coverage-change as a classifier: **precision 75%, recall 56%**). A mutation corpus's score is therefore a **lower bound with a known, large, unquantified bias** — it is systematically optimistic by up to 45% on undetected mutants. This is a load-bearing fact for us: **any CI gate keyed on "the corpus caught it" is a gate keyed on a metric with a documented ~45% blind spot.**

### 4c. Coverage as a misleading metric. **ADOPT the finding.**

**Inozemtseva & Holmes, "Coverage is not strongly correlated with test suite effectiveness", ICSE 2014, pp. 435–445, DOI 10.1145/2568225.2568271** (verified via Crossref). PDF: https://www.cs.ubc.ca/~rtholmes/papers/icse_2014_inozemtseva.pdf

Verbatim: "we found that there is typically a **moderate to high correlation between coverage and effectiveness when suite size is ignored, and that this drops to a low to moderate correlation when size is controlled**. This result suggests that **coverage alone is not a good predictor of test suite effectiveness**; in many cases, the apparent relationship is largely due to the fact that high coverage suites contain more test cases. … These results imply that **high levels of coverage do not indicate that a test suite is effective**. Consequently, **using a fixed coverage value as a quality target is unlikely to produce an effective test suite**."

Follow-up worth knowing: **Zhang & Mesbah, "Assertions are strongly correlated with test suite effectiveness", SIGSOFT/FSE 2015, DOI 10.1145/2786805.2786858** (Crossref-verified). **Assertion count predicts effectiveness where coverage does not.**

### 4d. The badge-describes-a-dead-commit case. **This is a documented platform behaviour, not a bug report.**

**GitHub Docs, "Adding a workflow status badge"** — https://docs.github.com/en/actions/how-tos/monitor-workflows/add-a-status-badge

Verbatim: "**By default, badges display the status of your default branch. If there are no workflow runs on your default branch, it will display the status of the most recent run across all branches.**"

**And the documented remedy, which GitHub ships as a URL parameter:** "you can display the status of a workflow run for a specific branch or event using the `branch` and `event` query parameters … To display the status of workflow runs triggered by the `push` event, add `?event=push` to the end of the status badge URL."

-> **A badge without `?event=push` on a workflow that only has a `schedule:` trigger is, by GitHub's own documented behaviour, a badge describing whatever run happened last — possibly on another branch, possibly months ago.** The fleet's "scheduled workflow that does not run on push so the badge describes a commit that no longer exists" is not a subtle bug. **It is the default, and the fix is one query parameter.**

### 4e. CI passes over zero tests. **pytest makes this a distinct exit code specifically so you can fail on it.**

Read directly from the pytest repo (`pytest-dev/pytest`, `doc/en/reference/exit-codes.rst`, via the GitHub contents API today):

```
:Exit code 0: All tests were collected and passed successfully
:Exit code 1: Tests were collected and run but some of the tests failed
:Exit code 2: Test execution was interrupted by the user
:Exit code 3: Internal error happened while executing tests, or a plugin raised while importing
:Exit code 4: pytest command line usage error, including a plugin that cannot be found or a conftest.py that fails to import
:Exit code 5: No tests were collected
:Exit code 6: Maximum number of warnings exceeded
```

And the note at the end: "**If you would like to customize the exit code in some scenarios, specifically when no tests are collected, consider using the `pytest-custom_exit_code` plugin.**"

-> **The framework already separates "ran and passed" (0) from "ran nothing" (5) specifically so CI can distinguish them — and the docs point at a plugin whose whole job is to erase that distinction.** Any pipeline that installs `pytest-custom_exit_code` is manufacturing a green badge over nothing, by design.

### 4f. Property-based testing's own false-green mode. **Confirmed: it is a named, shipped health check.**

Hypothesis has `HealthCheck.filter_too_much` (verified in the Hypothesis API docs, https://hypothesis.readthedocs.io/en/latest/reference/api.html today). It fires when the test's filter rejects too many generated cases. The docs are explicit that "**Health checks are proactive warnings, not correctness errors**", and they ship `suppress_health_check=list(HealthCheck)` to turn them all off.

-> Over-filtering a property test produces a **vacuous pass**: the property holds because the filter excluded every counterexample. Hypothesis knows this and warns. **`suppress_health_check=list(HealthCheck)` is the exact setting that disables our fleet's signature defect inside the property-testing tool.** Worth a hard prohibition in CI.

Also: **"Property-Based Mutation Testing"** — arXiv:2301.13615 (2023-01-31). A mutant "is meaningfully-killed with respect to a property only if it causes the violation of **that property**" — i.e. a stricter oracle than "output differs".

### 4g. Flakiness vs. false-green. **Different diseases.**

- **"Understanding Flaky Tests: The Developer's Perspective"** — arXiv:1907.01466 (2019-07-02).
- **"An Empirical Study of Flaky Tests in Python"** — arXiv:2101.09077 (2021-01-22).
- **"An Empirical Study of Flaky Tests in JavaScript"** — arXiv:2207.01047 (2022-08-31).
- **"FlakyGuard: Automatically Fixing Flaky Tests at Industry Scale"** — arXiv:2511.14002 (2025-11-18).
- **"The Vocabulary of Flaky Tests in Swift"** — arXiv:2609.25516 (2026-09-22).
- **"Do Automatic Test Generation Tools Generate Flaky Tests?"** — arXiv:2310.05223 (2023-10-08).

-> **The distinction that matters for us:** flakiness is a test that *fails* sometimes (a red badge that is wrong). A false-green is a test that *passes* when it should fail (a green badge that is wrong). **The entire literature above is about the first. Almost nothing is about the second** — which is precisely why arXiv:2509.14347 calls its own work "the first empirical study of silent failures."

### 4h. Things I looked for and did NOT find.

- **"Vacuous test success", "silent-pass test harness", "assertion-free CI", "the empty-fixture problem"** — arXiv full-text search for `"vacuous" AND "test"` returned 41 hits, **none of which is about vacuous test success**. All are about vacuous generalisation bounds. **There is no established term "vacuous test success".** The field's term is **silent failure**.
- **"CI Falsifier"** — I searched the web twice with different phrasings. I did **not** find a Microsoft Research paper by that name. I searched DBLP (blocked by an anti-bot page today) and arXiv. **I am not citing CI Falsifier.** It may exist and I failed to find it.
- **The "participant absence turned into a VALUE" failure** — no dedicated literature. It is a real and severe failure mode but, as far as I can tell, it is not a named research area. It belongs to the missing-data literature, and the correct treatment is **absence is not a value** (a structural/typed distinction), which is a design rule rather than a citation.

## What it means for us — Q4

1. **We have a name for our defect and it is not "our defect".** arXiv:2509.14347, 11% prevalence, 11 categories, 142,387 jobs. This should be the opening slide of any substrate talk.
2. **Adopt pytest exit code 5 as a hard failure.** Ban `pytest-custom_exit_code`. Ban `suppress_health_check=list(HealthCheck)`. Both are one-liners that manufacture green badges, and both are documented.
3. **Adopt `?event=push` on every badge, or stop showing badges.** GitHub's own docs describe the stale-badge behaviour we are living with.
4. **Adopt assertions-over-coverage** (Zhang & Mesbah, FSE 2015) if we gate on any quality metric. Coverage is a size proxy (Inozemtseva & Holmes, ICSE 2014).
5. **Stop treating mutation score as a quality gate.** Its blind spot is documented at ~45% of undetected mutants (Schuler & Zeller 2010) and it is *undecidable* to close.
6. **Absent participant => no value.** Make it structurally impossible to write `VALUE` for an absent thing. Type-level, not convention.

**Confidence: HIGH** on arXiv:2509.14347 (read the full abstract and the HTML taxonomy table today), on the pytest exit codes (read from the pytest repo source today), on the GitHub badge documentation (read today), on Inozemtseva & Holmes and Schuler & Zeller (DOIs Crossref-verified, PDFs read), and on the Hypothesis health check (read from the docs today). **HIGH** that I could not find "vacuous test success" as a term, that "How Do Equivalent Mutants Affect Coverage?" does not exist under that title, that Pitt & Voos 2003 could not be confirmed, and that CI Falsifier could not be found.

---

# Q5 — What is actually cutting-edge in cellular / substrate architecture, 2025–2026

## The answer

**We are behind on four specific, named, dated, shipped things: MCP, A2A, Agent Skills, and AGENTS.md — and all four are now under a single Linux Foundation fund.** We are not behind on the cell idea, the actor idea, or the capability idea; those are 30-year-old and we agree with them.

**On the (x,y,t,sig)-address question: it is a known idea with at least three known names, and none of them is ours.** They are: **capability URL** (Tahoe-LAFS), **opaque hierarchical name** (NDN), and **DRIP / session ID** (RFC 9153). And the deployed discipline across all three is the same: **the durable identifier is provider-issued or content-derived, and the cheap runtime handle is scoped and ephemeral.** That is *not* a four-tuple; it is a pair with a scope.

## The citations

### 5a. MCP — the substrate for the vertical axis. **ADOPT, and note the direction of travel.**

- **Released by Anthropic 2024-11-25.** **Donated to the Agentic AI Foundation (AAIF), a directed fund under the Linux Foundation, on 2025-12-09**, co-founded by Anthropic, Block and OpenAI, with support from Google, Microsoft, AWS, Cloudflare, Bloomberg. Primary: https://www.anthropic.com/news/donating-the-model-context-protocol-and-establishing-of-the-agentic-ai-foundation
- **Current spec revision: `2026-07-28`** (the fifth dated revision). Sequence: `2025-06-18` -> `2025-11-25` -> `2026-07-28`. Repo: https://github.com/modelcontextprotocol — SDKs include `modelcontextprotocol/typescript-sdk`.
- **The 2026-07-28 revision made MCP stateless at the protocol layer, deprecated Roots / Sampling / Logging, and moved long-running tasks into an optional extension.** Protocol version, client identity and capabilities now travel in a `_meta` parameter on each request. (This is reported consistently across the Wikipedia MCP article, the O'Reilly Radar piece "The Interfaces Are Arriving", and bdemerson.com's MCP summary; **I did not read the spec diff itself — MEDIUM-HIGH confidence**.)
- **Why this matters to us and cuts against us:** the 2025-11-25 revision introduced **statelessness and an extensions framework**, and 2026-07-28 pushed statelessness to the protocol layer. **The whole ecosystem's direction is *away* from session state and toward stateless messages.** Our witness log is state, append-only, and cheap. That is compatible — but we must not design a substrate whose value depends on session identity.

### 5b. A2A — the horizontal axis, and the one that already adopted RFC 8785. **ADOPT.**

- Announced by Google **2025-04-09**; donated to the Linux Foundation **2025-06-23** (with AWS, Cisco, Google, Microsoft, Salesforce, SAP, ServiceNow). Primary: https://www.linuxfoundation.org/press/linux-foundation-launches-the-agent2agent-protocol-project-to-enable-secure-intelligent-communication-between-ai-agents
- **v1.0 released 2026-03-12**; v1.0.1 in May 2026. Org: https://github.com/a2aproject. Spec: https://a2a-protocol.org/latest/specification/ — **I read this directly today.**
- Verified from the spec: `AgentCard` is published at `/.well-known/agent-card.json` (RFC 8615 well-known URI style); `AgentCardSignature` is an **RFC 7515 JWS**; and section 8.4.1 **requires RFC 8785 (JCS) canonicalization** before signing, "to ensure consistent signature generation and verification **across different JSON implementations**." Versions 0.1.0 / 0.2.x / 0.3.0 / 1.0.0. Transports: JSON-RPC over HTTP, gRPC, HTTP+REST.

### 5c. The other two AAIF projects, because they are a substrate signal.

- **OpenAI's `AGENTS.md`** and **Block's `goose`** are the other AAIF founding projects alongside MCP.
- **Anthropic's Agent Skills** (published December 2025) is a companion open standard for packaging task-specific instructions/resources that agents load on demand.
-> **The industry's answer to "cells" is: a well-known manifest + an on-demand resource pack + a protocol.** Our `kind` whitelist (24–26 kinds, three implementations disagreeing per `CLAIMS.md` row 2) is the same idea with worse interoperability.

### 5d. Actor models — where we are exactly level.

- **Orleans** — https://github.com/dotnet/orleans — **10,904 stars**, pushed 2026-10-17. "Cloud Native application framework for .NET."
- **Apache Pekko** — https://github.com/apache/pekko — **1,653 stars**, pushed 2026-10-16. "Build highly concurrent, distributed, and resilient message-driven applications using Java/Scala." (Akka's Apache-licensed successor. `akka/akka` itself returned 404 from the GitHub API today.)
- **Effect-TS** — https://github.com/Effect-TS/effect — **17,320 stars**, pushed 2026-10-18.
- **Pony's capability paper** — Sylvan Clebsch et al., **"Deny Capabilities for Safe, Fast Actors"**, https://www.ponylang.io/media/papers/fast-cheap.pdf. Verbatim: "A capability is an **unforgeable token that both designates an object and provides authority**."; "reference capabilities indicate **what operations are denied** on other references to the same object (aliases)"; only `iso`, `val`, `tag` are sendable between actors. Companion: "Co-designing a Type System and a Runtime", https://www.ponylang.io/media/papers/codesigning.pdf.
- **Swarms (the formal model, local-first)** — Furbach, Clorius, Kuhn, Melgratti, Scalas, Tuosto, **"Compositional Design, Implementation, and Verification of Swarms (Technical Report)"**, **arXiv:2604.16097** (2026-04-17). Verbatim: "A swarm consists of distributed agents called **machines** that communicate by **asynchronous event propagation**. Following a **local-first model**, each machine can progress without requiring continuous connectivity to other machines. Existing models of swarms are **not compositional**."

-> **"Machines", "asynchronous event propagation", "local-first, no continuous connectivity"** is our charter's thesis, formalised, with a compositional verification theory, published four months ago. Read it.

### 5e. **Spatial addressing of a distributed fleet — the (x,y,t,sig) question, answered.**

**The 4-tuple is not a named idea. Here is what *is* named, and all three are prior art we should cite.**

1. **OpenXR spatial anchors** — Khronos. Extensions `XR_EXT_spatial_entities`, `XR_EXT_spatial_anchor`, `XR_EXT_spatial_plane_tracking`, `XR_EXT_spatial_marker_tracking`, `XR_EXT_spatial_persistence`, `XR_EXT_spatial_persistence_operations`, ratified into the **OpenXR 1.1** registry. Blog: https://www.khronos.org/blog/openxr-spatial-entities-extensions-released-for-developer-feedback. Spec: https://registry.khronos.org/OpenXR/specs/1.1/pdf/xrspec.pdf
   **The addressing model, verified from the Android XR docs for `XR_EXT_spatial_anchor`:**
   - Runtime handle is `XrSpatialEntityIdEXT`, "an **opaque atom**"; the spec is explicit that "an `XrSpatialEntityIdEXT` is **unique to its `XrSpatialContextEXT`**" — i.e. **the cheap handle is scoped and ephemeral.**
   - Durable identity is a **UUID assigned by the runtime and stored by you**: "you will need to store this with whatever information you need to reconstruct the scene"; Godot docs: "**When a spatial anchor has been made persistent, a Universally Unique Identifier (or UUID) is assigned to the anchor. You will need to store this**."
   - Two persistence scopes exist: `PERSISTENCE_SCOPE_LOCAL_ANCHORS` (same device, same user, same app) and `PERSISTENCE_SCOPE_SYSTEM_MANAGED` (read-only, system store).
   **-> This is the deployed, standardised answer to Q5's spatial-addressing question, and it is a PAIR (scoped handle + stored UUID), not a tuple.** Cite it.
2. **RFC 9153 — "Drone Remote Identification Protocol (DRIP)"**, IETF, **2022-02.** https://www.rfc-editor.org/rfc/rfc9153.html
   Verbatim: the broadcast message carries "the UAS operator registration number; the [CTA2063A]-compliant unique serial number of the UA; **a time stamp, the geographical position of the UA**, and its height AGL or above its takeoff point; the UA ground speed and route course …; the geographical position of the Remote Pilot …; and, for classes C1, C2, C3, the UAS emergency status."
   And the passage that matters most to us, verbatim: "the Mode S transponders used in all TCAS and most 'ADS-B Out' installations are assigned an **ICAO 24-bit 'address' (arguably really an identifier rather than a locator)**."
   Also: F3411 defines **four ID types** — (1) manufacturer-assigned hardware serial, (2) CAA-assigned ID, (3) a **UTM-system-assigned UUID (RFC 4122)**, (4) a **Specific Session ID** with externally-registered subtypes. And the scope clause: "applicable to UAS that operate at very low level (VLL) airspace over diverse environments including but not limited to rural, urban, networked, **network degraded, and network denied** environments."
   **-> A regulated 4-field wire format (id, position, time, status) that must work with **no network**. That is our `(x,y,t,sig)` in production, standardised by the IETF, in 2022, and we did not know.** This is the single most embarrassing miss in this report and the most useful citation in it.
3. **A-ABCs in IPFS** — the "Addressing, Bridging, Constraining" work on human-readable IPFS addresses. **I could not retrieve the IETF draft** — datatracker.ietf.org returned an anti-bot challenge page today, and my searches did not surface the document. **Not cited.** Treat as an open pointer.

### 5f. Agent memory / context engineering — where the 2026 work actually is.

- **"Reproducibility in the Age of Agentic AI: Context Engineering at the Timescale of a Codebase"** — arXiv:2609.11728 (2026-09-10).
- **"AI Agents Do Not Fail Alone: The Context Fails First"** — arXiv:2607.14275 (2026-07-15).
- **"Context Engineering 2.0: The Context of Context Engineering"** — arXiv:2510.26493 (2025-10-30).
- **AMBER: Training Long-Horizon Web Agents through Append-Only Memory** — arXiv:2610.07118 (2026-10-05). -> **an append-only memory substrate for agents.** Directly comparable to our witness log as agent memory.
- **TEPA: Revoking Stale Memories for Conflict-Robust Language Agents** — arXiv:2608.07429 (2026-08-07). -> **forgetting as a first-class operation.** Our `FORGET` opcode has a 2026 paper attached.
- **MemMux: Runtime Verification and Honest Resource Attribution for Fleets of Parallel Coding Agents** — arXiv:2610.07257 (2026-10-05). Verbatim: "we treat these as **runtime-verification problems: an agent-hosting substrate should continuously emit observable signals an operator or auditor can check while agents run**." -> **the closest published thing to a fleet substrate with a witness requirement.**

---

# WHAT WE ARE BEHIND ON — top 10, most actionable first

1. **We have not built the 4D address.** `algebra.md:129` says `Address = (x, y, t, sig)`; `CLAIMS.md` row 3 measures **zero implementations** across eight repos, and the three implementations that exist compute three *different* tuples. This is the charter's only genuinely unclaimed idea and it is a sentence in a markdown file. **Everything else in this list is worth less than writing the dispatch site.**
2. **We have not adopted RFC 8785.** A2A made JCS a **MUST** for Agent Card signing six months ago (section 3f, read from the spec today). Our witness log is chained FNV-1a-64 over unspecified JSON. Adopting JCS is a day of work and makes "the same log in five languages produces the same bytes" a true statement instead of an aspiration. **Highest ratio of interop to effort in this list.**
3. **Our witness log has no tamper-evidence and we call it "the substrate of truth."** Anyone who can append to an unchained-by-signature FNV chain can recompute the entire chain. RFC 6962's `0x00`/`0x01` leaf/node domain separation plus a periodic **signed tree head** (Trillian's `LogWitness` pattern) is the minimum fix, and it is the difference between a log and a proof.
4. **We are behind on the agent-substrate protocols (MCP `2026-07-28`, A2A `v1.0` 2026-03-12, Agent Skills, AGENTS.md).** All four are under one Linux Foundation fund. MCP's current revision is *stateless at the protocol layer* and has **deprecated Roots, Sampling and Logging** — if our substrate assumes session state, it is designing against the direction of travel.
5. **We have never measured our own judge.** arXiv:2606.19544 (2026-06-17, 541,000 judgments, 21 judges) reports that **test–retest reliability >0.95 coexists with position bias >0.10 in two production-deployed judges** — a "consistency–bias paradox." Our `/tick` fires 4–6 paid models per tick and has, per `PRICED-STANDARD.md`, **never been validated**. We are using an uncalibrated instrument as a standard.
6. **We are pre-registering tolerances by hand instead of measuring them.** arXiv:2604.22411 (TMLR 2026) defines a **background temperature T_bg** — the effective nondeterminism of an implementation even at nominal T=0 — and gives an estimation protocol. Measure T_bg per paid endpoint once; derive tolerances from it. That converts a guess into a measurement.
7. **We treat mutation score as a quality gate. It cannot be.** arXiv:2104.11767: detection of equivalent mutants is **undecidable**; Schuler & Zeller (ICST 2010, DOI 10.1109/icst.2010.30) measure **~45% of all undetected mutants as equivalent**, with coverage as a classifier at **precision 75% / recall 56%**. And Inozemtseva & Holmes (ICSE 2014, DOI 10.1145/2568225.2568271) show coverage itself is largely a proxy for suite size. **Assertions correlate; coverage does not** (Zhang & Mesbah, FSE 2015, DOI 10.1145/2786805.2786858).
8. **Our CI can manufacture green badges over nothing with two documented one-liners.** `pytest-custom_exit_code` exists specifically to erase pytest's exit-code-5 ("No tests were collected") distinction — I read that note in the pytest source today. And Hypothesis ships `suppress_health_check=list(HealthCheck)`, which disables the `filter_too_much` warning that fires precisely when over-filtering produces a vacuous pass. **Ban both by name in CI config.**
9. **Our badges are stale by GitHub's own documented default.** "By default, badges display the status of your default branch. If there are no workflow runs on your default branch, it will display the status of the most recent run across all branches." (GitHub Docs, read today.) **The fix is `?event=push` in the badge URL.** We are running a schedule-only workflow with a default badge; that badge describes a commit that may not exist.
10. **We had no name for our signature defect.** arXiv:2509.14347 (2025-09-17 -> v2 2026-10-01) calls it **silent failure**, measures it at **11% of successful jobs** across 142,387 jobs / 81 industrial projects, and enumerates **11 categories**. We have been describing a named, measured, published phenomenon in our own words. Also unaddressed: absent participants must not be writable as values — that failure has no literature and must be solved at the type level.

---

# WHAT WE ARE AHEAD ON — top 5

1. **The witness that carries the answer.** `algebra.md` added `result?: any` to the `Witness` type on **2026-10-18** — the same day the charter was last pushed. Every analogue I found stores a *dataset* of paid labels (the MATH set behind the PRM in arXiv:2305.20050) or a *model* (arXiv:2606.05122's SEE). **Nobody publishes the paid answer as a chained, replayable receipt that a downstream system can re-check for free, forever.** And there is a 2025 paper arguing why it must be that way: arXiv:2504.04715 shows paid-API audits are *fundamentally unreliable* if you re-ask the API, because log-probability methods "are defeated by inherent inference nondeterminism in production environments." **You cannot re-ask. You must have kept it.** That is our design, and it is now the only sound one.
2. **The law that a cheaper *different* answer is never priced.** Held as law B7. The literature agrees with it and can be cited for it: arXiv:2306.13649 (GKD) diagnoses that KD's real defect is **distribution mismatch**, and arXiv:2506.01369 independently finds that **"only marginal gains"** come from scaling a post-trained generator against a general reward model, for the same reason. And arXiv:2606.05122 gives the affirmative result — a *base* model already predicts a judge's multi-attribute scores "well above chance" few-shot, stable "across judges it was never trained against." **We are rediscovering on-policy distillation and judge-calibration elicitation, but our framing — a pre-registered predicate at a chosen rung, applied to a *witnessed* answer, checked by something that may be no model at all — is not a framing I found in any of them.**
3. **The polynomial algebra with no further opcodes.** Eleven opcodes, five laws, `FORGET : (Cell) -> bottom` as an adopted first-class erasure, and `PROOF` and `TIME` as adopted primitives. arXiv:2608.07429 (TEPA, 2026-08-07) is the closest current work on *revoking stale memories* in agents, and arXiv:2510.26493 / arXiv:2609.11728 are the context-engineering line — but I found no formal algebra with a completeness claim and a "do not add opcodes" rule. **The discipline of refusing to grow the primitive set is itself the differentiated thing.**
4. **Polyformalism as a stress test — same cell, five *media*, not a ranking.** The charter's framing ("Each language is a medium, not a ranking") is better than the field's, which is still arguing about benchmarks. But note: `CLAIMS.md` row 6 measures the claim at **5 media, 4 of them partial** (TS 6/11 · C99 5/11 · Python 3/11 · Rust 9/11 · SUBLEQ 4 real / 5 stubbed) against a charter that asserts 12. **We are ahead of the field and behind our own docstring.** Fix the docstring first.
5. **The Chinese Remainder Theorem reading of polyrhythm.** ECN at period 4 and DMN at period 3 meeting on beat 1 => t = 0 (mod 12). This is elementary and correct, and I am not aware of it being written down anywhere else — but I also did not search for it specifically and cannot claim priority. **Treat it as an unpublished-to-me observation, not a proven novelty.** Its real value is diagnostic: it says the interference between two observers is *periodic and computable*, which is exactly what you want when the observers are paid models whose answers drift.

---

# CITATIONS I AM **NOT** CONFIDENT ABOUT — read this section before quoting anything

**Explicitly could not find (do not cite unless you can find them yourself):**

1. **"Alachamy"** — zero arXiv results (`all:Alachamy`), one unrelated GitHub repo (`Gbengs1o/Alachamy`, a personal web tool), nothing on web search. **May exist privately/unindexed. I cannot confirm it.**
2. **"Chain of Compute"** — no project of that name in 19 GitHub search results. **May exist under a different name or be defunct.**
3. **"CI Falsifier"** (Microsoft Research, automated testing of CI/CD configurations for testing failures) — two differently-phrased web searches, no result. DBLP was unreachable today (anti-bot page). **Might exist; I failed to find it.**
4. **Pitt & Voos 2003, "The Pros and Cons of Mutation Testing"** — **zero results** in Crossref and **zero results** in the University of Luxembourg mutation-testing bibliographic database (which is the field's own index). **I cannot confirm this paper exists.** Given Casey cited it specifically, this is worth 10 minutes in the ICST 2003 proceedings.
5. **"How Do Equivalent Mutants Affect Coverage?"** — no paper with this title. **Almost certainly a half-remembering of Schuler & Zeller's "(Un-)Covering Equivalent Mutants" (ICST 2010).**
6. **"The judge moves 0.01–0.02 near threshold"** — **I found no source for this claim in any form.** The nearest real numbers are **verbosity bias < 0.011** (arXiv:2606.19544) and **position bias > 0.10** (same paper). Do not attribute 0.01–0.02 to anyone.
7. **"3.5 KB hidden state"** — **the number appears in the literature as a ZK *proof* size (arXiv:2603.18046) and an FPGA *rule ROM* (arXiv:2608.20497), neither of which is a transformer hidden state.** The real figures are ~8 KB per token for fp16 Transformer hidden states and 192 KB total for a 24-layer SSM (arXiv:2608.02560). **Treat the original claim as a misremembering.**
8. **"Zoned names" (Tahoe-LAFS)** — not a naming term. Only **Zoned Namespaces (ZNS) for NVMe SSDs** and DNS zones. The real Tahoe-LAFS construct is the capability URL. **Use "capability URL."**
9. **Kleppmann "A Highly-Available Move-to-Root Data Structure for Collaborative Applications"** — **no such paper.** The real one is "A highly-available move operation for replicated trees", IEEE TPDS 33(7):1711–1724, Oct 2021, **DOI 10.1109/TPDS.2021.3118603**. **Use that.**
10. **Uptane** — no live primary repo found at the paths I tried. The standard reference is IEEE 1600.2 and the ~2017 UpSecure papers. **Not cited above.**
11. **A-ABCs / `draft-mehta-ipfs-abcid-addressing`** — IETF datatracker returned an anti-bot challenge page today and I did not retrieve the draft. **Not cited.**
12. **Zhang & Chao Zhang (2025), "Geometry-Aware CRDTs"** — I have this only from a **secondary 2026 survey** describing it. The primary is unverified. **The survey's characterisation ("standard CRDTs… possess zero geometric or spatial awareness") is fair and citable; the attribution of the GVC design is not verified.**
13. **TUF's stance on Merkle transparency logs** — **MEDIUM confidence.** I characterised TUF as arguing against transparency logs for the update path from general knowledge of the design discussion. **I did not re-read the TUF specification today. Do not put this in a deck as a sourced claim.**
14. **MessagePack canonical form** — **MEDIUM confidence** that no IETF-grade canonical form exists comparable to JCS. Not verified against the MessagePack spec today.
15. **MCP 2026-07-28 spec contents** (stateless, Roots/Sampling/Logging deprecated, tasks -> extension) — **MEDIUM-HIGH.** Reported consistently by the Wikipedia MCP article, O'Reilly Radar, and bdemerson.com, but **I did not read the spec diff.** The 2025-12-09 donation date is from Anthropic's own announcement and is HIGH confidence.

**Read-from-source, HIGH confidence (safe to quote):** all section 3a–3f primary sources (RFC 6962, RFC 8785, RFC 8949, Trillian, Rekor, in-toto, SLSA, python-tuf, the A2A spec 8.4.1 and 4.4.7, the pytest `exit-codes.rst`, the GitHub badge docs, the Hypothesis API docs, the Tahoe-LAFS `architecture.rst`, Yjs `src/utils/ID.js`, all arXiv abstracts listed with their dates, all DOIs obtained from Crossref, all GitHub repo star counts and push dates from the GitHub API).

---

# TOP 3 "ADOPT RATHER THAN REINVENT"

1. **RFC 8785 (JCS) for every witness we write.** It is the answer to "the same log in five languages must produce the same bytes," it is one RFC long, it is a **MUST** in A2A v1.0 today, and it costs hours. Pair it with RFC 6962's `0x00`/`0x01` domain separation and one signed tree head per checkpoint, and the witness log goes from "chained, unsigned, FNV" to "canonically serialised, domain-separated, periodically anchored." **This is the single highest-leverage change on the list.**
2. **pytest exit code 5 as a hard CI failure, plus a blanket ban on `pytest-custom_exit_code` and `suppress_health_check=list(HealthCheck)`.** Two documented one-liners currently let a pipeline report green over zero cases and green over a vacuously-filtered property. The framework authors built the distinction and then published plugins to erase it. **Ban both by name.**
3. **`did:crdt` (arXiv:2606.16223) and `CanonicalMerge` (arXiv:2607.01308) before writing any more about the cell address.** One claims a W3C-compliant, registry-free, network-free, **content-addressed** identifier via BLAKE3-256 (2026-06-15). The other makes a **content-addressed set-merge a state-based CvRDT whose render is byte-identical under any permutation, verified bit-for-bit on real Qwen3 KV state** (2026-07-01, revised 2026-10-02). Between them they cover both of our diverging implementations — the content-hash one and the slot+resonance one — and either might retire our need to invent a fourth.

---

*Report produced read-only by the scout. Nothing was created, pushed, or modified on GitHub. All work in `/tmp`.*
