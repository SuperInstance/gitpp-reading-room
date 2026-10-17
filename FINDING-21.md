# A reusable workflow that could not be called, and a check that compared nothing

This session's last chain of defects is about **infrastructure that looks installed and is not**.

## The zero-job failure

`SuperInstance/gitpp-pl` is the only port that called the shared CI in `SuperInstance/quilt-ci`:

```yaml
uses: SuperInstance/quilt-ci/.github/workflows/conformance.yml@main
```

Every run for that port was:

> **failed, with ZERO JOBS** — no error, no steps, no explanation.

Four separate causes, all of which had to be fixed:

| # | defect | consequence |
|---|---|---|
| 1 | the workflow files were at the repository **root**, not `.github/workflows/` | the reference resolved to a path that did not exist |
| 2 | no `on: workflow_call` | a reusable workflow that **cannot be called** |
| 3 | the workflow **ignored `inputs.conformance_script`** and ran a hardcoded runner | the caller's script was silently discarded |
| 4 | a second job calling **itself** | infinite recursion once the file is in the right place |

> **A workflow that resolves to nothing produces a failure with no trace.**
> "Zero jobs" is a failure mode, not an absence of one.

The first three made the fourth invisible: the file was never in the place where the self-call would have been recursion.

## The YAML error that hid all of it

GitHub's own message, once the run got far enough to be validated:

> Invalid workflow file: `.github/workflows/conformance.yml#L84` — You have an error in your yaml syntax on line 84

Line 84 was the **embedded Python**, sitting at a shallower indentation than the YAML block scalar it lived in:

```yaml
      conformance_script: |
          verdict=$(... | python3 -c "
        import json, sys          # <-- shallower than the scalar's base
```

The scalar ended at the first line indented less than its base, and the next line was parsed as a new mapping key. The whole script was never a script.

> **A block scalar's base is the indentation of its first content line.**
> Everything inside it must be at least that deep, or the scalar ends early and the remainder is a syntax error somewhere else entirely.

## The check that compared nothing and passed

After all that, the check ran and reported **success** — over **zero cases**:

```
find: './conformance/cases': No such file or directory
```

The reusable workflow clones the corpus to `<workspace>/conformance` and checks the port out at `<workspace>/port`. The script runs with the port as its working directory, so the corpus is at `../conformance`, not `./conformance`.

A wrong path yields zero cases, the loop body never runs, and the script exits 0.

> **A GREEN BADGE ON A CHECK THAT CHECKED NOTHING.**
> And this was in my own new code, introduced *while fixing the very thing it was about*.

The fix asserts the count twice — cases **found** and cases **compared** — because those are different failure modes: a wrong path finds none, and a broken input resolution finds 30 and compares 2.

The corpus has 30 cases and 2 that carry a prose `input_modification` with no `input_tampered`, so 28 is exact — and the cross-port harness independently names the same 2 as known gaps.

## An unset counter in a threshold test

```bash
if [ "$compared" -lt 28 ]; then ... exit 1; fi
```

with `compared` never initialised. Bash errors on the integer expression, the test is false, and the script passes.

> **AN UNSET COUNTER IN A THRESHOLD TEST IS A TEST THAT NEVER FIRES.**
> `[ "" -lt 28 ]` is not an error the script notices.

## The live deployment is flaky, and flaky means "skipped"

Two separate facts about `gitpp.superinstance.dev`:

- it answers urllib's default `Python-urllib/3.x` with **HTTP 403**, while the same URL with any other User-Agent returns 200. The traceback points at urllib, not at the deployment;
- it answers **HTTP 503 intermittently** — the same URL returns 200 seconds later.

Both turn a fetch into an empty input, and an empty input turned into a *skip*. Eight live cases became eight skips and the corpus shrank.

> **A FLAKY DEPENDENCY MUST NOT BE ALLOWED TO SHRINK THE CORPUS.**
> A fetch that fails after its retries is a **named** skip, and the count assertion catches the shrinkage.

## And the two rules that would have caught all of it

```bash
# 1. the count must be asserted, before AND after the loop
if [ "$NCASES" -lt 28 ]; then exit 1; fi
...
if [ "$compared" -lt 28 ]; then exit 1; fi

# 2. the check must be able to fail
#    a port sabotaged to return a constant, VALID, wrong root gives RC=1
```

The second is the one that matters most, and it is the same rule as everywhere else in this project:

> **A check that cannot fail is not evidence.**
> And a check that cannot fail *because it ran nothing* is the worst version of it.

## The generalisation

This session found the same class of defect in **four different harnesses**, and the class is:

> **Something that is supposed to run, does not — and the way it fails looks like success.**

- a reusable workflow that resolves to nothing → a failure with zero jobs
- a corpus path that resolves to nothing → a green badge over zero cases
- a participant that cannot load → an absent reader counted as agreement
- a port that errors → a sibling port's error shielding a wrong answer

The cure is the same in all four: **assert that the thing you are measuring was actually measured, and fail when it was not.**
