# What landed, and what it cost to land it

Eleven pull requests were opened across the session. All eleven are
now merged. The interesting part is not that they merged — it is what
had to be fixed *to* merge them.

## The merges

| repo | PR | what |
|---|---|---|
| `gitpp-fold-conformance` | #1 | `cmp` was a 1-arg function; 25 of 29 cases compared against nothing |
| `gitpp-fold-conformance` | #2 | the spec comparison must be demonstrably exercised; every participant must be present |
| `gitpp-cs` | #1 | `kind: null` threw `ArgumentNullException` |
| `gitpp-cs` | #3 | conflict ledger's `losers` must be in §5.4 order sequence |
| `gitpp-js` | #1 | tie-break must be topo order, not an ancestor-count heuristic |
| `gitpp-js` | #3 | conflict ledger's `losers` must be in §5.4 order sequence |
| `gitpp-pl` | #1 | conflict ledger's `losers` must be in §5.4 order sequence |
| `gitpp-cross-port` | #1 | the C port died above 128 KB of payload, silently |
| `gitpp-cross-port` | #2 | the record walker discarded half the mutant corpus |
| `negctl` | #1 | negctl had never run, and could not report its own failure |
| `gitpp-beam` | #3 | three Elixir canonicalisation defects, found by finally executing the port |

## The cost of merging

Four of the eleven needed work that had nothing to do with the defect
they fixed:

**A merge conflict on `cases.json`.** PR #1 and PR #2 both corrected
the d10/d20 stored orders, because both found the same stale
expectation independently. The conflict was resolved to #2's version —
they were the same fix — and the two runners' assertions were
verified present in the merged file before committing.

**A merge conflict on `results.json`.** `gitpp-cross-port#1` and
`#2` each recorded a *different run*: one clean, one deliberately
sabotaged. The file is rewritten by `fuzz.mjs` on every run, so a
tracked copy is a snapshot of one moment, and two branches recording
different moments will always conflict — with conflict markers landing
inside a JSON document. It is now gitignored, with the receipts in the
commit message where they belong.

**A hardcoded `/tmp` path in a fourth wrapper.** Verifying the merged
`main` exposed `wrappers/PlLeaf.pl` still using
`use lib "/tmp/gitpp-pl/lib"`, while the other three runners already
resolved their port from the environment. Running the 20,043-record
suite on a machine that is not the author's required **four separate
workarounds** — a documented corpus path, `GITPP_DOCTOR_DIR`,
`GITPP_JS`, and a copy of `gitpp-pl` into `/tmp`. Fixed, and the suite
now runs with no workarounds at all.

**GitHub rejecting the merge API for reasons that looked like
throttling.** `squash` returned 404 for some PRs while `merge`
succeeded for others; adding `commit_title` and `commit_message`
reproduced the failure on PRs that had already merged with a bare
`{"merge_method": "merge"}`. The API's write throttling and its
secondary-rate-limit responses both surface as a 404 body, which is
indistinguishable from "this PR does not exist" unless you check
whether the PR is actually open.

## The verification that mattered

Merging is not the claim. **Rebuilding the ports from merged `main`
and re-running the suites is.**

```
fold conformance, 5 real ports, built from merged main
    29 pass, 0 fail, 0 skip

leaf conformance, 20,043 records, from merged main
    20043 agree, 0 disagree, 0 errors   13.9s
```

Both numbers come from the merged trees, not from the branches I had
been testing against. The second one needed the wrapper fix first,
which is precisely why running it on merged `main` was worth doing:
**the defect had been fixed in a branch and the suite had never been
run without the workaround.**

## The generalisation

> A fix in a branch is a proposal. A fix in `main` is a fact — and the
> difference is only visible by rebuilding from `main` and running
> again, which is the one check every other check in this session
> implicitly assumed.

That check found a fifth instance of the hardcoded-path defect. It
would not have been found by any amount of further work on the
branches.
