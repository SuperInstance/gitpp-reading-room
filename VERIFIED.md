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

---

# Addendum: the experiment produced a spec change (v0.4.6)

Branch `spec/5-1-clarity` on `git.pp-spec` carries all five fixes. Each
is traceable to a measurement:

| change | evidence |
|---|---|
| §5.1 ancestry scope stated | Jev 0.960 vs 0.030; probe set 6→2 disagreements when patched |
| §5.1 unclassifiable ranks below witness | 3 readers concur; Jev 0.760 that text is *undetermined* |
| §5.2 equivalence claim completed | 4 hand-checked cases; 1 fails |
| §5.5 descendant effects stated | Jev 0.790 vs 0.100 |
| §2.5 digest corrected | 65 hex chars, not a SHA-256 of the bytes above it |

## The bug the experiment found in independent code

`reader3-c` computes orphans as *anything Kahn never emitted*
(`fold.c:1682`), justified by a comment claiming §5.5 "falls out for
free" — a missing parent blocks its child forever, so the child is never
emitted, hence an orphan.

**The implication does not hold.** Kahn-blocking is strictly stronger
than §5.5, which defines an orphan by a record's *own* parents. A
descendant of an orphan is blocked transitively and is wrongly orphaned,
which is the precise thing §5.5's closing sentence forbids.

It is subtle because a record downstream of a *cycle* genuinely should
be orphaned, and the same expression covers both cases — right for one,
wrong for the other. Filed as
[gitpp-rr-reader3#1](https://github.com/SuperInstance/gitpp-rr-reader3/issues/1).

## How the 6 -> 2 reduction happened, and what it is worth

Patching the ancestry-first reader to class-first took the probe set from
6 disagreements to 2. That is a **real** result: it shows the spec change
resolves a clause that readers genuinely read differently.

It is not a result about the *other* two. Those persist because reader3's
orphan rule is wrong, and the harness correctly keeps reporting a
disagreement it has not yet explained. A fix that had made the number
look better without fixing the cause would have been the failure mode
this whole repo was built to detect.

---

# Addendum: the fix was itself incomplete, and the harness caught it

v0.4.6 shipped four clause fixes. The next question is the one a spec
author is least inclined to ask — **is the new text better than the old?**

Probing the new text found two real gaps in it (0.700 and 0.790
undetermined, the latter a near-tie at 0.790/0.860), repaired in a
follow-up commit: unclassifiable records are a *single class*, so a
contest between two of them falls to §5.2 step 3 like any same-class
pair.

Three probes added: `p14` (unclassifiable vs unclassifiable), `p15`
(unclassifiable descendant), `p16` (unrecognised kind string). All
three are **unanimous across the three readers**, and all three match
the repaired reading:

| probe | py | js | c | unanimous |
|---|---|---|---|---|
| p14 | x=1 | x=1 | x=1 | yes |
| p15 | x=2 | x=2 | x=2 | yes |
| p16 | z=from-witness | z=from-witness | z=from-witness | yes |

Not one of the three readers was told about the repair. They reached
the repaired reading because "unclassifiable is its own class, so the
ordinary tie-break applies" is what falls out of the code — which is
evidence the repair is the natural reading and not an arbitrary one.

Corpus is now 16 probes; 14 agree, 2 disagree. Both remaining
disagreements are the known reader3 orphan rule
(`gitpp-rr-reader3#1`), which is a bug in that port rather than an
ambiguity in the text.

---

# Addendum: v0.4.6 codified existing behaviour — the ports were already right

A spec change is only worth what it changes. So after shipping v0.4.6,
the obvious question: do the ports actually diverge now, or did the spec
catch up to code that was already correct?

`gitpp-js` and `gitpp-c` were run against the seven v0.4.6-discriminating
cases, unbuilt from the spec and unaided:

| case | gitpp-js | gitpp-c |
|---|---|---|
| p14 unclassifiable vs unclassifiable | x=1 ✓ | x=1 ✓ |
| p15 unclassifiable descendant | x=2 ✓ | x=2 ✓ |
| p16 unrecognised `kind` string | z=from-witness ✓ | z=from-witness ✓ |
| unclassifiable, uncontested key | q=solo ✓ | q=solo ✓ |
| override ancestor vs task descendant | x=override ✓ | x=override ✓ |
| same-class unrelated siblings | x=1 ✓ | x=1 ✓ |
| orphan's descendant applies | y=from-kid ✓ | y=from-kid ✓ |

**7/7 each. Neither port had read v0.4.6.**

The reason is visible in the code and is a genuine piece of evidence:

```js
// gitpp-js/src/gitpp.js:298
const topClass = Math.max(...candidates.map(r => CLASS_RANK[r.kind] || 0));
const top = candidates.filter(r => (CLASS_RANK[r.kind] || 0) === topClass);
```

```c
/* gitpp-c/gitpp.c:632 */
static int class_rank(const char *kind) {
    if (!kind) return 0;
    if (strcmp(kind, "override") == 0) return CLASS_OVERRIDE;
    if (strcmp(kind, "task") == 0) return CLASS_TASK;
    if (strcmp(kind, "witness") == 0) return CLASS_WITNESS;
    return 0;
}
```

Both had been mapping an unknown kind to rank 0 — below `witness` — and
both then treat rank-0 records as a single class, so the ordinary
tie-break applies. That is **exactly** the repaired reading, arrived at
independently years before the spec said it.

So the value of v0.4.6 is not that it changed the fold. It is that it
made a long-standing implicit behaviour **explicit and checkable**, and
it did so by naming a rule three separate readers had already
implemented by hand. A spec that only records what code does is
worthless; a spec that predicts what independent implementations do is
evidence the rule was the right one to state.

### A harness note worth keeping

`gitpp-c` first scored **0/7**, and every state came back `null`. The
cause was my driver, not the port: `gitpp <file.json> --at <id>` returns
a *closure root*, and `derive()` needs `--derive`. A CLI that requires a
flag to produce the thing you asked for, with no error when you omit it,
is a small trap — worth remembering before reporting any 0/N from a
multi-port run.

---

# Addendum: the C# port crashes on the case v0.4.6 was written for

`gitpp-cs` scores **5/7** against the seven v0.4.6-discriminating cases.
It does not disagree with the spec — it **throws**.

```
Unhandled exception. System.ArgumentNullException: Value cannot be null. (Parameter 'key')
   at System.Collections.Generic.Dictionary`2.FindValue(TKey key)
   at System.Collections.Generic.CollectionExtensions.GetValueOrDefault[TKey,TValue]
   at Gitpp.Fold.Derive(IEnumerable`1 recordsList) in src/Gitpp.cs:line 215
```

`Gitpp.cs:215`:

```csharp
if (precedence.GetValueOrDefault(newKind, 0) > precedence.GetValueOrDefault(existingKind, 0))
```

The intent is right — an unknown kind maps to rank 0, which is exactly
what v0.4.6 now mandates. The bug is that .NET's
`Dictionary<string,int>.GetValueOrDefault(null, 0)` **throws** rather than
returning the default: a `Dictionary` rejects a null key outright,
including through the `CollectionExtensions` overload that is *supposed*
to be null-tolerant. The author wrote the correct rule and the runtime
silently disagreed.

Both failing cases are the ones with **two `kind: null` records
contesting a key** (p14, p15). A *single* null-kind record passes,
because line 215 is only reached once a key is already set.

## Why this is the best result in the whole series

- `gitpp-js`, `gitpp-c`, and now `gitpp-cs` were all run **without being
  told v0.4.6 exists**. Two were accidentally right. One crashes.
- The crash is on **precisely the case the spec change was written to
  close**. The `kind: null` hole was found by two independent agents
  reading the spec, confirmed by an adjudicator at 0.840, fixed in the
  spec, and the fix immediately surfaced a real defect in a maintained
  port.
- It is a **language-specific trap**, invisible to cross-port testing that
  only checks *agreement*. Three ports agreeing on 20,043 leaf cases and
  29 fold cases would never have caught this, because no corpus case
  contained two `kind: null` records contesting a key. The existing
  29-case fold suite had no `null`-kind case at all.

That last point is the generalisable lesson, and it is the same one the
whole `gitpp-reading-room` project was built to demonstrate:

**agreement between implementations only tests the cases you thought of.**
A defect that requires a specific record shape to surface is invisible to
conformance testing by construction, and only becomes visible when a
*specification* tells you the shape is legal.

## The fix

```csharp
static int Rank(string kind) =>
    kind is null ? 0 : precedence.GetValueOrDefault(kind, 0);
```

and call `Rank(...)` at both sites. One helper, one null check, applied
consistently — the defect is that a null-tolerant *default* was confused
for a null-tolerant *dictionary*.
