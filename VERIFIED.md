# Verified findings

Every claim here was checked directly, not taken from an agent's report.
Two independent agents (reader3, reader4) reported overlapping findings;
the ones marked VERIFIED below were re-derived from the spec bytes here.

---

## V1 — §2.5's published "ground truth" digest is not a SHA-256 and is 65 chars

The spec calls §2.5's byte sequence *"the ground truth — every port must
produce these bytes (and only these bytes) for this input."*

Step 2 publishes the bytes. Step 3 publishes a digest for them:

```
step-2 bytes : {"effects": {"x": 1, "y": "hello"}, "id": "task-1", "kind": "task", "parents": ["a", "b"], "reason": null}
step-3 digest: b8c5e7f3a0d4c2b1e9f8a7d6c5b4a3f2e1d0c9b8a7f6e5d4c3b2a1f0e9d8c7b6a
actual sha256: eb70b332a71ea6c8aab9a3525ef0906f1875f64778b8f2d2f2ebcf27081c6b03
```

**The published digest is 65 hex characters. SHA-256 is 64.** No
implementation can match it, and it is not the digest of the bytes printed
directly above it.

Severity: the *bytes* are correct and every real port reproduces them, so
this is not a live conformance risk. It is a credibility risk: the one
example the spec designates as authoritative ground truth is wrong, in a
section whose purpose is to remove doubt. A reader checking their work
against it gets a false failure.

Independently reported by reader3 (A-25) and reader4.

---

## V2 — two independent agents converged on 5 of 5 named defects

Reader3 (C99 fold, forbidden from reading any implementation) and reader4
(auditor, no code) were separate agents given the same spec and no
contact with each other. Both raised all five of:

| topic | reader3 | reader4 |
|---|---|---|
| `kind: null` has no class, but §5 must rank every record | A-12 | highest-risk clause |
| §2.5's pinned digest is not the SHA-256 of its bytes | A-25 | top-2 finding |
| §3's table contradicts itself on `1e-05` vs `0.00001` | A-22 | finding B3 |
| §2 mandates `localeCompare` where §1/§2.2 say code point | A-3 | — |
| §5.2 and §5.4 describe two different topological orders | A-13 | finding D1 |

This is the strongest evidence the project has produced. Conformance
testing cannot produce it: it only shows implementations agree. Two
independent readers landing on the same defects shows the *defects are in
the text*, not in one author's reading of it.

The `kind: null` case is the sharpest. §1.1 makes `kind` optional with
default `null` and says other values' semantics are "out of scope for the
fold" — in a document whose §5 output depends entirely on the class
ranking. Nothing catches it: the leaf hashes `kind` as a byte either way,
the closure root never reads the ranking, and §6 compares a port against
itself and returns `VERIFIED`.

---

## V3 — an unassigned ambiguity, found by divergence alone

Three readers over 10 discriminating probes. Two failures, with
**different causes**:

| probe | r1-py | r2-js | r3-c | cause |
|---|---|---|---|---|
| p01 class vs ancestry | override | task | override | **assigned** — r2 was told to read it the other way |
| p07 orphan descendants | y applies | y applies | y dropped | **unassigned** — nobody told r3 anything |
| p10 id validation | accepted | accepted | rejected | **unassigned** |

p01 is a controlled contrast: it proves the clause admits two defensible
readings, and nothing more.

**p07 is a real finding.** Nothing instructed reader3 to diverge on §5.5.
It says an orphan's descendants are "NOT auto-orphaned" — and the two
readings differ on whether the descendant's *own effects apply at all*.
That is a silent state difference, not a cosmetic one, and it is in the
part of the spec that governs what a missing parent does to a whole
branch.

Reader3 shipped `--order`, `--conflicts`, `--required` and `--strict-utf8`
switches: it found the same ambiguities and made them executable.

---

## What this does NOT establish

Readers 1 and 2 were written by me. Their agreement with each other is
evidence of nothing — it only shows I did not make two mistakes. Reader3 is
the only independent implementation, so **every claim of cross-port
agreement here rests on a sample of one.**

And V3's p07/p10 divergence is currently a 1-vs-2 split where the 2 share
an author. It is a hypothesis that deserves a second independent reader,
not a settled result.
