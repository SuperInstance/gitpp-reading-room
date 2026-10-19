# LEG 3 — THE ANSWER, AND THE NEAREST MISS IS NOT THE ONE WE NAMED

**Date:** 2026-10-19
**Question:** of the 21 files the vocabulary census left for a separate
judgement, is any root actually PUBLISHED where the log writer does not
control it?

---

## 1. THE ANSWER: NO. 0 of 21.

| verdict | files |
|---|---|
| **PLANNED** — a roadmap item, a build plan, a "Parallel Track" | **6** |
| **LOCAL** — computed, stored beside the log, published nowhere | **15** |
| **ALREADY CORRECT** — the claim is honest about being local or planned | **1** |
| not a fleet claim | **1** |
| **PUBLISHED OUT OF BAND** | **0** |

**Checked against all reachable git history of the repos the 21 live in, plus
a 21-repo tree sweep of every `witness|ledger|receipt|seal|proof|notar`
repository in the account.** No anchor path. No `.sig`. No `witness-heads/`.
No publish job.

**The one confirmed case checks out.** `flux-tensor-midi`'s *"Published to
the fleet as the trust anchor"* sits under **Parallel Track B** inside
**§8 The Build Plan: Year 1 → Months 1–3**. It is a plan. (A correction to
the earlier census: Track B is **1 month**; the 2 months is Track A.)

**And two files were ALREADY CORRECT and must be left alone** — one is a
*critique* of another paper's Merkle claim, inside a section headed
"Factual Errors"; the other sits under a heading literally named
"Research Scope". **A claim that correctly says "this is a plan" is not a
defect.**

---

## 2. THE NEAREST MISS IS NOT `flux-signatures`

The earlier census nominated `SuperInstance/flux-signatures` as "leg 2
**with key custody**". **That is wrong**, and it was wrong in a way that
mattered, because it is the difference between a near-miss and a
not-near-miss.

**Reading all 932 lines of `src/signing.py`:**

1. **THERE IS NO KEY PAIR.** `public_key = _sha256(private_key)` — **SHA-256
   *of the secret***. It appears in six places and **verifies nothing**.
   `AgentIdentity.verify` uses `private_key` and `previous_keys`. **It is
   HMAC: the verifier must hold the secret.** There is no asymmetric
   verification, so there is no key custody — and, in the sense the word
   implies, **no leg 2 at all.**
2. **`verify_snapshot` CANNOT FAIL.** It computes `current_hash` and never
   compares it. Demonstrated: on a rewritten chain, `verify_chain()` returns
   `False, 'link_hash mismatch at link 0'` while `verify_snapshot(1)`
   returns `True, 'snapshot valid'`. **The same object — the snapshot
   verifier blesses a chain the chain verifier just called tampered.** And
   `test_signing.py:195` asserts that constant, so the suite is green on a
   verifier that cannot fail.
3. `export_state` omits `private_key` and there is **no `from_dict`**, so an
   exported signature is **permanently unverifiable**.

> **THE CENSUS'S VERDICT CAME FROM THE WORD MATCH, NOT THE SENTENCE.**
> `signing.py`'s only two "tamper-evident" hits are inside a docstring that
> says *"NOT tamper-evident, and the missing leg is leg 3 … Nothing in this
> module does that."* It belongs in **ALREADY-CORRECT**.
>
> **ALL 24 FIX ROWS SHARE ONE BOILERPLATE REASON.** The verdict came from
> the word, not from what the sentence said. **That is the house's own
> lesson — a claim is not a string — applied to the census that was written
> to enforce it.**

---

## 3. THE REAL NEAREST MISS: `SuperInstance/receiptd` → `tipnotary.py`

**Not a plan. IT HAS RUN, AGAINST LIVE CLOUDFLARE KV.**

* `test-tipnotary/receipts.jsonl` is committed and holds **real
  `anchored_h` values**, a real `prev_anchor_h` progression, and a real
  namespace `a47cb5ab…`.
* **Its T5 pin asserts a self-consistent forgery must PASS `receiptd verify`
  AND FAIL `tipnotary check`.** That is the fleet's own test, pinning the
  exact property leg 3 is about.

**Why it still misses leg 3 — and the sentence is the finding:**

> **one `kv()` helper, one `CF_API_TOKEN`, BOTH VERBS. The writer who PUTs
> the anchor is the writer who can rewrite it.**

`notary` is a self-asserted string. `prev_anchor_h` chains anchors **inside
the thing being anchored**. Coherently poisoning the chain *and* KV returns
MATCH. **And whose Cloudflare account it is: caller-supplied, unevidenced
anywhere in 4,400 repos.**

**That is the self-anchor failure mode wearing the costume of an external
anchor.**

---

## 4. WHAT IT WOULD TAKE — AND IT IS SMALLER THAN THE CENSUS SAID

**The build, in order of cost:**

1. **SPLIT THE CREDENTIAL.** The publisher needs **PUT**; the verifier needs
   only **public GET**. **NO NEW ACCOUNT REQUIRED.** **This alone is the
   gap.**
2. **A key the writer does not hold.** The fleet **already wrote this and
   did not build it**: `fleet-witness/docs/L3-QUORUM.md` — *"L3
   designed-not-built … the Ed25519 keypair lives on the witness host,
   never in this repo."*
3. **A key the writer does not hold is NECESSARY AND SUFFICIENT.** A second
   account helps only if it has a **genuinely different owner** — a second
   *repo* closes nothing, and a second *account* controlled by the same
   person closes nothing either.
4. **The cheapest append-only third party is a TRANSPARENCY LOG** — and the
   fleet has already picked it twice **on paper**: `quilt-stone/STONE-SPEC.md`
   §9 (SCITT, RFC 9943) and `doubt-ledger`'s capsule adapter.

**COLLATERAL, AND IT IS THE SAME CLASS:** `guardc`'s "tamper-evident seal"
hashes `trace_digest: sha256_hex(b"placeholder")` — the literal bytes
`placeholder` — for every obligation, while asserting
`ProofStatus::Verified`. **It is a seal that is not sealing yet.**

---

## 5. THE SENTENCE

> **VIABILITY IS BINARY; DIFFERENCE IS GRADED.**
>
> Nobody holds leg 3 — that finding is confirmed and unchanged. But someone
> **built, ran, and pinned** the forgery test that proves publication is the
> *only* thing missing — and then put the anchor **in the log's own
> credential domain**.
>
> **That is a smaller, more fixable gap than the census states.** Not a
> research project: a credential split.

---

## 6. WHAT THE CENSUS GOT WRONG ABOUT ITSELF

**THE VERDICT CAME FROM THE WORD MATCH, NOT THE SENTENCE.** All 24 FIX rows
carried one boilerplate reason. `flux-signatures` — the repo the census
nominated as the nearest miss — was in fact **already correct**, and its
nearest-miss status rested on a docstring that says the opposite of what
the census read into it.

**AND THE METHOD THAT FOUND IT WAS NOT A METHOD ANYBODY HAD USED BEFORE:**
read all 932 lines, and check the *behaviour* of the verifier rather than
the *name* of the field. `public_key = _sha256(private_key)` is the kind of
thing only a full read finds.

**A CENSUS IS A HYPOTHESIS UNTIL SOMEBODY READS THE FILES.**
