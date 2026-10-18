# SILENT GREEN — a census of the green badge over nothing

**Scope:** the `SuperInstance` GitHub account (5,293 repos).
**Date of scan:** repo inventory paginated to completion (53 pages, 0 short page).
**Method note:** GitHub *code search* was not used — it does not index this account, and a 0-hit
search is not evidence of absence. Everything below comes from **fetching the repository content
itself** and grepping locally. See §2 for the exact denominator and what was *not* covered.

---

## 1. The literature anchor: the 11 categories

I did not take these on faith. I pulled the paper's own HTML
(`https://arxiv.org/html/2509.14347v2`, 411 KB → 137 K chars of text) and read the RQ3 taxonomy
table and the T1–T11 theme definitions directly.

> **Aïdasso, H., Bordeleau, F., & Tizghadam, A. (2025). *On the Illusion of Success: An Empirical
> Study of Job Reruns and Silent Failures in Industrial CI*. arXiv:2509.14347 [cs.SE] (v2).
> DOI: 10.48550/arXiv.2509.14347**
>
> 142,387 jobs, 81 industrial projects (a telecom), GitLab CI. 11% of successful jobs are rerun;
> 35% of those reruns happen >24 h later. 92 public issues were hand-coded into **11 themes**.

| # | Theme (paper's name) | n | Share | In my words |
|---|---|---|---|---|
| T1 | Artifact Operation Errors | 26 | 28% | Upload/download of build artifacts fails or silently produces a stale/empty artifact; the job still ends green. |
| T2 | Caching Errors | 21 | 23% | Cache save/restore fails or restores the wrong key; the job proceeds as though the cache were warm. |
| T3 | Ignored Non-Zero Exit Codes | 17 | 18% | A script/tool returns non-zero and the pipeline does not stop; the *work* failed, the *job* did not. |
| T4 | Infrastructure Config. Issues | 10 | 11% | Runner/queue/timeout/partial-config problems that truncate a job without failing it. |
| T5 | Security Scan Failures | 7 | 8% | A scan runs, finds something, and the finding never becomes a failure. |
| T6 | Skipped Script Execution | 7 | 8% | A step or command that should run does not, and nothing records that it was skipped. |
| T7 | Post-deployment Issues | 6 | 7% | Marked deployed but not actually live in the target environment. |
| T8 | Ignored Test Failures | 3 | 3% | Failing tests that never fail the job. |
| T9 | Silent Curl Errors | 3 | 3% | `curl` fails to fetch (bad URL, DNS, 404) but the pipeline treats the response as content. |
| T10 | Invalid CI Variables | 2 | 2% | A variable is unset/empty and the pipeline behaves as though it were set. |
| T11 | Syntax Errors | 1 | 1% | A malformed config/script still runs and still passes. |

Two structural facts from the paper matter for reading this report:

* The paper's own §VI lists **T3 (ignored exit codes), T10 (invalid CI variables) and T11 (syntax
  errors)** as "CI configuration smells that produce fake successes," in the Vassallo et al. sense.
* Its validation sample (RQ4) found **58% of rerun-initiating successful jobs were genuine silent
  failures** (29 confirmed + 39 likely of 100 sampled).

### 1b. The other two anchors, verified at source

**GitHub's badge fallback — verified verbatim** from
`docs.github.com/en/actions/how-tos/monitor-workflows/add-a-status-badge`:

> "If there are no workflow runs on your default branch, it will display the status of the most
> recent run across all branches."

**pytest's exit-code-5 distinction — verified in pytest's own source**, not in a blog post:

* `src/_pytest/config/__init__.py:117-126` — `class ExitCode(enum.IntEnum)`: `OK = 0`,
  `TESTS_FAILED = 1`, … `USAGE_ERROR = 4`, and the line
  `#: pytest couldn't find tests.` / `NO_TESTS_COLLECTED = 5`.
* `src/_pytest/main.py:388-392` — `_main()` returns `ExitCode.NO_TESTS_COLLECTED` when
  `session.testscollected == 0`.
* `src/_pytest/main.py:325` — `wrap_session` seeds `session.exitstatus = ExitCode.OK` before any
  work happens.

So "no tests were collected" is a *distinguished, non-zero* state by design. Anything that rewrites
it to 0 is deliberately destroying a signal pytest went to some trouble to create.

---

## 2. Coverage — what I actually scanned, and what I did not

This matters more than the instance count, so it goes first.

| Layer | Count |
|---|---|
| Repos in account (paginated to exhaustion, 53 pages, last page short) | **5,293** |
| …of which forks | 821 |
| …primary (non-fork, non-archived, non-empty) | **4,397** |
| Default branch split | `main` 3,246 / `master` 2,021 → **38.1% on `master`** |
| Repos attempted for content fetch (size-capped ≤12 MB, ordered by triage score) | 3,072 |
| **Repos that yielded content (the scanned set)** | **2,776** |
| Repos attempted but empty (fetch/extract failed) | 296 |
| **Workflow files scanned** | **2,318** |
| **Repos with ≥1 workflow file** | **2,009** |
| **Python files scanned** | **9,945** (across 1,069 repos) |

**Honest denominator:** I scanned **2,318 workflow files in 2,009 repos, out of an unknown total**
across the account's 4,397 primary repos. I cannot state "N of M workflows" because I never
enumerated M. The 296 empty repos and all repos >12 MB are unscanned, and I deliberately excluded
forks and the lowest-scored ~1,325 primary repos. **Treat every count below as a floor, not a
total.**

### A deviation from the briefed technique, and why

The brief said to use GraphQL `HEAD:<path>` batching against a 5,000/hr budget. That budget was
genuinely spent: a tree-API sweep of 509 repos consumed it, and the API then returned
`API rate limit exceeded for user ID 193104091` for the rest of the run. (The brief's other
warning was correct — `GET /user/5293/repos` returns **404 Not Found**; the working endpoint is
`GET /users/SuperInstance/repos`.)

I found a better path and switched: **`codeload.github.com/OWNER/REPO/tar.gz/HEAD` and
`raw.githubusercontent.com` are not metered by the API quota.** One tarball per repo returns the
whole file tree *and* all contents. So the 2,318 workflow files and 9,945 Python files above were
fetched **at zero API cost**, which is why the scan covers 2,776 repos rather than 500. Repo
ordering used triage on `name`/`description`/`language`/`topics` from the inventory — deployment,
gate, approval, policy, audit, CI, publish names and Python/JS/TS/Shell/Go languages score highest.

I also avoided a self-inflicted trap worth recording: `pkill -f "api.github.com/repos/"` matched
*my own shell's command line* and killed the running sweep. And an early scanner died on
catastrophic regex backtracking — `\bcurl\b[^|;&\n]*(...)` against a 9 MB vendored
`typescript.js`. Both cost real time; both are noted so the next pass doesn't repeat them.

---

## 3. CLASS 1 — the green badge over nothing: instance table

### 3.1 The coercion family: `'' == '0'` is TRUE

This is the sharpest finding in the census, and it is **worse than one bad line**.

GitHub's own docs state the casting rule: *"GitHub casts data types to a number… **Note: empty
string returns `0`**."* A step that is **skipped** never sets its outputs, so
`steps.X.outputs.Y` is `''`, and `'' == '0'` evaluates **TRUE**.

| Repo | File | Line | Snippet | Category |
|---|---|---|---|---|
| `quilt-k3s` | `.github/workflows/chaos-ci.yml` | **129** | `${{ steps.chaos.outputs.chaos-exit-code == '0' && '✅ All scenarios passed.' \|\| '❌ At least one scenario failed.' }}` | T6 Skipped Script Execution |
| `quilt-k3s` | `.github/workflows/chaos-ci.yml` | **137** | `if: steps.chaos.outputs.chaos-exit-code != '0'` | T6 / T3 — **inverted gate** |
| `jev-lattice` | `action.yml` | **67** | `if: steps.gate.outputs.code != '0'` | T6 Skipped Script Execution — **fail-open gate** |
| `herdr-cocapn` | `vendor/libghostty-vt/.github/workflows/{nix,test}.yml` | 28, 144 | `if: always() && steps.status.outputs.result != 'success'` | T6 — **vendored third-party**, not fleet-authored |

**The `quilt-k3s` pair is a single defect that inverts in both directions.** The same coercion
drives the message *and* the job-level failure gate:

```yaml
      - name: Run chaos gauntlet
        id: chaos
        run: |
          set +e                                   # line 82 — step can never fail
          node dist/cli.js test ...
          CHAOS_EXIT=$?
          echo "chaos-exit-code=${CHAOS_EXIT}" >> $GITHUB_OUTPUT
...
      - name: Comment on PR
        if: github.event_name == 'pull_request' && always()    # <-- runs even on failure
        message: |
          ${{ steps.chaos.outputs.chaos-exit-code == '0' && '✅ All scenarios passed.' || ... }}
...
      - name: Fail the job if any scenario failed
        if: steps.chaos.outputs.chaos-exit-code != '0'          # line 137
```

When `chaos` is skipped, `'' == '0'` → **TRUE** → the bot posts **"✅ All scenarios passed."**,
and simultaneously `'' != '0'` → **FALSE** → the fail-the-job step is skipped. One coercion, both
the claim and its enforcement destroyed. Because the comment step carries `always()`, it fires
even when an earlier step already failed — which is exactly how a PR gets a green "all passed"
comment from a gauntlet that never ran.

**The `jev-lattice` instance is the more dangerous one.** It is a *composite action that is itself
the gate* (`name: JEV gates`, `branding.color: green`), and `set +e` at line 53 guarantees the
`gate` step never fails, so `code` is the **only** failure signal:

```yaml
        pass=$([ $code -eq 0 ] && echo true || echo false) >> "$GITHUB_OUTPUT"   # 58
        echo "code=$code" >> "$GITHUB_OUTPUT"                                     # 59
    - name: Fail when gates are not GO
      if: steps.gate.outputs.code != '0'      # line 67 — '' != '0' is FALSE
```

It also publishes `pass:` as an action output (line 31). When the gate step is skipped, `pass` is
`''` — **neither `"true"` nor `"false"`** — so *any* consumer testing `== 'false'` or `!= 'true'`
gets the fail-open answer. This is the one instance in the census with reach outside the account.

**Fleet-wide counter-evidence:** I grepped all 2,318 workflow files for the correct pattern —
`steps.*.outcome` / `steps.*.conclusion`. **Zero hits.** The fix documented in the brief
(`outcome == 'success'`) is used **nowhere in the account**. The disease is known and the
medicine is absent.

### 3.2 `|| true` on the test command — the widest shape in the fleet

**161 workflow steps across 135 distinct repos** discard the exit status of the very command that
is supposed to prove the code works.

| Repo | File | Line | Snippet | Category |
|---|---|---|---|---|
| `conversation-toolkit` | `.github/workflows/ci.yml` | 15 | `pip install -e ".[dev]" 2>/dev/null \|\| pip install -e . 2>/dev/null \|\| true` | T3 + T9 |
| `conversation-toolkit` | `.github/workflows/ci.yml` | 16 | `python -m pytest tests/ -q \|\| true` | T3 / T8 |
| `ai-token-counter` | `.github/workflows/ci.yml` | 15-16 | identical pair | T3 |
| `lattice-climate` | `.github/workflows/ci.yml` | 19 | `- run: pytest \|\| true` | T3 / T8 |
| `plato-i2i` | `.github/workflows/ci-rust-python.yml` | 25, 45 | `cargo test --verbose \|\| true` / `python -m pytest -x -v \|\| true` | T3 |
| `plato-training-casino` | `.github/workflows/ci-rust-python.yml` | 25, 45 | same template | T3 |
| `plato-room-persist` | `.github/workflows/ci-rust-python.yml` | 25, 45 | same template | T3 |
| `plato-tile-split`, `-ranker`, `-query`, `-import`, `plato-room-search`, `plato-room-context`, `plato-query-parser`, `plato-ghostable`, `plato-fleet-graph`, `plato-demo`, `flux-hdc`, `embedding-utils`, `deadband-python`, `cudaclaw`, `commit-predictor`, `collective-ai`, `character-skill-trees`, `character-library`, `character-agent-integration`, `cat-agent`, `casting-call-mcp` | `ci*.yml` | — | same 2-step template, one each | T3 |
| `cache-layer-optimizer` | `.github/workflows/ci.yml` | 14 | `cargo test --all-features \|\| true` | T3 |

Shape breakdown across the 161: **97 `pytest`, 30 `pip install`, 22 `npm test`, 22 `cargo test`,
1 `npm run build`.**

Two details make this worse than a flat count. First, `conversation-toolkit` and `ai-token-counter`
stack **two** erasures: the install is swallowed (`|| true`), so a missing/failed dependency
produces a collection error, and then the test command is swallowed too — so the collection error
never fails the job either. Second, `ci-rust-python.yml` is a **shared template replicated across
at least 10 `plato-*` / `flux-*` repos**: one authoring decision propagated a silent test-disable
across the fleet. This is an *edge with high fan-out*, not 20 independent bugs.

### 3.3 `continue-on-error: true` on release/publish steps

30 occurrences in workflows. Most are defensible (a flaky optional lint). Two are not:

| Repo | File | Line | Snippet | Category |
|---|---|---|---|---|
| `git-agent` | `.github/workflows/release.yml` | 17-21 | `python -m twine upload dist/*` … `continue-on-error: true  # Token may not be configured yet` | **T7 Post-deployment + T3** |
| `plato-runtime-kernel` | `.github/workflows/release.yml` | 17 | same `continue-on-error: true  # Token may not be configured yet` | **T7 + T3** |

In `git-agent`, the step after it is `softprops/action-gh-release@v2`. So a **missing or wrong
`PYPI_API_TOKEN` produces a green job, no PyPI upload, and a GitHub Release published anyway** —
pointing at a version that was never distributed. That is T3 and T7 simultaneously, and it is the
canonical "post-deployment issue" the paper says maps exclusively to release jobs.

### 3.4 Bare `set +e` — 5 fleet-authored instances

| Repo | File | Line | Context |
|---|---|---|---|
| `quilt-k3s` | `.github/workflows/chaos-ci.yml` | 82 | disables error checking around the gauntlet |
| `jev-lattice` | `action.yml` | 53 | disables error checking around the **gate evaluation** |
| `quilt-base` | `security/grype-scan.sh` | 125 | disables error checking around a **security scan** → T5 |
| `quilt-base` | `security/trivy-scan.sh` | 127 | same → T5 |
| `quilt-core-os` | `scripts/flash-pi.sh` | 167 | provisioning script |

The two `quilt-base` cases are the sharpest: `set +e` around a vulnerability scanner converts a
**finding** into a non-finding by the cheapest possible means. That is T5 (Security Scan Failures)
without any scanner bug at all.

### 3.5 Status badges with no `?event=` / no `?branch=` — T10, fleet-wide

I fetched the `README.md` of the repos with workflows (**1,974 of 2,009** retrieved; 35 404/absent) and counted workflow badges:

| Form | Count |
|---|---|
| `…/actions/workflows/X.yml/badge.svg` (**bare — inherits cross-branch fallback**) | **34** |
| `…/badge.svg?event=push` or `?branch=…` | **0** |

**All 34 workflow status badges found are bare** (34/34 carry no query string). Per the verified GitHub sentence,
every one of them can display *"the status of the most recent run across all branches"* whenever
the default branch has no runs — a stale or unrelated green.

The named instance: `plato-room-deployment-approval/README.md:5`
```markdown
[![Tests](https://github.com/SuperInstance/plato-room-deployment-approval/actions/workflows/ci.yml/badge.svg)](…)
```

### 3.6 Classes the brief named that I could **not** find

Reporting these as absent is only meaningful because code search was not my method — this is
exhaustive grep over fetched content.

| Shape | Hits | Note |
|---|---|---|
| `pytest-custom_exit_code` | **0** | Not used in any of 9,945 Python files. The exit-code-5 distinction is *not* being erased this way. |
| `suppress_health_check=list(HealthCheck)` | **0** | Not present. |
| `steps.*.outcome == 'success'` (the correct fix) | **0** | Never used — see §3.1. |
| `node test/*.test.js` **without** `--test` | **0** | See §5.2 — the fleet already fixed this shape once. |

---

## 4. CLASS 2 — the swallowed programming error

**Method.** Extracted every `except …:` handler in 9,945 Python files that has a non-empty body
(13,025 handlers), captured each handler body, then filtered to handlers that (a) **do not
re-raise**, (b) **record a verdict that reads as a normal outcome** (`log_audit`, `verdict=`,
`status=`, `decision=`, `code=0`, `"APPROVED"`/`"OK"`/`"PASS"`/`"IDENTITY"`), and (c) are **not**
honest error signalling (`raise`, `status=4xx/5xx`, `sys.exit`, `os._exit`, `.exception()`).

Raw volume before filtering — worth stating, because it is the denominator:
**8,355 `except Exception`/`BaseException`, 475 bare `except:`.** These are the *ambient* rate.
Overwhelmingly they are ordinary API handlers that correctly `return web.json_response(..., 500)`.
Those are **not** defects and I excluded them.

**Result: 12 handlers in 11 repos genuinely convert a programming error into a policy answer.**

| # | Repo | File:Line | Handler | What decision it swallows |
|---|---|---|---|---|
| 1 | `plato-room-deployment-approval` | `plato_room_deployment_approval/room.py:562` | `except Exception as e:` → `logger.warning(f"Gate re-verification failed: {e}")`; `room.log_audit("blocked", {"reason": f"gate_error: {e}"})`; `return` | **Two defects behind it.** The observable outcome — an audit entry reading `"blocked"` — is indistinguishable from a legitimate policy block. |
| 2 | `keyprobe` | `keyprobe.py:175` | `except Exception as e:` → `name = getattr(p, "__name__", "?"); code = 0` / `verdict = "IDENTITY"` | A crash in a probe becomes **HTTP `200`** and a **normal domain verdict `"IDENTITY"`**, written to `last_run.json` as a legitimate row. |
| 3 | `nexus-runtime` | `jetson/agent/tripartite/consensus.py:79` | `except Exception as exc:` → `# On error, reject for safety` → `ConsensusResult(decision=DecisionVerdict.REJECT, …confidence=0.0…)` | A programming error in consensus evaluation is emitted as a first-class **`REJECT` verdict** with fabricated confidence scores. Fail-closed, but a bug is now indistinguishable from a real dissent. |
| 4 | `quilt-casting` | `quilt-casting:510` | `except Exception as e:` → `success = False`; `error = hashlib.md5(str(e).encode()).hexdigest()[:8]`; `self._witness_observed(decision, 0, success, error)`; then retries with a local model | The exception text is **MD5-hashed to 8 chars** and recorded as the witness. The failure becomes untraceable by construction. |
| 5 | `quilt-substrate` | `quilt-substrate:510` | byte-identical to #4 | Same, replicated. |
| 6 | `plato-forge-listener` | `plato-forge-listener:80` | `self._dead_letter.append({…}); self._stats["dead_lettered"] += 1` | A handler failure increments a **success-path statistic** and returns normally. |
| 7 | `quilt-cellular-arch` | `meta_pincher_v2.py:101` | `code = 0` … `results[model] = ("FAIL", code, 0)` | Records a crash as `FAIL` **with exit code `0`** — the success code. |
| 8 | `quilt-cellular-arch` | `meta_pincher_v2.py:121` | same, `results[name] = ("FAIL", code)` | Same. |
| 9 | `quiltcall` | `:86` | `status, body, verdict = 0, str(e).encode(), "error"` | An exception is synthesised into a **plausible protocol response** (status 0, a body) rather than raised. |
| 10 | `DMLogn8n` | `master/event_bus.py:380` | `self.metrics.failed_events += 1`; `self.dead_letter_queue.append({…})` | Delivery failure counted and swallowed on the event bus. |
| 11 | `quilt-cell-harness` | `cell.py:231` | `self.witness_chain.append({…}); return {"status": "error", "error": str(e)}` | **Borderline / likely false positive** — it does return `"status": "error"`. Included for completeness; I would not call this a defect. |
| 12 | `fleet-oracle2` | `scripts/gc-pid-server.py:221` | `except KeyboardInterrupt:` → print shutdown JSON, `server_close()` | **False positive.** Clean signal handling. |

### 4.1 Instance #1, verified by execution rather than by reading

The brief described this one. I confirmed it and then **ran it**, because the claim is strong and
reading is not proof. I reproduced the exact environment `tests/test_room.py` sets up
(`sys.path.insert(0, <repo root>)`) and called the actuator:

```
--- DEFECT 1: bare `from gate import` inside _actuator_approve ---
  ModuleNotFoundError: No module named 'gate'
--- DEFECT 2: deployment_decision(...) returns a tuple, code calls .block ---
  return value: ('APPROVED', []) type: tuple
  AttributeError: 'tuple' object has no attribute 'block'
--- OBSERVABLE: what _actuator_approve records ---
  approve_deployment registered: True
  audit log: [{'event': 'blocked', 'data': {'reason': "gate_error: No module named 'gate'"}}]
  conservation.count_today() = 0   <-- test asserts == 1
```

Both defects are real and **independent**:

1. `room.py:554` does `from gate import run_all_gates, deployment_decision` — a **bare** import.
   The module's own top-of-file import is **relative** (`room.py:34`: `from .gate import …`), with a
   bare fallback only for direct-script execution (`room.py:41`). Line 554 contradicts both.
2. `gate.py:368` — `def deployment_decision(results: list[GateResult]) -> tuple[str, list[GateResult]]:`
   returns `("APPROVED", blocking_failures)`. The call site does `decision.block`
   (`room.py:558`) and `decision.reason`. A `tuple` has neither.

**Defect 1 masks defect 2.** Fixing the import alone converts a `ModuleNotFoundError` into an
`AttributeError` and the gate still never runs. That is why this is one finding with two fixes,
not two findings.

The commit that introduced it is real — from the repo's commit atom feed:
> **2026-07-20 — "fix: actuator approve now re-verifies gates before approving (was bypassing
> safety checks)"**

The safety check could never run. And the audit trail records it as `{"event": "blocked"}` — a
normal policy decision, indistinguishable from a real one.

**The consequence is inverted from what the class name suggests.** Because the gate fails *closed*,
the observable damage is **every deployment silently blocked** — a catastrophic denial of service
reported as routine policy. The sibling instance in this family (`nexus-runtime`, #3) fails closed
in the other direction. A *fail-open* variant of this exact handler shape would approve every
deployment; nothing in the type system or the linter distinguishes them.

---

## 5. The worst three, ranked by consequence

### 1st — `plato-room-deployment-approval`: the gate that can never run

Two independent defects, one swallowed handler, and a commit message asserting the opposite.
Every deployment is blocked; the audit log says `"blocked"`; a reader concludes the policy worked.
The reason string *is* in the log (`gate_error: No module named 'gate'`) but it is filed under
`event: "blocked"`, next to legitimate blocks, and no one is looking for a Python error in a
policy verdict. **Ranked first because it is the only instance in the census that is a
deployment-control system reporting total control failure as correct operation** — and because the
tests that should have caught it were themselves structurally unable to.

### 2nd — `git-agent` / `plato-runtime-kernel`: a release that publishes nothing and reports success

`twine upload` wrapped in `continue-on-error: true` with the comment *"Token may not be
configured yet"*, followed by an unconditional `softprops/action-gh-release`. The job is green, the
GitHub Release exists, and **PyPI has nothing**. Downstream consumers install a version tag that
was never published. This is T7 + T3 and it is the shape the paper's own RQ3 found maps
exclusively to release jobs. Ranked second because the blast radius is every downstream consumer,
and because unlike #1 it produces a *shipped artifact* attesting to a deploy that didn't happen.

### 3rd — `jev-lattice/action.yml`: a published gate whose fail-closed signal is `''`

`set +e` guarantees the gate step cannot fail, leaving `code` as the sole failure signal — and
`if: steps.gate.outputs.code != '0'` is **false** when `code` is `''`. The action is
`branding.color: green` and exists to be a required status check. Ranked third, above the other
`|| true` cases, because it is a **composite action consumed outside this account**, and because
the `pass` output degrades to `''` — a value that satisfies *neither* `== 'false'` nor
`!= 'true'`, so every plausible consumer test is wrong in the fail-open direction. The fleet's
complete absence of `steps.*.outcome` (§3.1) means there is no idiom in the account that would
have caught this in review.

### Honourable mention — `plato-room-deployment-approval`'s own test suite

Not a separate defect, but it is why #1 survived. `tests/test_room.py:257` asserts
`room.conservation.count_today() == 1`. My reproduction shows `count_today() == 0`. **The test
should fail.** If CI here is red, the fleet has a red gate that everyone routes around — see §6.3.
If CI is green, then something is suppressing it, and that is the more interesting finding.

---

## 6. Shapes the literature does not have

This is the part I would argue is worth more than the confirmations above.

### 6.1 The coercion inverts — the brief found one polarity, the fleet has both

The brief identified `== '0'` → false-positive. Grepping for the family surfaced that the **same
coercion is deployed in the opposite polarity** — `!= '0'` — where it is *not* a cosmetic bug but the
**sole enforcement mechanism** of a gate (`jev-lattice/action.yml:67`,
`quilt-k3s/chaos-ci.yml:137`). `'' == '0'` lies about *success*; `'' != '0'` lies about *failure*.
The second is strictly worse and the first hides it. **Neither polarity has a name.** The paper's
T6 ("Skipped Script Execution") describes the skip; it does not describe the arithmetic that makes
the skip indistinguishable from a pass, nor that the arithmetic flips meaning with the operator.

### 6.2 The action output degrades to a value that is not in its own type

`jev-lattice` declares `pass` as *"true when every gate is GO"* and yields `''` when skipped. `''`
is neither `"true"` nor `"false"`. Any consumer written against the declared contract is wrong.
This is **T10 (Invalid CI Variables) with an inverted sign**: the paper's T10 is an unset variable
behaving as *set*; this is an unset output behaving as *neither value*, so correctness depends on
the consumer's comparison direction and cannot be fixed at the producer. **No category for
"the producer's contract is violated in a way the consumer cannot detect."**

### 6.3 The red badge that trains its own bypass

`pong-quilt/.github/workflows/merge-gate.yml:37` — in-repo, in the repo's own words:

> *"Without it every run is red by construction, and **red-but-ignored gates merge red PRs
> (#48, #50)**."*

The whole census assumes the failure mode is a **false green**. This documents the opposite
pressure: a gate that is *permanently* red is functionally identical to a gate that does not exist,
because humans learn to route around it. The defect is **not** that the badge lies; it is that the
badge is so noisy that its colour stops carrying information. Nothing in the taxonomy covers
gate-decay-by-fatigue, and it is the one shape that argues for *deleting* enforcement rather than
repairing it.

### 6.4 The detector exists, names the disease correctly, and is wired into exactly one repo

`plato-room-code-review/plato_room_code_review/reviewer.py` already implements the check:

```python
148:  findings.append(f"{current_file}: `continue-on-error: true`")
150:  findings.append(f"{current_file}: `set +e` disables error checking")
```

I grepped every workflow in the account for a reference to it. **It is invoked only by
`plato-room-code-review/.github/workflows/code-review.yml` — its own repository.** Meanwhile
`quilt-k3s/chaos-ci.yml:82`, `jev-lattice/action.yml:53`, `quilt-base/security/{grype,trivy}-scan.sh`
all still carry `set +e`, and 30 workflows still carry `continue-on-error: true`.

This reframes the campaign. The account is not unaware of the defect class — it has a working
static detector with the right rule and the right wording. It is a **deployment** failure, not a
detection failure. The cheapest possible fix is not "write a linter"; it is "run the linter you
already wrote, everywhere."

### 6.5 The fleet has already written the post-mortem for this class

`quilt-claw/.github/workflows/test.yml:1-10` is the clearest statement of the disease anywhere in
the account, including the mechanism:

> *"This repo had NO .github directory at all, so nothing had ever run these tests. The suite was
> 182 tests; `npm test` was `node test/*.test.js`, which node resolves to the FIRST path and treats
> the other 26 as argv. **It ran 4 of 182 and exited 0 — a green result over a truncated suite.**
> This workflow exists so that cannot recur. […] **The load-bearing step is 'assert the suite is the
> whole suite'. Running the tests is not enough: a green badge over 4 of 182 is the disease, not
> the cure.**"*

Two things follow. First, this **confirms the `node <glob>` mechanism** described in the brief —
and my grep found **zero remaining instances of it**, so the fix pattern propagated. Second, it
states the countermeasure that the paper also reaches for (post-execution verification steps): you
cannot detect a truncated suite by running it; you must **assert the denominator**.

**That assertion pattern — "assert the work happened, not just that the command exited 0" — is the
generalisable finding of this census.** Every instance in §3 and §4 shares one root: *the fleet
verifies that a command was invoked, never that it did the work.* `''` reads as `'0'`; `except`
reads as a verdict; `|| true` reads as success; `set +e` reads as a clean run. The paper's
recommendation ("add verification steps immediately after jobs whose execution is critical") and
this repo's self-diagnosis are the same prescription, arrived at independently.

---

## 7. False positives, and what I got wrong

Stated plainly, because a census that over-claims is worse than a small one.

1. **`pong-quilt/merge-gate.yml:26` — I nearly filed this as the worst instance in the census.**
   `node --test tests/page-parse.test.js` runs **1 of 88 test files**, inside a job named
   `merge-gate`. It looks like the exact §5 disease. It is not: a second job (`suite`, `needs:
   page-parse-pin`) runs the full `node --test tests/*.test.js`. It is a deliberate canary, and the
   header comment explains why. **Verified before reporting.**
2. **`node --test test/*.test.mjs` (unquoted) is NOT a defect.** It appears in ~7 workflows
   (quilt-neighbourhood, quilt-ml-recipes, quilt-nn, quilt-bandit, quilt-lab, pong-quilt). GitHub
   Actions runs `run:` under bash on ubuntu, so the **shell** expands the glob and `node --test`
   receives many files and runs all of them. Only `node <glob>` **without** `--test` has the
   first-file-only behaviour, and there are **zero** of those left. The brief's phrasing — *"unquoted
   runs one file under some shells"* — is right in mechanism but the surviving instances are not
   affected. I excluded all of them.
3. **`platonic-creative-suite`, `edge-ledger`, `quilt-rl` single-file `node --test`** — I checked
   each: `test/` contains exactly one file. Whole suite. Not defects.
4. **~8,355 `except Exception` are not defects.** Nearly all are API handlers returning
   `status=500`, which is correct. My first ranker surfaced 25 of them as "worst"; that was a
   filter failure (`\bfailed\b` matching inside log *strings*), fixed before this report.
5. **`herdr-cocapn` vendored `libghostty-vt`** — third-party code vendored in-tree. Counted but
   not attributed to the fleet.
6. **`.github` in `trees/` vs `src/`** — early results were computed over a partial sweep; all
   numbers here were recomputed over the full 2,776-repo corpus. Where an early and final number
   disagree, the final number is the one reported.

---

## 8. Summary

| Finding | Count | Confidence |
|---|---|---|
| `'' == '0'` / `'' != '0'` coercion on `steps.*.outputs` | **3 fleet-authored** (+2 vendored) | High — mechanism verified against GitHub's own casting docs |
| …in the **fail-open** (`!= '0'`) polarity | **2** | High — this is the under-reported half |
| Uses of the correct `steps.*.outcome` | **0** | High — exhaustive over 2,318 workflows |
| `\|\| true` on a test/install/build command | **161 steps / 135 repos** | High |
| `continue-on-error: true` on release/publish | **2** | High |
| `set +e` fleet-authored | **5** (2 in security scanners) | High |
| Bare workflow badges (no `?event=`/`?branch=`) | **34 of 34** | High |
| `pytest-custom_exit_code` / `suppress_health_check` | **0 / 0** | High (method: content grep, not code search) |
| Programmatic errors recorded as policy verdicts | **12 handlers / 11 repos** | Medium — hand-vetted from 13,025 handlers; #11 and #12 are flagged as likely FPs |
| …of which **proven by execution**, not reading | **1** | High |

**One sentence.** The fleet's dominant shape is not a sophisticated false green — it is
`|| true` on 161 test commands across 131 repos, a decision replicated by a shared CI template; and
beneath it sits a smaller, sharper set where a genuine programming error is formatted as a verdict
(`"blocked"`, `"IDENTITY"`, `REJECT`, `code=0`). Both are the same root cause: **the fleet checks
that a command ran, never that it did the work.** The account already contains both the detector
(§6.4) and the post-mortem (§6.5) for this class; neither is enforced beyond its own repository.
