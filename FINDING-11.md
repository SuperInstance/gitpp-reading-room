# Six defects between the workflow and a single executed case

I went to check whether a badge had ever been green, on the theory that
a suite nobody has watched fail is a suite nobody has watched run.

**The fold-conformance CI had never produced a successful run.** Zero
successes across the entire window the API returns.

Six defects stood between the workflow and one executed case. None was
visible from the badge, because the badge was reporting a *setup*
failure that looked exactly like a *test* failure.

| # | defect | how it presented |
|---|---|---|
| 1 | workflow cloned `gitpp-doctor` — **404** — instead of `git.pp-doctor` | job died at "Clone the ports" |
| 2 | `run.mjs` hardcoded the C# wrapper at `bin/Debug`; the workflow builds `-c Release` | cs could never load |
| 3 | workflow built the C# **library**, never the `CsDerive` **wrapper** | corpus failure, not a build failure |
| 4 | `CsDerive.csproj` referenced `/tmp/ports/gitpp-cs/...` | built on one machine only |
| 5 | `PyDerive.py` imported a scratch module — **and it was not the port at all** | a different implementation wearing the port's name |
| 6 | `PlDerive.pl`, `JsDerive.mjs` and the workflow hardcoded `/tmp` paths | four of five participants silently absent |

Now:

```
conformance (c)           success
conformance (csharp)      success
conformance (javascript)  success
conformance (python)      success
conformance (perl)        success
```

## Defect 5 is the one that matters

The `python` participant was not `gitpp-doctor`. It was a local file
that answered to the same module name, imported via a hardcoded
`/tmp` path.

**A participant that is not the thing it claims to be is worse than one
that is absent, because it contributes a result.** An absent port shows
up as a smaller denominator. A substituted port shows up as agreement,
and agreement is exactly the signal the suite exists to produce.

I introduced that substitution myself, while setting up a local
environment — and it survived several rounds of local runs, because
locally the substituted module happened to be a *correct* fold. It
looked right for the same reason a broken harness looks right.

## What made defects 2–6 legible

The participant assertion added earlier in the same branch: a port that
cannot load is a **failure**, not a smaller denominator.

A run over four participants and a run over five report the same pass
count. Nothing in the output distinguished them — which is how the `cs`
port could have been missing this entire time, and how `py` could have
been a different implementation, without anyone noticing.

One assertion turned an uninformative "0 pass, 29 fail" into
`[participants missing: py, pl]`, and that message is the entire
diagnosis.

## The two traps, recorded because they are not obvious

**Perl's `use lib` runs at compile time.** Building the path list in a
preceding `my @candidates` is too late — the `use` statement is compiled
before the assignment executes, and it silently searches the wrong
directories. The list has to be built inline from `%ENV`. This cost me
one wrong turn and would have produced a wrapper that "worked" locally
and searched nothing in CI.

**A hardcoded path is a claim about the world.** `/tmp/ports/gitpp-cs`
was true on exactly one machine and became a silent participant
elsewhere. Every wrapper now resolves from an environment variable with
the sibling checkout as the default.

## The generalisation

This is the sixth consecutive finding in the same family, and the first
where the *infrastructure* was the defect rather than the code under
test. The chain is always the same:

1. Something is labelled verified.
2. Nobody checks *how*.
3. The label holds — the thing does work, locally, on the inputs
   someone thought of.
4. The gap is invisible because it is not a value, it is an *absence*:
   of a run, of a participant, of a comparison.

**"It passes" and "it ran" are different claims, and this project
conflated them for months.** The distinction is cheap to check — look
for a successful run, count the participants — and neither had been
done.
