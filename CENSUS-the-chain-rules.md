# THE CHAIN-RULE CENSUS — 5,314 repos, and a fourth property the vocabulary is missing

**Date:** 2026-10-19
**Scope:** every repository in the `SuperInstance` org, enumerated from the API
(**5,314 repos**), its tree fetched, and every file that *defines* a chain rule
read and fingerprinted.

**The unit of the census is the CHAIN RULE, not the chain file.** There are
far more chains than rules, and **a new rule is a new dialect** — so the rule
is what you enumerate.

---

## 1. How it was measured, and the trap in the method

**GitHub's code search is UNRELIABLE for this account and cannot be the basis of
a census.** `row_hash org:SuperInstance` returns **1 hit**, when six repos use
that field heavily. The index covers a SUBSET of the org. **A code search that
returns nothing is not evidence of absence here** — which is exactly the shape
of the mistake this fleet keeps making.

So the census enumerates instead:

| step | count |
|---|---|
| repos enumerated (`GET /user/repos?affiliation=owner`) | **5,314** |
| repos whose NAME suggests a chain/ledger/witness/log/anchor | **587** |
| trees fetched (587 × `git/trees/HEAD?recursive=1`) | **587, 0 errors, 42 s** |
| repos with a rule-shaped SOURCE file | **112** (604 files) |
| repos with `.jsonl` chain files | **68** (915 files) |
| files with an actual rule fingerprint (hash + canonical + linkage) | **27**, across **10 repos** |

**THE NAME HEURISTIC OVER-PROPOSES, AND THE READ DISPOSES.** 604 files matched
a rule-shaped name, and many are not chains at all: `quilt-playtest` ships
`no-non-null-asserted-optional-chain.js`, which is a **TypeScript lint rule
about optional chaining**. Only the file's contents decide.

---

## 2. THE FINDING: rules the fleet's canonical verifier does not know about

`SuperInstance/quilt-stone` knows **eight** dialects. These ten repos define
rules that are **not among them**:

| repo | file | hash | canonical | linkage |
|---|---|---|---|---|
| `quilt-chrono` | `src/seal.js` (32 KB) | **SHA-256 + Ed25519** | `canonicalJson` | `hash` + a signed `checkpoint` |
| `quilt-chrono` | `src/ledger.js` | SHA-256 | `JSON.stringify` | `prev` |
| `jev-quilt` | `jev_quilt/signed_receipts.py` | **BLAKE3 + Ed25519** | `json.dumps(sort_keys=True)` | signed receipts |
| `quilt-gpu-lab` | `experiments/d21_perception_ledger.py` | SHA-256 | `json.dumps(sort_keys=True)` | `prev` |
| `quilt-rust` | `packages/core/src/ledger.rs` (**50 KB**) + a **Go** and a **Python** binding | SHA-256 | Rust-side | `prev` |
| `moth-ledger` | `src/moth_ledger/{hashes,ledger,merkle}.py` | fnv1a64 + SHA-256 + **Merkle** | python | `prev` |
| `quilt-arcade` | `shared/receipts.mjs` | fnv1a64 | `JSON.stringify` | — |
| `quilt-crabbox` | `internal/cli/receipt.go` | SHA-256 | — | — |
| `quilt-research-canons` | `projects/chain-lint/chain_lint.py` | — | — | **a chain LINTER** |
| `pong-quilt` | `tests/coev-birth-seal.test.js` | — | `canonicalJSON` | lineage chain |

**`quilt-rust` is the sharp one: a 50 KB Rust ledger with a Go and a Python
binding of the same rule.** That is a three-port byte-equivalence obligation
that nobody has stated, and the fleet's own rule — *cross-port byte-equivalence
is the leaf invariant* — says it should be pinned.

**AND A CHAIN LINTER EXISTS** (`quilt-research-canons/projects/chain-lint/chain_lint.py`).
**A linter is a shared notion of what a chain IS.** It is the fleet's closest
thing to a standard, and it is not the one the verifier uses.

---

## 3. THE NEW FINDING, AND IT IS ABOUT THE KEY, NOT THE LOG

`SuperInstance/quilt-chrono` and `SuperInstance/quilt-jev-toolkit` have
**mutual Ed25519 verification across two repos**, and it is the best-engineered
witness machinery in the fleet. `quilt-chrono/examples/ed25519-cross-repo-proof.mjs`:

* one runtime-minted Ed25519 identity, **two repos, each verifying the other's
  artifact under its OWN law**;
* **fail-closed probes that refuse BY NAME** — a forged signature, a wrong key
  and a tampered link each produce `CHECKPOINT_SIGNATURE_INVALID` or
  `RECEIPT_HASH_MISMATCH`, under **both** laws;
* `verifyCustody` **replays the full chain** and returns the state;
* **the receipt carries no private key.**

Measured against the three legs:

| leg | holds? | evidence |
|---|---|---|
| 1 · re-derives | **YES** | `verifyCustody` replays the chain and returns the state |
| 2 · signed | **YES** | Ed25519, and the probes refuse by name |
| 3 · published out of band | **NO** | the receipts live in `examples/receipts/` and `receipts/` — **two repos, both in the same account** |

**So this is MUTUAL VERIFICATION, which is a real property, and it is ONE STEP
SHORT of leg 3.** Two repos each holding a verdict about the other is much
stronger than one repo holding a verdict about itself. But the repos have the
same owner, and there is **no anchor path in either repo**.

### AND HERE IS THE PART THAT IS NEW

**The identity is RUNTIME-ONLY.** The receipt says so itself:

> *"RUN-TIME-ONLY identity … the private key never left this process and is NOT
> in this receipt. Re-runs mint a fresh identity — verdicts are structural;
> the fingerprint of record changes honestly per run."*

**So the committed receipt cannot be re-verified by anyone, ever.** The public
key is in the file; the private key is gone; the signature is checked against
a key nobody holds. The receipt is a **RECORD of a verdict, not a VERIFIABLE
ARTEFACT**.

> **THE THREE-LEG VOCABULARY HAS A HOLE: A SIGNATURE BY A KEY YOU NO LONGER
> HOLD IS NOT A SIGNATURE YOU CAN CHECK.**
>
> There is a **fourth property**, and it is a property of the KEY, not of the
> log: **can the signature still be checked tomorrow?**
>
> `quilt-chrono`'s answer is **no** — and that is a *deliberate and correct*
> choice about key hygiene. Which is the sharp part:
>
> **THE BEST KEY HYGIENE AND THE BEST RECEIPTS ARE IN TENSION.** A
> runtime-only key cannot be stolen — and a seal made with it can never be
> re-checked. The strongest protection against key compromise produces the
> weakest receipts.

**This is not a defect in `quilt-chrono`.** Its key hygiene is right and its
probes are exemplary. It is a **gap in the vocabulary**, and the gap is only
visible because two repos verify each other under different laws — which is
the first time in this fleet that a *key property* has been observed rather
than assumed.

### AND IT CORRECTS THE EARLIER NEAREST-MISS

The vocabulary sweep named **`flux-signatures` as the fleet's closest thing to
a signer** — a real signing engine with key rotation, revocation and weighted
approval. That is still true, and `quilt-chrono` + `quilt-jev-toolkit` is
**closer**: it has signed seals, a **second independent law**, **cross-repo
receipts**, and **fail-closed probes**. The difference is that its receipts
are not re-verifiable, for a reason that is a virtue.

---

## 4. WHAT THIS ADDS TO THE THREE LEGS

| | question | quilt-chrono |
|---|---|---|
| leg 1 | does it re-derive? | yes |
| leg 2 | is it signed by a key we trust? | yes, and the probes refuse by name |
| **leg 2b** | **can the signature still be checked tomorrow?** | **NO — the key is runtime-only** |
| leg 3 | is the head published where the writer does not control it? | no — two repos, one owner |

**THE HONEST WORD for `quilt-chrono` today: `attested`, not `tamper-evident`,
and not merely because leg 3 is missing — but because leg 2's durability is
missing too.**

**THE BUILD THAT WOULD CLOSE IT, AND IT IS NOT A KEY:** keep the runtime-only
key, and **publish the CHECKPOINT (the signed document) to a place the log
writer does not control, along with the public key.** The signature is already
there and already fail-closed. What is missing is not cryptography; it is
**a place to put the receipt where the writer cannot rewrite it** — and the
fingerprint of record, being different per run, is exactly what makes that
publishing meaningful rather than a formality.

---

## 5. THE RECEIPT

* `CENSUS-the-chain-rules.md` — this document.
* `CHAIN-RULE-CENSUS.json` — the machine-readable form: the repos, the trees
  fetched, the rule-shaped files, and the fingerprints.
