# gitpp-reading-room

> Independent readings of the git.pp spec, with **disagreement as the output** —
> and, since 2026-09, the map of the sketchbook the readings came from.

This repository is where the fleet's *evidence* lives. Not the code — the
code is in the `gitpp-*` repos and the 5,000-odd sketches. This is the
record of what was measured, what was found, and **what was wrong in the
findings themselves**.

---

## Why it exists

Conformance testing answers *"do these implementations agree?"* It does not
answer *"is the spec saying anything?"*

Five fold ports agree on 20,043 records. That is a real result and it is
bounded. It says the spec is *self-consistent* and that five implementations
can hit it. It does not say the spec is *right*, because the spec, the corpus,
the reference and the test runner were written by the same person. Every bug
found so far was in code that person wrote or in code written to that person's
reading.

The one genuinely outside judgment in the project was a sixth fold, written by
someone else, which **disagreed** with the spec's reading of §5.1–§5.2. That
disagreement was the most valuable thing that happened all week.

That is the method this room follows: **find something, write it down, then
try to break your own finding.** Four of the corrections in here were written
by the agent whose own work they correct.

**And on 2026-10-18 the method was run against the corrections themselves.**
Six claims in this room were re-measured; six new correction files were
filed, and **two of the six turned out to be wrong in the opposite
direction to what the brief expected, and one was not false at all.** The
brief, the receipts, and the correcting map were all re-measured too — see
[the re-measurement table](#the-2026-10-18-re-measurement--six-claims-that-were-false)
and [CORRECTION-intercontext-map](CORRECTION-intercontext-map.md).

---

## Start here

| if you want | read |
|---|---|
| **the state of everything, and what is left** | **[PLAN.md](PLAN.md)** — the inventory, the critical path, the refined order |
| the sketchbook, mapped | [ATLAS-01.md](ATLAS-01.md) |
| where the gold actually is | [ATLAS-02.md](ATLAS-02.md) |
| the ladder: sketchbook -> production -> hardened -> shelf | [ATLAS-03.md](ATLAS-03.md) |
| the substrate's own shape, and where it drifted | [FINDING-26.md](FINDING-26.md) |
| two ports of one idea, and no equivalence check | [FINDING-25.md](FINDING-25.md) |
| the npm layer, and the publish trap | [FINDING-24.md](FINDING-24.md) |

---

## The corrections

Kept in the room on purpose. **A finding left visible and labelled is worth
more than a finding quietly repaired** — the error is the receipt that the
check works.

| | what it corrects |
|---|---|
| [CORRECTION-canon-shape.md](CORRECTION-canon-shape.md) | I claimed the Live Canon service changed its response shape. **It never did.** The client was written against a shape the service never had — four days before the service existed. |
| [CORRECTION-cell-history.md](CORRECTION-cell-history.md) | "There are 11 opcodes" — ~~there are **two** elevens seven hours apart, and 6/5/1/0 are implemented~~ **[re-measured 2026-10-18: THREE elevens, and 9/6/5/4/1/0 — see the four corrections below]** . "26 kinds" is 25/25/24. |
| [CORRECTION-address-vs-id.md](CORRECTION-address-vs-id.md) | I had the **ID and the ADDRESS the wrong way round.** The hash is the cell's ID; the address is a 4D place — `(x, y, t, sig)`, "4D navigation without a map." |
| [CORRECTION-quilt-namespace.md](CORRECTION-quilt-namespace.md) | The name "quilt" is held **three times over**, and the package I thought was ours is a third party's. |
| [CORRECTION-third-party-packages.md](CORRECTION-third-party-packages.md) | 36 of the published packages are **other people's**, with correct provenance — and `motifcode` is `taewooopark`'s. |
| [CORRECTION-signed-zero.md](CORRECTION-signed-zero.md) | An earlier one: a defect I reported **was not a defect.** |

### The 2026-10-18 re-measurement — six claims that were false

A measurement pass over the substrate, the canon and the ladder sent to this
room found six claims that did not survive re-measurement, plus three facts
the record did not know. **Every correction below carries its receipt and
its date, and every false claim is annotated in place rather than deleted.**

**Three of the six did not come out the way the brief expected, and saying so
is the point:**

| | verdict |
|---|---|
| "Rust is 0" | **confirmed, wrong receipt** — the receipt named a repository that does not exist; the file is `quilt-rust/crates/quilt-polyformalism/src/lib.rs` |
| "the five lifecycle opcodes are implemented" | **half false** — `CONTEST` and `WITHDRAW` run; `MERGER`, `REVOKE`, `DELEGATE` do not. Writing "implemented" for all five would have *overstated* the fleet by three opcodes |
| "`PRICED-TRAINING.md` never existed" | **false** — it exists, on an unmerged branch. The receipt also cited the wrong HTTP status |
| "`algebra.md` has one commit" | **confirmed, exactly** — including the reason it stopped being true |
| "`parse_address` is in `openjev/cell.py`" | **never claimed in this record** — the record already had it right; the correction records that, so it is not re-opened |
| "there are five cell models" | **the record never said five** — it said three. Eight is the measured count |

| | what it corrects |
|---|---|
| [CORRECTION-rust-is-nine.md](CORRECTION-rust-is-nine.md) | **"Rust implements 0 of the 11 opcodes" is true of ONE FILE and false of the language.** `quilt-rust/crates/quilt-polyformalism/src/lib.rs` implements **9 of 11** — `COUNT = 9` at `:85`, `ALL_OPS: [Op; 9]` at `:195`, `struct Engine` at `:131`, six `law_` fns. **This also corrects `ATLAS-02`'s "independently implemented in four languages" — the canary parity is real, the algebra parity (6/5/1/0) is not.** |
| [CORRECTION-the-elevens-remeasured.md](CORRECTION-the-elevens-remeasured.md) | **"CONTEST/MERGER/REVOKE/DELEGATE/WITHDRAW are never implemented anywhere" is 2/5 wrong** — `CONTEST` at `browser/substrate-sdk.js:242` and `WITHDRAW` at `:251` both run; `MERGER`, `REVOKE`, `DELEGATE` are still unimplemented. **There are THREE elevens, not two** — the third is `massive_simulation.js:16` with `MERGER`, and it is *executable*. **There are FOUR witness dialects, not three**, and a fifth node that is a vocabulary, not a dialect. |
| [CORRECTION-eight-cell-models.md](CORRECTION-eight-cell-models.md) | **"Three cell models" is eight.** `intercontext-map` measured `cell_model: 8` over 48 nodes / 70 edges. **Three of the five missing were already in this room, filed under other headings** — which is why the count was never wrong so much as un-tabulated. |
| [CORRECTION-phantom-ladder.md](CORRECTION-phantom-ladder.md) | **`PRICED-TRAINING.md` is not a phantom — it is on an unmerged branch**, `claude/priced-training`, 17,693 B, `6fc5e5b3325f`. The real defect is a **dangling citation**, not a missing file. Plus: the commits API returns **200 `[]`, not 404**; `parse_address` was **already** correctly attributed; and the two ladder facts (`SuperInstance` is a `User`; `si-ready` holds 1 repo) were **already in this record**. |
| [CORRECTION-algebra-second-commit.md](CORRECTION-algebra-second-commit.md) | **"`algebra.md` has one commit, ever" — it had one for 27 days and has had two for seven hours.** `1da6180bb7`, 2026-10-18T07:46:49Z, added `result?: any`. **The only correction here that time falsified rather than measurement.** `algebra.md:76` still lists eleven A, so the drift finding survives. |
| [CORRECTION-intercontext-map.md](CORRECTION-intercontext-map.md) | **The map I was corrected *from* has four errors of its own.** `intercontext-map/CLAIMS.md` supplied the receipts; on re-measurement `MERGER`/`REVOKE`/`DELEGATE` are absent from the file it cites, `PRICED-TRAINING.md` exists, "four dialects" disagrees with its own `node_kinds: 5`, and "zero names in common with the charter's eleven" is wrong by exactly one word — `ROUTE`. |

---

## The method, in the house's terms

- **Measure, don't judge.** If it can be run, run it. If it cannot, say so
  and name where you looked.
- **A claim about a score has to be checkable.** Every number here carries
  the command or the file that produced it.
- **The broken artifact is not the witness.** When a client disagrees with a
  service, the client is under suspicion until history says otherwise — and
  history is a commit log, not a vibe.
- **A number in a charter is a claim about a count, and counts drift.**
- **A witness that records only the request witnesses an intent, not an
  outcome** — and an intent is not the substrate of truth for anything.
- **Confidence is a minimum, never a mean**, and a weight never enters the
  confidence number.
- **Green over nothing is the signature defect.** A badge is not a result
  until the thing it claims has been asserted.
- **A search scoped to a directory is not a search of a repository.**
  `substrate-sdk.js` lives in `browser/`; grepping `substrate/` reported two
  opcodes as "never implemented anywhere" while they ran.
- **A repository with 75 branches is not a repository with one tree.**
  "Does not exist" has to mean "does not exist on the branch you looked at."
- **Three receipts do not generalise to five findings** — and a corrective
  document needs the same independent verification as a finding.
- **A claim about a moving subject carries the date it stops being true.**
  `algebra.md` had one commit for 27 days and two for one.

---

## What is here

```
ATLAS-*.md            the map
FINDING-*.md           what was found
CORRECTION-*.md        what was found WRONG, and by whom
                       (six of them, 2026-10-18, are a re-measurement of
                       claims this room had already filed as true)
adr/                   decisions, options, and the graph
chronicle/             where things came from
```

**Known dangling pointer, found 2026-10-18 and NOT fixed here.** Five files
— `FOUR-HARNESSES.md:87`, `SESSION-SUMMARY.md:76`, `FINAL-STATE.md:61`,
`TIEBREAK-FINDING.md:87`, `NEGATIVE-CONTROLS.md:105` — say *"CORRECTED — see
`CORRECTION-five-ports.md`"*, and **that file does not exist and never has.**
The substantive correction is not lost (each of the five states it inline:
*`gitpp-doctor` is the clone* directory *name, not a repository; the five ports
are c, `git.pp-doctor`, js, cs, pl*), but the pointer dangles.

**It is left dangling on purpose.** Writing the file would mean asserting a
correction about the five `git.pp` ports that this pass did **not measure** —
and a correction file written from someone else's summary is exactly how
`intercontext-map/CLAIMS.md` ended up with four bad receipts (see
[CORRECTION-intercontext-map](CORRECTION-intercontext-map.md)). **The next
pass should measure the five ports and then write it.** Filed as
incompleteness, which the house allows, rather than as a claim, which it does
not.
