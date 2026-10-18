# CORRECTION — `PRICED-TRAINING.md` is not a phantom, it is on an unmerged branch

*2026-10-18T15:19Z. `PLAN.md` says the ladder is built on *"the house's own
`PRICED-TRAINING.md` ladder."* A measurement pass was sent to confirm that
file **does not exist** and that the real one is `PRICED-STANDARD.md`. **That
is wrong, and the error is the exact one this house has a rule about: the
search looked at `main` and nothing else.** The file exists, at
`situations/arch/PRICED-TRAINING.md`, 17,693 B, on branch
`claude/priced-training`, landed 2026-09-30.*

---

## 1. The claim as the record carries it

> **So the score table has to be BUILT, not generalised.** Which makes
> `PRICED-STANDARD.md` the first measurement of its kind rather than a
> generalisation, and the house's own **`PRICED-TRAINING.md` ladder** the
> thing to build it on. — `PLAN.md:171`

**There is exactly one occurrence of the string `PRICED-TRAINING` in this
repository**, and it is this line. `PLAN.md:47` and `PLAN.md:170` both name
`PRICED-STANDARD.md` correctly, and `PLAN.md:232` names it correctly again.
So the record is *nearly* right and one line wrong.

**Why it was believed:** the file is not on `main`. It is on a branch.
Anyone checking `main` — the way every other path in this record was
checked, correctly, in every other case here — will not find it.

## 2. What was measured

| check | result |
|---|---|
| `GET /repos/SuperInstance/AI-Writings/commits?path=PRICED-TRAINING.md` | **HTTP 200, body `[]`** — *not 404* |
| `GET …/contents/PRICED-TRAINING.md` (root, default branch) | 404 |
| `GET …/contents/situations/arch/PRICED-TRAINING.md` | 404 on `main` |
| `GET …/contents/situations/arch/PRICED-TRAINING.md?ref=claude/priced-training` | **HTTP 200 — 17,693 B, blob `68bcaf3d367acebb6562c3f81d0264221ed68344`** |
| commits on `claude/priced-training` for that path | **`6fc5e5b3325f`, 2026-09-30T20:08:40Z** — *"priced-training: README + situations/arch/PRICED-TRAINING.md (verdict, extr…"* |

**So the file has exactly one commit, ever — and it is on an unmerged
branch that `main` has never had.** The claim "PRICED-TRAINING.md has never
existed" is **false**; the accurate claim is "PRICED-TRAINING.md has never
existed *on main*."

**And the receipt I was given is wrong in a way that would have made it
unfalsifiable.** It said *"The commits API returns **404** for that path."*
The commits API returns **200 with an empty array** — a 200/`[]` is what
that endpoint returns for a path with no commits, and it is what
`GET …/commits?path=PRICED-STANDARD.md` **also** returns at the repo root,
because `PRICED-STANDARD.md` is not at the root either. **A 404 and a
200/`[]` are different answers and the difference is the whole finding.**

## 3. The actual defect — a dangling citation, not a phantom file

Now the real defect, which is better than the one I was sent to correct:

**`PRICED-STANDARD.md` cites `PRICED-TRAINING.md` by name, three times, and
the link is dangling on `main`.**

```
AI-Writings/situations/arch/PRICED-STANDARD.md        (12,449 B, 65dc44794f37d1)
  4: *Architecture mark, 2026-10-18. This answers a question
  5: [PRICED-TRAINING.md](PRICED-TRAINING.md) left open, and it uses the
 11: 1. **a ladder** — `step` → `preds` → `labels`, strongest to weakest
 12:    ([PRICED-TRAINING.md](PRICED-TRAINING.md) §3.1). ✓ exists.
 15: 3. **a pre-registered tolerance** … ([PRICED-TRAINING.md](PRICED-TRAINING.md) §3.2). ✓ exists
```

The relative link resolves to `situations/arch/PRICED-TRAINING.md` — which
is **exactly the path the file actually has**, on the branch it is on. So
the mark is not citing a wrong name at a wrong place; **it is citing a
correct name at a correct place that `main` does not carry.**

**That is a real, load-bearing defect and it is the opposite of the one I
was told to write.** The ladder document says the ladder **"✓ exists"** —
and it does, in the runnable half, which *is* on `main`:

```
AI-Writings/labs/priced-training/priced_training.py      18,405 B  (main)
  """priced_training — can a training STEP be priced like a forward route, or only bounded?"""
AI-Writings/labs/priced-training/prereg.py                2,139 B  (main)
  """the pre-registered constants of the priced-training experiment.
   Committed BEFORE the first experiment ran … sigma is REQUIRED and
   pre-registered — no silent default, no peeking"""
AI-Writings/labs/priced-training/route_witness.py              (main)
AI-Writings/labs/priced-training/nettrain.py                   (main)
```

`PRICED-TRAINING.md` on the branch, §3.1 *"The nouns"* and §3.2 *"Why fsum
works and the rest cannot"*, is the prose half of the same experiment, and
it names `labs/priced-training/` as its runnable half — the link is
**forward from the branch, and backward from `main`**. The two halves were
split across a branch boundary on 2026-09-30 and 2026-10-18 respectively.

**So: the ladder is real, the code is on `main`, the prose is on a branch,
and `PRICED-STANDARD.md` on `main` cites the branch's file by a path that
is correct in the repo and unreachable from the branch it is read on.**

| | on `main` | on `claude/priced-training` |
|---|---|---|
| `situations/arch/PRICED-STANDARD.md` (12,449 B) | ✓ `9705f57f661c`, 2026-10-18T07:46:47Z | ✓ |
| `situations/arch/PRICED-TRAINING.md` (17,693 B) | — | ✓ `6fc5e5b3325f`, 2026-09-30T20:08:40Z |
| `labs/priced-training/*.py` (4 files) | ✓ | ✓ |

## 4. The second claim in the same pass — `parse_address` — is ALREADY CORRECT

The pass also reported *"`parse_address` is in `openjev/cell.py`."* **That
claim is not in this record, and it was never wrong here.**

- `cell-history/CELL-HISTORY.md:43` (row 10b) already says
  `.../code/spreadsheet/spreadsheet_plugin.py` — *"The cell↔spreadsheet
  bridge: `address` = `Sheet1!C3` regex, `parse_address`."* **Correct.**
- `grep -rn "parse_address"` over this repository returns **exactly one
  line**, and it is the correct one.
- Measured, 2026-10-18: `openjev/cell.py` (6,861 B, `dbdda303d456`) has
  **zero** occurrences of `parse_address`; `spreadsheet_plugin.py` (7,428 B,
  `139059f5ca5c`) defines it at **`:23`**
  (`def parse_address(address: str) -> Tuple[Optional[str], int, int]`)
  and calls it at `:48`.
- `intercontext-map` had already corrected this too —
  `CLAIMS.md:60`: *"`parse_address` in `openjev/cell.py`" → "it is in
  `code/spreadsheet/spreadsheet_plugin.py:23`"*, and `MAP.md:72` separates
  them correctly: *"`cell.py:34` — while its *address* is `Sheet1!C3`."*

**Recorded here because the house's rule is that a correction with no
target is still worth filing: the next reader who goes looking for this
defect should be told it was already fixed, not re-open it.** The two
statements that are true together — `cell.py` has the **identity**,
`spreadsheet_plugin.py` has the **address** — are the whole of it, and both
records already say so.

## 5. The two ladder facts — one was already in the record

**`si-ready` holds exactly ONE repository, an org profile README.**
**CONFIRMED, and already stated, three times:**

- `ATLAS-03.md:16` — `| **ready-to-use** | **\`si-ready\`** | 1 | 2026-10-03 | the org profile only — **the shelf is empty** |`
- `ATLAS-03.md:93` — *"**`si-ready` holds one repository: the org profile.**"*
- `ATLAS-03.md:104` — *"the ready-to-use rung holds nothing."*

Measured: `GET /orgs/si-ready` → `type: Organization`, `public_repos: 1`;
`GET /orgs/si-ready/repos` → 1 result, `si-ready/.github`, size **2 bytes**,
default branch `master`, description *"SI Ready org profile: the curated
shelf for ready-to-use app…"*. **Two bytes is the org-profile README and
nothing else. There is no code on the top rung of the ladder.**

**`SuperInstance` is a `User` account, not an Organization — ALREADY IN THE
RECORD, in four places, and stated as a correction in its own right:**

- `ATLAS-01.md:55` — *"**`SuperInstance` is a `User`, not an `Organization`.**"*
- `ATLAS-03.md:13` — `| **sketchbook** | \`SuperInstance\` (a **user** account) | … |`
- `FINDING-24.md:73` — *"## 2. `SuperInstance` is a **user** account, not an organization"*, and `:77` — *"called the 5,284-repository fleet 'the SuperInstance organization.'"*
- `intercontext-map` models it as 8 `organization` nodes with the boundary
  crossings drawn (`MAP.md:41`, crimson-red weight 4).

Measured: `GET /users/SuperInstance` → `"type": "User"`,
`"public_repos": 5290`, `"name": "Casey Digennaro"`.

**So both ladder facts in my brief were already true in the record, and one
of them had already been the subject of its own correction.** I am filing
the measurement anyway, with the receipt and the date, because a fact with
no receipt in a live record is a fact that will be re-measured.

**The count is the only thing that moved:** `5,284` (`ATLAS-01.md:5`,
`ATLAS-03.md:13`, `:103`, `FINDING-24.md:52,65,77,104`) vs `5,289`
(`ATLAS-01.md:23,:57`, `cell-history/CELL-HISTORY.md:6`,
`ATLAS-ADDENDUM-01.md:236`, `PLAN.md:162`) vs **`5,290` measured now**.
`CRITIQUE-RESPONSE.md:179` already handled this correctly — *"The counter
caught up. The general claim was right and this is its prediction coming
true"* — and `ATLAS-01.md:36-40` shows the 5,284 → 5,289 reconciliation
with the delta arithmetic. `ATLAS-03.md`'s ladder table is the one place
still carrying the older figure; annotated in place rather than renumbered,
because **this number changes about eight times a day and chasing it into
the text is how the text stops being true.**

## 6. The rules this correction adds

| # | rule |
|---|---|
| 112 | **"Does not exist" means "does not exist on the branch you looked at," or it is not a claim.** `PRICED-TRAINING.md` exists, on `claude/priced-training`, one commit, 17,693 B. Every path check in this record has been on `main`, correctly — but a repository with **75 branches** is not a repository with one tree. |
| 113 | **`GET /commits?path=` returns 200 with `[]`, not 404, and the difference is the finding.** A 404 means the endpoint is wrong. A 200/`[]` means "no commits at this path on the default branch" — which is a statement about *history on one ref*, not about existence. |
| 114 | **A dangling citation is not a phantom file.** `PRICED-STANDARD.md` cites `PRICED-TRAINING.md` by a correct name at a correct path. What is missing is the merge, not the document. "This file never existed" would have been a much worse finding, and it would have been wrong. |
| 115 | **A correction with no target is still worth filing.** "`parse_address` is in `cell.py`" was already false-and-corrected in two records before this pass. Filing it stops the next reader re-opening a closed defect. |
| 116 | **A count that moves eight times a day gets a date, not a new number.** `ATLAS-03.md`'s sketchbook rung is annotated rather than renumbered. The claim worth keeping is *"growing ~8–12/day"*, and that one is still true. |
