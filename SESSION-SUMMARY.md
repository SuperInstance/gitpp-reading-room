# What this session did, and what it cost

## Merged

| repo | PRs | what |
|---|---|---|
| `gitpp-fold-conformance` | #1, #2 | the comparator that compared nothing; two blind spots in the harness |
| `gitpp-cs` | #1, #3 | `kind: null` threw; conflict ledger's `losers` order |
| `gitpp-js` | #1, #3 | tie-break was not topo order; conflict ledger's `losers` order |
| `gitpp-pl` | #1 | conflict ledger's `losers` order |
| `gitpp-cross-port` | #1, #2 | the C port died above 128 KB; the walker discarded half the corpus |
| `negctl` | #1 | the negative control had never run, and could not report its own failure |
| `gitpp-beam` | #3 | three Elixir canonicalisation defects, found by finally executing the port |

**Ten merged.**

## Open

| repo | PR | what |
|---|---|---|
| `gitpp-js` | #4 | cycle descendants are orphans |
| `gitpp-cs` | #4 | cycle descendants are orphans |
| `gitpp-pl` | #2 | cycle descendants are orphans |
| `gitpp-cross-port` | #3 | the mutation pass had a generator and no runner |

## The four things established

**1. A number without a demonstrated failure is not a result.** Four
harnesses produced a green they could not fail — `run.mjs` compared 25 of
29 cases against nothing; `compare.py` counted an absent reader as
agreement; `cleaf.mjs` let a port die and reported it as a crash; and
`mutate.py` produced 131,499 mutants that nothing ever judged.

**2. A fix in a branch is a proposal; a fix in `main` is a fact.**
Rebuilding the ports from merged `main` and re-running found a *fifth*
hardcoded-path defect — the Perl wrapper — that no amount of work on the
branches would have surfaced.

**3. A corpus's reach is a property of its generator.** The base corpus
has **0 of 20,043** records containing an escape sequence; the mutant
corpus has **14,611 of 131,499**. That single number explains why
`gitpp-c#1` survived weeks, and it was measured rather than argued.

**4. Output is not a verdict.** The mutation pass was the most valuable
check in the fleet and was never wired up, because writing the generator
felt like the work and writing the runner did not.

## A methodological trap worth recording

My first planted defect was **inert**: it reversed a key sort inside a
function that builds an *object*, and object key order does not affect
the hash. The base corpus "caught" it on 200 of 200 records, so I could
have published "both corpora catch it" and been wrong about the
mechanism.

The test for a planted defect is not *"does it fail?"* but *"does it fail
for the reason I planted it?"* A defect in a no-op path is a decoration.

## Still open, and named

- **`gitpp-c#1`** — `parse_str` drops `\uXXXX` escapes. A hash
  collision, not a formatting difference: `{"c":"ab"}` and
  `{"c":"a\u0001b"}` get the same leaf. **The one confirmed defect in a
  maintained port still live.**
- **`gitpp-c#2`** — an absent or `null` `kind` is promoted to `"task"`,
  so an unclassifiable record outranks a `witness`.
- **`gitpp-cs`** — `L1/signed-zero` emits `0.0` where §3 wants `0`, and
  its official runner ignores `expected_order` and
  `expected_orphans_includes`.
  - **`gitpp-doctor` is NOT a 404 repo** — it is the clone *directory* name in the
  conformance workflow. "Five ports" means
    four.
  **CORRECTED — this was wrong.** `gitpp-doctor` is the clone *directory*
  name in the conformance workflow, not a repository. The five ports
  are c, `git.pp-doctor`, js, cs and pl, and all five are real. See
  `CORRECTION-five-ports.md`.
- **`reader3-c`** — the independent C reader, deliberately left
  diverging. It is the only implementation not authored by the spec's
  author, and conforming it would leave an independent reader that
  agrees with everything.

## One operational note

GitHub's API answers a throttled or rate-limited write with **404**, not
429. The body is `{"message": "Not Found"}` and it is indistinguishable
from "this PR does not exist" unless you check whether the PR is
actually open. Several times in this session a merge or a PR creation
"failed" with that body and the operation had already succeeded. The
rate-limit headers are the only reliable signal:

```
x-ratelimit-remaining: 4926/5000     <- core, fine
```

and the failure was a *secondary* limit on writes specifically. The
reliable workaround is a plain `{"merge_method": "merge"}` payload
without `commit_title`/`commit_message`, spaced by a few seconds.
