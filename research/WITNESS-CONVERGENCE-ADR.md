# ADR-0007 — One witness rule, one verifier, one anchor

**Status:** Proposed
**Date:** 2026-10-18
**Supersedes:** nothing. **Amends:** `witness-spec/SPEC.md` (marked *draft*, and stale — see §0.4).
**Scope:** the fleet's witness-log idiom — `recorder.py`, `MicroMoth-quilt/tools/collapse_ledger.py`, `quilt-claw/witness/ledger.js`, the 11 bot `witness.log` files, and the `quilt-stone` conformance runner.
**Read-only ADR.** Nothing in this document has been pushed. Every number below was measured today against `main`; the harness is in §8 and the artefacts are at `/tmp/witness-dialect-prototype.mjs` and `/tmp/witness-encoding-probes.json`.

---

## 0. The receipts

I did not take the survey's numbers on trust. Here is what I measured, and two of the survey's claims are wrong in ways that change the plan.

| # | Measurement | Result |
|---|---|---|
| R1 | `recorder.py::_jcs` vs **RFC 8785 Appendix B** (24 numbers + §3.2.2 string + UTF-16 key sort) | **19/25 — FAILS 6** Appendix B number samples |
| R2 | `witness-spec/ref/jcs.py` on the same suite | **25/25** |
| R3 | `quilt-stone/stone.mjs::canonicalJSON` on the same suite (JS) | **26/26 — already RFC 8785 conformant** |
| R4 | `collapse_ledger.py` v0 rule vs `recorder.py` v0 rule | identical `GENESIS`; **5000/5000** random inputs byte-identical |
| R5 | `quilt-claw/witness/ledger.js::canon` vs `recorder.py::_canon` (v0 rule, 20 encoding probes) | **8 of 20 DIVERGE** |
| R6 | `quilt-stone` `witness-v0` + `witness-v1` dialects, prototyped | v0: **7/7** committed chains verify. v1: **byte-identical both directions** with `recorder.py` |
| R7 | `quilt-stone/verify_all.mjs` on the recorder transcripts | **refused** — `"no link fields (not a chain)"` |
| R8 | Bot `witness.log` files carrying a `prev`/`prev_hash` field | **0 of 13** |
| R9 | `bots/rsi-gan/witness.log` under deletion of 50 rows | 262 rows, 262/262 self-verify → **112/112 intact** after deletion |
| R10 | A **proper** `recorder.py` v1 chain, tail-truncated by 50 rows | **still verifies OK.** Head changes, nothing notices |
| R11 | `recorder.py` on a chain mixing v0 and v1 | **refused**, with the right message |
| R12 | `situations/dispatch-ledger.csv` | 167 rows, 13 columns, **zero** hash/prev/sig fields |
| R13 | `gitpp-oracle/ledger.jsonl` | **1 record**, CI results, no hash, no prev |
| R14 | Suite health today | `witness-spec` 15/15 · `quilt-stone` smoke **92/92** · `verify_all` CONFORMANCE GREEN |
| R15 | Ed25519 | Node: `node:crypto`, **zero deps**, 64-byte sigs — and `stone.mjs` **already ships `signTip`/`verifyTipSignature`**. Python: needs `cryptography`; **not in stdlib** |

### 0.1 The root cause is a *duplicate*, and the duplicate is broken

`witness-chain/1` needs a canonicaliser. There are **three** of them:

- `witness-spec/ref/jcs.py` — **25/25** (R2)
- `quilt-stone/stone.mjs::canonicalJSON` — **26/26** (R3)
- `recorder.py::_jcs` — a private re-implementation, **19/25** (R1)

The reference implementation — the thing every other port is synchronised against — is the **only one that fails**, and it fails on exactly the six Appendix B samples `witness-spec/test/completeness.test.mjs` names as *"the samples that exposed the Python bug"*. It fails because `_jcs_number()` pads exponents to two digits (`9.999999999999997e-07` vs the RFC's `e-7`) and never implements the ECMAScript fixed-notation rule for exponents in `[-6, 21)`, and because `isinstance(v, int) -> str(v)` lets arbitrary-precision Python ints escape the double model entirely (`2**68` -> `295147905179352825856`, RFC: `295147905179352830000`).

Two CIs are green today. `witness-spec` proves *its* Python is conformant; `.github/workflows/corpus.yml` proves *recorder's* Python self-verifies. **Nothing makes the two agree.** They do not agree. This is the fleet's own disease — *"a green badge over a truncated test set"* — reproduced one level up, across two repos instead of inside one.

### 0.2 `quilt-stone` is not the blocker the survey thought it was

The brief says `quilt-stone` has zero occurrences of `recorder`, `witness-chain`, `RFC 6962`. True. It also has **zero** `quilt-claw`, `AI-Writings`, `MicroMoth`. And **RFC 8785 in three files**. `quilt-stone` is not a monolith that must be taught a foreign format — it is **already a dialect host**: six `ALGS` entries, `detectAlg`, `verifyChainFile`, and a `verify_all.mjs` that walks `../` sniffing every `.jsonl` and cross-checks each verdict against the repo's own verifier. Its canonical JSON is already RFC 8785 (R3). Its Ed25519 tip-signing lane already exists with a trusted-key registry and an explicit *binding != identity* rule (R15).

**`witness-chain/1` as a stone dialect costs 13 lines.** I wrote them (R6) and they agree with `recorder.py` byte-for-byte: stone verifies `recorder.py`'s v1 output to head `0x5d9de7c5a8e13526…`, and `recorder.py` verifies a stone-sealed v1 chain. The whole convergence is *not* a large engineering problem. It is a **governance** problem wearing an engineering costume.

### 0.3 Why they have never met (and it is not "no one read the spec")

`quilt-stone/verify_all.mjs::sniffChainFile` admits a file as a chain only if some row carries `row_hash`, or `hash && prev`, or `row_hash && prev_hash`. **A `witness-chain` row carries `hash` and no `prev`** — the linkage is implied by *record order* and recovered only by replay. So the canonical verifier classifies the reference implementation's output, and its own flagship ports' output, as **not a chain** (R7). It is a false negative, and a false negative in a conformance runner is worse than no runner, because it reports *absence*.

**The two formats disagree about where the linkage lives.** That is the actual seam. Everything else is downstream of it.

### 0.4 Corrections to the survey

- **"`witness-spec` is a spec nobody reads."** It is read by its own CI on every push and it is the most rigorous artefact in the fleet — 24 samples asserted by count, by the RFC's own hex, and by a digest, with a test whose stated purpose is to prevent exactly the truncation that let a bug through once. It is read by nobody else.
- **"There is no `corpus.yml`."** There is `AI-Writings/.github/workflows/corpus.yml`, and it is correctly wired: it runs `recorder.py`, then `run_all.sh`, then `corpus.py`. It is `paths:`-filtered to `situations/transcripts/**`, `situations/dispatch-ledger.csv`, and `labs/situation-recorder/**` — **so it does not run when `witness-spec` changes.** The gate is real and it is blind in exactly the direction that matters.
- **"All 174 chains verify."** I could not reproduce 174 from anything in the fleet. `situations/transcripts` contains **7 chains**, all `witness-chain/0`, all verifying. `quilt-stone`'s last published fleet census is **42/42 across 11 repos**. I am using 7 and 42. An ADR that quotes a number nobody can re-derive is the disease this fleet exists to kill.
- **"7 of 13 probes diverge."** I get **8 of 20** on a wider set — but the shape matters more than the count (§0.5).

### 0.5 What actually diverges in `quilt-claw` — and what does not

`ledger.js` has a hand-rolled `pyEscape` that is **correct**: non-ASCII, astral planes, surrogate pairs, DEL, control characters all agree with Python's `ensure_ascii=True`. Whoever wrote it did the hard part carefully. The divergence is elsewhere, and it is three things:

| class | py | js | probes |
|---|---|---|---|
| integral floats | `1.0` | `1` | 3 |
| negative zero | `-0.0` | `0` | 1 |
| ints beyond 2^53 | `9007199254740993` | `9007199254740992` | 1 |
| exponent zero-padding | `1e-07` | `1e-7` | 1 |
| **key order at U+E000 / U+10000** | code point | UTF-16 unit | 2 (top-level **and nested**) |

The string layer is right. **The number layer is wrong, and the key comparator is wrong at exactly the boundary the spec named** — silently, inside nested objects, where nobody is looking. This is why the bug is latent: the 8 real records in `quilt-claw/witness/ledger.jsonl` carry no floats, no big ints, and no astral keys. They agree 8/8. **The port is broken on the cases its data does not contain**, which is the definition of latent.

---

## 1. THE DECISION

> **`witness-chain/1` — as `recorder.py` defines it and `witness-spec/SPEC.md` specifies it — is the fleet's one chain rule. `witness-spec` owns the normative bytes. `quilt-stone` is the one verifier, and gains two dialects. `recorder.py` stops being a canonicaliser and becomes a writer that delegates. The signed tree head becomes Ed25519, with `sigAlg` inside the signed bytes and a verifier that refuses rather than degrades.**

Five commitments, in order of how much they cost you to reverse:

**D1 — `witness-spec` is the normative source, not a document about one.**
It owns the rule, the vectors, and the two reference ports (`ref/jcs.py`, `ref/jcs.mjs`). Any port's conformance is *defined* as passing `conformance/rfc8785-vectors.json` (all 24, by count/hex/digest) and `conformance/chain-vectors.json` (the new chain-level vectors).

**D2 — `recorder.py` deletes its private `_jcs` and imports the spec's.**
Not "patch the six number cases." Six cases is a patch; the seventh arrives with the next JSON-shaped edge and nobody will be looking. The fix is to **delete the duplicate**, which is the actual defect. This is a one-line-shaped change with a permanent effect: it is the only way the reference implementation can never again be the least-conformant copy.

**D3 — `quilt-stone` gains `witness-v0` and `witness-v1` as dialects, and gains the second detection mode.**
`witness-v1` is 13 lines (R6). `witness-v0` is 11 lines plus a 29-line Python-faithful canonicalisation shim, and it is **transitional, frozen, and sunset-dated** (§6.4). `verify_all.mjs` learns to admit order-implied linkage.

**D4 — The head is Ed25519, not HMAC.** Full design in §3.

**D5 — A verdict must name its linkage class, and `witness-chain/1` without a published head reports `attestable: false`, never `tamper-evident`.**

### Why `witness-chain/1` and not `stone-v1`

`stone-v1` is already deployed (`pong-quilt` PR #46, merged 2026-09-27) and is also SHA-256 over canonical JSON. It looks like a competitor. It loses on three measured counts:

| | `stone-v1` | `witness-chain/1` |
|---|---|---|
| canonicalisation | house recursive sort | **RFC 8785** (R3 confirms the house *already has a conformant one*) |
| domain separation | none — `[prev, row]` in one hash | **`0x00`/`0x01`, RFC 6962 §2.1** |
| rule carried in the record | `stone.header` row | `v` field, **inside the hashed bytes** |
| mixed-chain refusal | n/a | **already implemented and tested** (R11) |
| genesis | `STONE-GENESIS-1` | `0x0000000000000000`, **shared with v0** |

The genesis matters more than it looks: `witness-v1` inherits v0's `GENESIS`, so a v0 chain and a v1 chain are distinguishable *only* by the `v` field inside the hash. That is why R11 works. `stone-v1` would have forced a second genesis convention on an ecosystem that already has one.

**And `stone-v1` is not thrown away.** It stays as a dialect. `quilt-stone` ends up with seven dialects and one verifier, which is exactly what it was built to be.

---

## 2. The dialect question

**Decision: `quilt-stone` learns to read the chain format directly, as two new dialects. `recorder.py` does not become "a dialect inside `quilt-stone`" — it becomes a *client* of the same spec, and the two are joined by vectors, not by code.**

The survey's framing offers two options that are not actually in tension, so it is worth naming why the third option is the right one.

**Rejected: "make `recorder.py` a dialect inside `quilt-stone`."** A dialect is a *chain format* hosted by a verifier. `recorder.py` is a *writer* for one situation type, coupled to `Situation`, `REL_TYPES`, the FOLD/argmin fold, and `corpus.py`'s table builder. Stuffing it into `stone.mjs` would drag a situation-transcript writer into a chain library, and would put the fleet's witness machine behind a dependency it did not ask for — while leaving the *other* three ports synchronised against `recorder.py` by hand, which is the problem. It also inverts the actual dependency: the writer should not need the fleet-wide verifier to write a receipt.

**Rejected: "`quilt-stone` learns the format" as a special case bolted onto the sniffer.** This is what §0.3 warns about — it is done as a one-off, and the next order-implied chain gets another one-off.

**Chosen: the rule lives in `witness-spec`; both sides implement it; vectors arbitrate.**

- `recorder.py` — writer. Imports the spec's canonicaliser (D2). Keeps its own `verify()` because a writer that cannot check its own output is a writer that ships untested ledgers. That check stays honest and local.
- `stone.mjs` — verifier. Gains `witness-v0`/`witness-v1` dialects so *one* command verifies *every* repo's chain, including chains whose own repo no longer runs.
- **Arbitration is by vector, never by copying.** Neither side imports the other. Both pass the same `conformance/`. This is what makes it *convergence* rather than *adoption*.

This inverts the survey's stated order of operations in one place, deliberately. The survey says: *fix the reference -> choose one spec -> add `recorder-v0` as a dialect -> then the ports.* Correct sequence, wrong first verb. **"Fix the reference" is not a patch; it is a deletion.** Once `_jcs` is gone from `recorder.py`, steps 2 and 3 are both cheap (R6 proves 13 lines), and the ports stop being four hand-synchronisations and become two ports of a published rule.

---

## 3. The anchor: Ed25519, and what it costs

**Decision: yes — Ed25519. But the interesting cost is not the signature, it is the key custody and the refusal semantics.**

### 3.1 The cost is asymmetric by language, and the asymmetry is the whole problem

| | Ed25519 available | Cost |
|---|---|---|
| Node | `node:crypto` — **zero deps**, and `stone.mjs` already has `signTip` / `verifyTipSignature` with a `trustedKeys` registry and the binding!=identity rule | **nothing.** Copy the lane. |
| Python | `cryptography` — **not stdlib** | **`recorder.py` loses "zero dependencies, Python stdlib only"**, which is a stated house value and appears in its own module docstring |

That loss is real and I am not going to pretend otherwise. Three ways out, one recommended:

- **(a) RECOMMENDED — optional dependency, mandatory refusal.** `sign_head`/`verify_head` require `cryptography`; if absent, `verify_head` **raises `AnchorUnavailable`**. It does not fall back. The chain still verifies; the head does not. This is `doubt-ledger/ledger/sign.py`'s exact discipline: *"the pins skip — never silently pass."*
- **(b) Vendor RFC 8032 pure-Python Ed25519 (~90 lines).** Keeps stdlib-only. Cost: a hand-rolled curve implementation in a repo whose entire thesis is *"don't hand-roll the primitive, cite the RFC"* — and it would be the one place the fleet does. It would also need its own test vectors to be credible.
- **(c) Accept `cryptography` as a hard dependency of the anchor subcommand only.** Chain writing/verifying stays stdlib-only; `recorder.py --sign-head` gains a `try/except ImportError` that says so out loud.

**(c) is what I would actually ship**, because it puts the dependency exactly where the asymmetric primitive lives and leaves the hot path — `emit()` and `verify()` — untouched. But **(a) is the property that matters and (c) inherits it**: *absence of a backend is a refusal, never a fallback.*

### 3.2 Do not copy `moth-ledger`'s `best_signer()`

`moth-ledger/src/moth_ledger/signatures.py` has the right *vocabulary* — *"A MAC chain where the verifier holds the key is a confession, not an alibi"* — and the right `Ed25519Signer`. But `best_signer()` **silently degrades to `KeyedMacSigner` when `cryptography` is missing.** For the witness head that is unacceptable: an environment that cannot verify Ed25519 would silently verify HMAC and report success. The degradation is invisible precisely when the environment is broken, which is when you need it loud.

**Rule:** `sigAlg` goes **inside the signed payload**. The head says which algorithm signs it, and a verifier that lacks that backend **refuses**. There is no code path that returns "verified" for an algorithm it did not use. This is what makes the refusal auditable rather than a promise.

### 3.3 The domain separation, precisely — and a defect in the spec

`recorder.py` signs `HMAC-SHA256(0x03 || head)`. `0x03` is a **house extension**; RFC 6962 defines only `0x00` (leaf) and `0x01` (node). It must be pinned in the spec before ports diverge on it. `stone.mjs` uses a *string* prefix `"stone-v2"` instead. These must be reconciled to one constant.

Worse, and found by measurement rather than reading: **`recorder.py` concatenates the leaf as a `0x`-prefixed ASCII hex string, not as a raw 32-byte digest.** RFC 6962 hashes raw bytes. This is why my first dialect attempt diverged at index 0 while the canonicalisations were byte-identical. It is not a bug — it is a decision nobody wrote down. `SPEC.md` currently shows `hash(i) = SHA256(0x01 || hash(i-1) || leaf(i))` with no statement about *which encoding of those operands*. **Any port written from the spec as written will get this wrong**, which is precisely why the new `conformance/chain-vectors.json` must exist and must be authoritative.

### 3.4 The anchor is only an anchor if the writer cannot reach the key and cannot reach the head

Both halves, or it is not an anchor:

1. **Key separation.** The signing key is held by CI of the *parent* repo (or a repo the log writer's workflow cannot push to). A signing key in the same repo as the log is not an anchor; it is a confession with extra steps.
2. **Publication.** The signed head is committed to `witness-heads/<sid>.head.json` **out-of-band**, on a `paths`-triggered workflow that runs even when the transcript does not change.

This kills R10. Tail-truncation of a perfectly chained v1 log is **undetectable by replay** (R10) because replay is self-consistent — the same defect as the re-chained rewrite that `recorder.py`'s own self-test demonstrates beautifully. A published head turns "the chain verifies" into "the chain verifies **and ends where it ended yesterday**". That is the entire tamper-evidence story, and it is worth saying plainly: **the fleet's most-repeated idiom is its least protected one, and no amount of hashing fixes it.**

---

## 4. What does NOT need migrating — and one thing that does, in the other direction

**Confirmed: `gitpp-sheet`, `gitpp-oracle`, and `dispatch-ledger.csv` are not chains, and converging them would be a category error.** But the *reason* matters, because the obvious reason is the wrong one.

The wrong reason: *"they have no hash field."* The right reason: **there is no linkage to replay.** A chain is a sequence in which each element's commitment covers its predecessor. Converging a non-chain onto a chain format does not make it evidence; it makes it a **chain-shaped object with an implied history nobody committed to** — which is strictly worse than an honest append-only log, because it borrows the credibility of the idiom without the property.

- `dispatch-ledger.csv` — 167 rows, 13 columns, zero hash/prev/sig (R12). A **dispatch register**: `dispatch_id, booked_at, tier, model, cost_class, task, serves_gap, handed_down_from, booked_up_to, verdict, standing, receipt, status`. `dispatch_id` is a **primary key, not a link**. Do not touch it.
- `gitpp-oracle/ledger.jsonl` — **one record**, a CI results blob (`ports`, `suites`, `newDivergences`, `acceptedDivergences`). It is a *run log*. The word "ledger" is the only thing chain-shaped about it.
- `gitpp-sheet` — a spreadsheet with cell-level confidence and a provenance graph (`state`, `setters`, `order`, `conflicts`). The provenance graph is **already** a trust structure; it needs no hash chain and would gain nothing from one.

**Also in this category** (found by running `verify_all.mjs`, R7): `MicroMoth-quilt/receipts/exp022-desert-break/exp022.telemetry.k{4,5,6,7}.jsonl` — *"no link fields (not a chain)"*. Telemetry, not receipts.

**And now the correction in the other direction — the 11 bot logs are ALSO category errors, and they are the worse ones.**

`bots/*/witness.log` is named `witness.log`, carries a field called `hash`, and is described as witnessing. **0 of 13 carry a `prev` field** (R8). The `hash` is `sha256(payload)[:16]` — a checksum of its own row. It detects bit-rot. It detects **nothing else**:

- `rsi-gan`: 262 records, **262/262 "intact"**; delete 50 and it is **112/112 intact** (R9). Zero `prev`, zero `seq`.
- Truncating the tail is undetectable. Reordering is undetectable. Removing the 50 worst runs is undetectable.

These logs are currently **claiming a property they do not have, under a filename that promises it.** That is worse than `gitpp-oracle`'s single CI record, which never claimed to be a chain.

### 4.1 How an unchained log is detected, and refused

**D5, in full.** The fleet's verdict vocabulary gains a mandatory field, and "intact" becomes a word only one class may use:

| linkage class | test | may say |
|---|---|---|
| `chain` | dense `seq` from 0, and replay reproduces every linkage | `N/N records, chain intact` |
| `row-checksum` | each row's hash matches its own payload | `N/N rows, row checksums match` — **never "intact"** |
| `none` | neither | **refused**: `not a witness log — no linkage` |

And the refusal is mechanical, in `verify_all.mjs` and `corpus.py` alike:

```
sniffChainFile()  ->  { linkage: 'chain' | 'row-checksum' | 'none' }
verifyChainFile() ->  ok:false, why:'no linkage — a receipt log, not a chain'
```

`rsi-gan` must come out as `linkage: 'row-checksum'`, 262/262 row checksums, **deletion undetectable — not a witness log**. If the bot team wants `linkage: 'chain'`, the migration is to write v1 records — not to keep calling a checksum a chain.

---

## 5. Migration order and exact file list

Ordered so that **every step is independently mergeable and leaves every suite green.** No step requires a flag day.

### M0 — Freeze the numbers (`witness-spec`) · *pure docs + one new file*

The spec is the normative source, so it must be true before anything cites it.

- `SPEC.md` — status `draft` -> `normative`; **delete "the current chain … is what ships today"** (§0.4); pin `0x03` as a house extension; **state the leaf/node operand encoding as ASCII-hex-with-`0x`, not raw bytes** (§3.3); state the v0 int/float portability limit (§6.4).
- `conformance/chain-vectors.json` — **NEW.** Chain-level vectors: leaf, node, head, per-version. Authoritative for operand encoding.
- `conformance/rfc8785-vectors.json` — unchanged. It is already correct and it is the best artefact in the fleet.

### M1 — Make the reference delegate (`AI-Writings`) · *the load-bearing step*

- `labs/situation-recorder/recorder.py` — **delete `_jcs`, `_jcs_key`, `_jcs_string`, `_jcs_number` (~65 lines)**; import the spec's canonicaliser. **Fix the module docstring**, which still reads *"fnv1a-64 chained, tamper-evident, replayable"* — a live falsehood about a rule the file no longer ships.
- `labs/situation-recorder/test_jcs_conformance.py` — **NEW.** Asserts all 24 Appendix B samples, by count/hex/digest. **This test fails today** (R1) and passes after the deletion.
- `labs/situation-recorder/corpus.py` — emit the verified `head` alongside the tables (§6.5).
- `.github/workflows/corpus.yml` — add the new test **and** a `workflow_run`-triggered job so a `witness-spec` change re-runs this gate (§0.4).
- `labs/situation-recorder/run_all.sh` — unchanged; already correct.

### M2 — Give the verifier the dialects (`quilt-stone`)

- `stone.mjs` — add `ALGS['witness-v0']`, `ALGS['witness-v1']`; export `fnv1a64utf8` (UTF-8 bytes, unmasked — **not** `fnv1a64RawString`, which masks to 8 bits and is the `quilt-raw` dialect).
- `verify_all.mjs` — **second detection mode**: admit order-implied linkage (`hash` + dense `seq`, no `prev`). Count `linkage:'none'` as a *reported category*, not a silent skip (§4.1).
- `smoke.mjs` — add the 24-vector gate **and** a self-seal/self-verify round-trip per dialect.
- `STONE-SPEC.md` — document the two dialects, the detection modes, and the sunset.

### M3 — Flip the ports

- `quilt-claw/witness/ledger.js` — delete `pyEscape`/`canon`; delegate to the dialect. **Rewrite the header**, which still describes the v0 rule. Add the 20-probe gate as a test.
- `MicroMoth-quilt/tools/collapse_ledger.py` — read-only support for v1 alongside v0. **Zero-risk**: byte-identical at v0 (R4), and nothing in it writes v1 yet.
- `quilt-stone/README.md` — dialect table: 6 -> 7 + `witness-v0` transitional.

### M4 — The anchor goes Ed25519

- `witness-spec/ref/chain.py`, `ref/chain.mjs` — `sign_head`/`verify_head` with `sigAlg` pinned inside the signed payload; **refuse on missing backend**; keep HMAC **read-only** for historical heads, never as a fallback.
- `quilt-stone/stone.mjs` — reconcile `"stone-v2"` vs `0x03` to one constant; port the `trustedKeys` binding/identity rule.
- `AI-Writings/witness-heads/<sid>.head.json` — **NEW**, one file per transcript, written by a workflow the log writer cannot push to.
- `AI-Writings/.github/workflows/witness-heads.yml` — **NEW**, `paths:`-triggered on the transcript.

### M5 — Relabel the bot logs

- `bots/<11>/witness.log` — each gets `linkage` classified by M2's sniffer. `rsi-gan` lands on `row-checksum`. Either migrate to v1 records or accept the weaker label; **do not leave them claiming `witness`**.

**M0 -> M1 is the only step that can break something, and it is the step that must happen first.** M2–M5 are additive.

---

## 6. Consequences, including the bad ones

### 6.1 What breaks

Every hash in every v0 chain becomes **legacy the moment M1 lands** — not because the bytes change, but because a v1 reader will refuse a v0 chain and we will have stopped calling v0 a supported rule. That is intended. It is also 7 committed chains plus every chain in the fleet's history, and none of it may be rewritten. **`witness-chain/0` becomes read-only and frozen on the day `witness-chain/1` is declared canonical.** The docstrings that say "byte-compatible with MicroMoth-quilt … tamper-evident" become false and must die with it.

### 6.2 What gets harder

- **The conformance gate now has to run across repo boundaries.** Today two green CIs sit next to each other and disagree (R1 vs R2). Wiring M1's gate to `witness-spec` is a `workflow_run` dependency — more moving parts than a self-contained suite, and it is the only way the reference can stop being the weakest copy.
- **Key custody becomes an operational burden that did not exist.** Somebody must generate keys, hold them, rotate them, run CI with them, and publish heads out-of-band. Two new failure modes: **lost key -> a permanently unverifiable head**; **leaked key -> a forgeable one**, which is strictly worse than no anchor because it is believed.
- **Python loses stdlib-only at the anchor.** §3.1(c) keeps it off the hot path, but the docstring claim has to change.
- **The 29-line `pyCanon` shim is a liability we choose on purpose.** Every line of Python-faithful emulation is a place for the next divergence to hide, and it exists only to keep dead chains readable.

### 6.3 What a contributor must now do that they did not before

1. **Set `v` on every record.** The chain no longer infers its own rule.
2. **Pass 24 vectors + chain vectors.** One command. This is the new floor.
3. **Register the dialect.** A new chain format is an `ALGS` entry + vectors + a `detectAlg` branch. Not a README paragraph.
4. **Never write "tamper-evident"** without a published head. D5 makes it a lie the tooling will contradict.

### 6.4 The v0 dialect's known limitation — stated, not hidden

`witness-chain/0`'s encoding is **not a function of the JSON value. It is a function of the (JSON, host-language) pair.** Python's `json.dumps` distinguishes `int` from `float` — `1` and `1.0` hash differently — and JS has only doubles, so **a v0 chain containing `1.0` cannot be re-derived in JavaScript at all.** No amount of care fixes it; the information is gone at `JSON.parse`.

This is not a bug in `pyCanon`. It is the reason `witness-chain/0` **must** die, stated in one sentence a reviewer can check. The v0 dialect is therefore **transitional, frozen, and given a sunset date in `STONE-SPEC.md`.** It exists to keep 7 chains and a repo's history readable — not because it is portable. It is not.

### 6.5 The ratchet — how this design keeps a judge from being steered

The measured hazard: a poisoned ledger entry moved an honest **0.990 to 0.603 +/- 0.060**. So:

- **`verify()` / `verifyChain()` are pure predicates.** They return `(ok, why)`. They never write a record, never repair one, never substitute a computed hash for a stored one, never blend. I checked: `stone.mjs` advances `prev = r[hashField]` only *after* `want === r[hashField]`, and returns a verdict object rather than a repaired chain. That property is correct today and **must be pinned by a test**, because it is the property that keeps a ledger from steering a judge.
- **A re-derivation COMPARE-and-FLAG. Never blend.** `corpus.py` emits the verified `head` beside the tables (M1). A later run that produces a **different head for the same `sid`** prints `HEAD DISAGREEMENT: sid was <h1>, now <h2>` and exits non-zero. It does not merge, average, prefer, or "update".
- **The tables are derived, never authoritative.** `decompositions.jsonl` verdicts are re-derivable from the transcript at any time. A consumer that wants a number must re-derive it; one that reads a cached table is reading an unauthenticated cache, and the head is what makes that checkable.

### 6.6 The honest cost, stated plainly

**None of this makes the fleet's logs tamper-evident.** v1 makes them *attestable*: a log becomes evidence the moment its head is published somewhere the writer cannot reach. Until M4 ships and heads are actually published, the fleet has **better hashes and no tamper-evidence** — and every docstring, README and PR description claiming otherwise is wrong. The most this ADR delivers before M4 is: a rewritten log is *more* expensive to produce and *easier* to spot. That is not tamper-evidence. Say so.

---

## 7. What would make me change my mind

I am asking for the measurement that would falsify **D3** — the "one verifier, dialects" commitment — because that is the load-bearing one.

> **The falsifier: if `witness-chain/1` cannot be made byte-identical across Python and Node on the corpus, D3 is wrong and the fleet needs two verifiers, not one.**

Concretely, I would reverse D3 if any of these is measured:

1. **Cross-language byte disagreement on the corpus.** After M1 and M2, if `recorder.py`-sealed and `stone.mjs`-sealed v1 chains over the *same* bodies do not produce the same head for >= 99% of records, the rule is not portable and a single verifier is a fiction. **Today it is 3/3 identical, including a FOLD record carrying floats** (R6) — which is why I believe it, and why this is falsifiable rather than faith.
2. **Detection-mode collateral damage.** If admitting order-implied linkage causes `verify_all.mjs` to admit a materially larger number of **false** chains (receipt logs with a `hash` and a `seq`), the single-verifier premise leaks: one verifier that waves everything through is worse than two that are honest. *Guard:* the census must report `linkage:'row-checksum'` as its own counted class, so this number is visible before it becomes a problem.
3. **The 29-line v0 shim grows.** If emulating Python's `json.dumps` needs more than ~40 lines to hold `7/7`, v0 is not portable in practice, and the honest move is to **quarantine v0 in a Python-only archival tool** rather than ship a lossy JS dialect that looks portable.
4. **Falsifier for D4 (Ed25519):** if key custody cannot be operationally held — if no repo can hold a signing key that the log writer's workflow cannot reach — then an Ed25519 head is *cosmetic* (it proves nobody without the key wrote it, and the key sits next to the log anyway). At that point HMAC-with-honest-labelling is the more truthful artefact, because it does not borrow the word "signature". I would rather ship a labelled MAC than an Ed25519 key in the wrong place.
5. **Falsifier for D5:** if relabelling the 11 bot logs costs more than the two weeks it takes, the labels are wrong and the honest fix is to **delete the `witness.` prefix from the filenames** and stop implying a property the format cannot provide. That is also a legitimate outcome — and a cheaper one.

---

## 8. Reproducing every number in this ADR

Read-only. Nothing was pushed. All repos read at their API-reported `default_branch` (`main`; **`doubt-ledger` is `poc`**).

```
/tmp/witness-dialect-prototype.mjs    13-line witness-v1 + 11-line witness-v0 dialects,
                                     registered into quilt-stone's exported ALGS
/tmp/witness-encoding-probes.json     the 20 encoding probes behind R5
```

```bash
# R1/R2 — reference implementation vs the spec, on the RFC's own 24 samples
python3 probe_py.py       # 19/25, fails 4430000000000000 444b1ae4d6e2ef4e 444b1ae4d6e2ef4f
                           #            3eb0c6f7a0b5ed8c 3eb0c6f7a0b5ed8d becbf647612f3696
python3 …import spec.ref_jcs…       # 25/25
node probe_full.mjs       # quilt-stone canonicalJSON 26/26 ; witness-spec ref/jcs.mjs 26/26

# R4 — collapse_ledger is byte-identical to recorder at v0
#                       # 5000/5000 random inputs, identical GENESIS

# R5 — quilt-claw v0 divergence
python3 dp.py > py.jsonl && node dp.mjs > js.jsonl && python3 cmp.py   # 8 of 20 DIVERGE

# R6 — the two dialects
node runproto.mjs        # witness-v0: 7/7 committed chains
node xcheck.mjs          # stone witness-v1 verifies recorder.py v1  -> ok, head 0x5d9de7c5…
node seal.mjs && python3 # recorder.py verifies a stone-sealed v1 chain -> True

# R9/R10 — the two unlinkable/undetectable cases
python3 deltest.py       # 262/262 -> delete 50 -> 112/112 intact
python3 …                # v1 chain, tail-truncated 50 -> STILL VERIFIES

# R14 — suite health, unchanged by this ADR
cd witness-spec  && node test/all.mjs     # 15/15
cd quilt-stone   && node smoke.mjs        # 92/92
cd quilt-stone   && node verify_all.mjs   # CONFORMANCE GREEN (and it refuses the transcripts: R7)
```

---

## 9. In one paragraph

The fleet has one witness-log idiom and three canonicalisers, and the reference implementation is the broken one — 19/25 against the RFC's own Appendix B, on precisely the six samples its spec says catch this bug — while two green CIs sit next to each other and never compare notes. `quilt-stone` is not the obstacle the survey took it for: it is already a six-dialect host whose canonical JSON is already RFC 8785, and `witness-chain/1` costs 13 lines there and verifies `recorder.py`'s output byte-for-byte. The real seam is that stone looks for a `prev` field and a witness chain does not have one, so the canonical verifier has been reporting *absence* where the reference implementation stands. So: `witness-spec` owns the bytes, `recorder.py` deletes its duplicate and delegates, `quilt-stone` gains two dialects and a second detection mode, the head goes Ed25519 with the algorithm pinned inside the signed bytes and a verifier that refuses rather than degrades, and every verdict in the fleet must name its linkage class — because `rsi-gan` reports 112/112 intact after losing 50 rows, and that sentence must become unproducible. None of this makes a log tamper-evident. A chain is self-consistent, and self-consistency is not tamper-evidence; the anchor is the whole of it, and until M4 ships and heads are published out-of-band, the honest word is *attestable*, not *tamper-evident*.
