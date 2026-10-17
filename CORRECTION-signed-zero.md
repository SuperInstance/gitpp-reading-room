# Correction: the "gitpp-cs L1/signed-zero" defect I reported was NOT a defect

I have listed this in `FINAL-STATE.md` and `SESSION-SUMMARY.md` as an
open defect in `gitpp-cs`:

> **`gitpp-cs` `L1/signed-zero`** — emits `0.0` where §3 wants `0`.

**That is wrong.** The port is correct, and the claim was mine, made
without checking the spec's scope note. Let me be precise about what
the check actually shows.

## What the port does

`d19-float-edge-in-effects` carries `negzero: -0.0`. The spec's §3
table says:

| Input | Encoded form | Note |
|---|---|---|
| `-0.0` (signed zero) | `0` | (the sign is dropped) |

And the C# port's **leaf** is:

```
7f42758d690681383f675c2212c9b6a95bd26a29c7cd5cf64b2d308422799fd3
```

which is **byte-identical to the reference port's leaf** for the same
record. The sign IS dropped in the canonical form, exactly as §3
requires.

## Where the difference actually is

Only in the **state** the port returns:

```
cs   {"tiny":1E-05,"huge":1.5E+20,"negzero":-0,"int":42}
py   {"tiny":1e-05,"huge":1.5e+20,"negzero":-0.0,"int":42}
```

`-0` versus `-0.0` — the same float64, serialised differently. And the
spec says, in its scope note, that this is **not** a conformance
matter:

> **§3 pins the encoding of the *leaf* only.** The serialization of the
> *state* returned by `derive()` is deliberately **not** pinned: ports
> may serialize the same float64 differently ... without any of them
> being wrong. Cross-port **fold** conformance must compare state **by
> value**, not by bytes.

So the C# port is conformant, the reference is conformant, and the
difference is in a field the spec explicitly leaves open.

## Why I got it wrong

The conformance corpus's `expect` for `d19` stores `"negzero": 0`, and
the harness compares state **by value** — so the case passes. I read a
*description* of the failure ("emits `0.0` where §3 wants `0`") from
an earlier audit and carried it forward as a defect without re-running
the port against the case.

I did not check the leaf. Had I, the two hashes are identical and the
claim dies immediately.

## The generalisable error

> **A defect claim that names a field, a value, and a port is still a
> claim until you re-run the thing.**

I have spent this whole session insisting that every number needs a
demonstrated failure, and I filed a defect in my own summary that had
no demonstrated failure behind it — only a report, from a subagent, that
I did not verify. The report was about the *harness* being lax
(`expected_order` and `expected_orphans_includes` are ignored by the
official runner) and I converted it into a *port* defect, which it is
not.

That is the same shape as every other finding in this session, pointed
at me: **output without a verdict.** A subagent's observation is output.
The verdict is running the port against the case and reading the leaf.

## What survives

The harness laxity is real and still worth fixing:

> its official runner also ignores `expected_order` and
> `expected_orphans_includes`

Those two fields are in the corpus and the runner does not check them,
so a port that gets the order or the orphan set wrong can still pass
the suite. **That** is the defect — and it is in
`gitpp-fold-conformance`, not in `gitpp-cs`. It is exactly the
"output without a verdict" shape: the runner computes the right
comparison and then does not perform it.

Filed and fixed as a separate piece of work.
