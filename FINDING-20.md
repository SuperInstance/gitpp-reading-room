# The harness could not fail — and the same bug was in three harnesses

This session's deepest result is not a bug in a port. It is that **the harnesses which were supposed to catch bugs in the ports had the same bug in them**, and each of them had it in the same shape.

## The shape

> **A port that is absent, or that errors, is turned into a VALUE.**
> The comparison then finds agreement.

Found independently, in three different harnesses, in this project:

| harness | the substitution | result |
|---|---|---|
| fold conformance | a missing participant counted as agreement | 25 of 29 fold cases compared against nothing |
| cross-port leaf | `leaves[name][k] ?? "MISSING"` — an absent port's *value* | a port that printed nothing counted as a disagreement, and a case where 4 of 5 ports printed nothing counted as **ALL-AGREE** |
| cross-port fold | `catch (e) { errors++; leaves[name] = [] }` | `0 agree, 200 disagree, 0 errors`, **exit 0** |

And now, in the same repo, a fourth instance in `run.mjs`:

```js
if (uniqueRoots.length === 1)           allAgree++;
else if (roots.length < ports.length)    errors++;
else                                     disagree++;
```

A **disagreement was only counted when EVERY port answered.** If any port errored, the case was filed as an "error" and the disagreement among the ports that *did* answer was never counted at all.

**That is precisely how `gitpp-c`'s SPEC §4 violation survived.** The python reference exits 4 on the incomplete-closure case — it signals an incomplete closure with a non-zero exit rather than a null root — so the case was filed as an error, and the C port's *wrong root* was never counted as a disagreement.

**A sibling port's error shields it.**

## The rule this yields

> **A port that cannot answer must be a FAILURE, never a value.**
> And a disagreement must be counted INDEPENDENTLY of whether other ports failed.

Both halves are needed. The first stops an absent reader from voting. The second stops a failed reader from covering for a wrong one.

## The other half: there was no exit code

`run.mjs` printed its verdict and exited **0, always**. `fuzz.mjs` printed `0 agree, 200 disagree, 0 errors` and exited **0**. `run_mutants.mjs` printed `0 agree, 200 disagree, 1 errors` and exited **0**.

Three harnesses. All three. Each one was capable of reporting a catastrophic failure and reporting it as success.

> **A harness that cannot fail is not evidence.**

## The third defect: a corpus silently shrank

```js
if (!records && c.json.input_source === "live") records = await fetchLive();
```

`input_source` was compared to the **literal string** `"live"`. Every live case actually sets it to a **full URL**:

```json
"input_source": "https://gitpp.superinstance.dev/api/records?project=poc-quilt-1"
```

so the comparison **never matched**. **10 of the 30 cases — a third of the corpus — were never tested**, and the run still printed a clean agreement count.

The corpus's own runner gets it right:

```js
if (!records && c.input_source && c.input_source.startsWith("https://"))
```

**Two runners implemented the same corpus, and the weaker one produced this fleet's headline receipt.**

> When two implementations of the same specification disagree, the one that produces the receipt is not automatically the one that is right. Diff them.

## The fourth defect: the repo could not bootstrap itself

`setup.sh` was a **JavaScript program with a `.sh` extension** — a near-copy of `run.mjs` with a live fetch added. **Neither** file created `ports/` or `cases/`.

That is why the repository had **no CI at all**: there was nothing that could be automated.

## The fifth defect: a wrapper that tested the wrong build

```perl
use lib "/tmp/gitpp-pl/lib";
```

A hardcoded scratch path. In CI it does not exist, so the port cannot load. On a developer machine it **did** exist and held a **different build** — so the wrapper quietly tested code that was not the code under test.

I hit this while measuring a real cross-port disagreement. A leaf comparison came back *"the leaves match"* when the port under test **had not been loaded at all**. Had I trusted it, I would have concluded the disagreement was in the closure root when it was in the leaf.

> **A participant that reports agreement without having been asked is worse than a participant that is missing.**

## What the fix looks like

1. A real **exit code**, with the failure reasons printed.
2. A disagreement counted **independently** of whether other ports failed.
3. All-agree requires **every** port to have answered.
4. A **skipped** case is counted, and an **unexpected** skip fails the run. A known gap is **named**.
5. A **roster assertion** before the run: a participant that cannot run is a failure, not a smaller set.
6. A **preflight** that proves each participant loads before the batch begins.
7. A **ledger** that marks batches **invalid** (`records_not_compared`) and never counts them as agreement.
8. Each port checked against **the spec**, not just against its siblings. All five ports agreeing on the wrong answer is a failure that port-vs-port agreement cannot detect.

## A separate class: the corpus blind spot

`gitpp-pl`'s `decode_records` used `JSON::PP->new->utf8(0)`, so a raw non-ASCII byte was read as Latin-1 and the leaf was the hash of a **different string**.

The **20,043-record leaf corpus could never see it**, because that corpus is generated with `\uXXXX` escapes — pure ASCII, where the decode flag is irrelevant. The **cross-port case corpus** uses raw utf-8, which is what every ordinary editor produces.

> **The two corpora had complementary blind spots and the defect lived in the gap between them.**

This is the same shape as the project's earlier finding that a *leaf* corpus cannot detect *fold* bugs. The generalisation:

> **A corpus tests the cases its generator happens to produce. Two corpora that differ in representation can be blind to each other's defects.**
> Cross them: a case corpus in one representation and a leaf corpus in another.

## A correction worth recording

An earlier comparison in this session reported *11 disagreements* on the leaf corpus. **The oracle was wrong, not the port:** it used raw `json.load`, which leaves a bare integer literal as an arbitrary-precision `int`, instead of the port's own decode entry point (`gitpp_doctor.loads`, `parse_int=float`).

The harness in `gitpp-cross-port` already carried a comment warning about exactly this:

> Testing the runner's idea of decoding instead of the port's is how 724818294626474393600 diverged while the suite kept reporting disagreement after the port was already fixed.

And the spec settles it:

> | `int` (record supplied as JSON integer literal, `|v| >= 1e16`) | **treated as the float64 it becomes** | `724818294626474393600` → `7.248182946264744e+20` |

So the 11 records were a **measurement error** and the non-BMP case was a **real defect**. Both are now settled by the spec, not by a majority vote.

**This is the failure mode to watch for in the rest of this work: a disagreement that is really an artifact of the measuring apparatus.** A bug in the oracle looks exactly like a bug in the port, and it is much easier to misattribute.
