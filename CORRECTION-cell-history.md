# CORRECTION — FINDING-26's opcode and address claims, against the measured history

*2026-10-18. `snowball-scout` traced the three cell models back through
git. Two of my claims in [FINDING-26](FINDING-26.md) were wrong, and the
correction is bigger than the finding was.*

---

## 1. "There are 11 opcodes" — there are two elevens, and the corpus asserts both

I quoted the game's `docs/DIALECTIC_IN_11_OPCODES.md` as the charter's
eleven. **It is not the charter's eleven.**

`algebra.md` — the charter's own formal reference, which has **exactly one
commit, ever** — lists:

> BIND LINK EFFECT VIEW TICK **FORGET PROOF ROUTE CRDT WORLD TIME**

The game's dialectic doc lists:

> BIND LINK EFFECT VIEW TICK **ATTEST CONTEST MERGER REVOKE DELEGATE WITHDRAW**

**Two different elevens, seven hours apart.** `algebra.md` landed
2026-09-21T14:46:45Z; `wr-obs13-missing-opcodes.md` landed
2026-09-21T22:01:18Z, replaced six opcodes, and ended with *"Run the
numbers: eleven plus four is fifteen."* `algebra.md` was **never
revised**, so the corpus asserts both simultaneously. The game
implements **ATTEST** — six B's sixth member — which the charter's own
algebra.md does not list.

**And the implementation is thinner than any of the two elevens:**

| port | path | opcodes implemented |
|---|---|---|
| TypeScript | `substrate/ts/src/index.js` | **6** — BIND LINK EFFECT VIEW TICK ATTEST; `default:` throws `Unknown opcode` |
| C99 | `substrate/c/cell_algebra.{c,h}` | **5** — BIND LINK EFFECT VIEW TICK |
| Python | `substrate/py/clt_substrate/__init__.py` | **1** — `TICK = 'tick'` |
| Rust | `substrate/rust/src/lib.rs` | **0** — no opcode identifiers at all |

`CONTEST`, `MERGER`, `REVOKE`, `DELEGATE` and `WITHDRAW` are **never
implemented in any port** — they appear in one prose doc, marked
*"6 proposed (R11 derivative)"*. A fleet-wide code search for
`ATTEST CONTEST MERGER` returns **0 files**; for `ATTEST CONTEST`, 13
files, **none of them in the three repos**.

## 2. "The address is the data" — but the canon's mechanism is a FOURTH scheme

FINDING-26 said the game's FNV-1a-64 substrate is the model with the
address, and that the agent cell therefore cannot participate in the
canon's recognition mechanism. **The first half is right. The second
half names the wrong mechanism.**

The recognition mechanism in the canon — *"read the hole your cargo
names; leave your address in it"* — is **`qthe`'s wormhole table**, and
`qthe` addresses by **slot + resonance, with no hash at all**. So:

| | addressing |
|---|---|
| the game's substrate | **FNV-1a 64-bit** over `JSON.stringify({s,t,b})` |
| `qthe`'s lattice cell | **slot + resonance**, no hash |
| the agent cell | nothing (a `witness_hash` **sha256 over content**, not an identity) |
| the openjev ancestor cell | **a spreadsheet coordinate** — `Sheet1!C3` |
| `@quilt/core` | a string id — `"compass.heading"` |

**Five addressing schemes.** My claim that the agent cell "cannot
participate in the mechanism" is right in substance and wrong in
mechanism — it is not that the agent cell lacks *the game's* address, it
is that the mechanism it should be able to join does not use a hash at
all, and the agent cell has neither a hash nor a slot.

## 3. "26 kinds" is an off-by-one, twice over

| source | count |
|---|---|
| `quilt-claw` `src/cells-*/` directories | **25** |
| `quilt-claw-cells-game` `CELLS` array | **25**, labelled "26" |
| the live validator `KIND_WHITELIST` | **24** |
| `@quilt/core` `CellKind` | **9** |

The 24-vs-25 gap is exactly **`perm`** — in the zoo and in `quilt-claw`,
dropped from the validator whitelist at some point between.

## 4. The order was the reverse of what I assumed

FINDING-26 implied the address came with the model. **The kinds came
first, by four days.**

```
2026-09-17  quilt-claw: 25 kind modules land, no address, ever
2026-09-19  zoo: 25 cards labelled 26
            _worker.js: KIND_WHITELIST(24) + OP_WHITELIST(11)   <-- THE WELD
2026-09-20  lab/brew: the eleven is GROWN, not designed
2026-09-21  algebra.md + AGENTS.md = THE CHARTER
            wr-obs13: six B                                   <-- the canon contradicts itself
2026-09-27  cargo-line-tycoon: 6 opcodes RUN, 11 DOCUMENTED, FNV-1a ADDRESS
2026-09-27  qthe: lattice cell, slot+resonance address, no hash
2026-10-18  quilt-claw finally sees fnv1a -- as a LEDGER, ported from
            AI-Writings, on an UNMERGED branch
```

**And there was never an agent cell with an address.** `git log -S` across
all 26 commits and 5 refs of `quilt-claw`: `-S 'fnv'` → **1 hit**
(2026-10-18, an unmerged branch), `-S 'computeAddress'` → **0**,
`-S 'cbf29ce4'` → **0**. The one fnv that exists is in
`witness/ledger.js` — **a hash-chained receipt ledger over the test
suite**, ported from `AI-Writings/labs/situation-recorder/recorder.py`,
**19 days after the same idiom had already been the game substrate's
cell address.** The address algebra reached the agent cell from the
witness-log idiom. **The agent cell never hashed a cell.**

## 5. What this does to FINDING-26

FINDING-26's structural claim survives and sharpens: **the charter
describes a cell model that never existed.** It welds an agent-cell kind
roster (25, mislabelled 26) to a canon opcode set (11, of which at most
6 are implemented anywhere) and to an addressing scheme (FNV-1a-64) that
belongs to a *different repository* and that the canon's own mechanism
does not use.

**And the three models met — once, and then diverged.** `AI-Writings/_worker.js`
(2026-09-19T21:49:03Z) puts `KIND_WHITELIST` and `OP_WHITELIST` as
sibling constants and validates the canon 14-tuple against them. **That
is the weld** — and it happened 41 hours *before* the charter that later
describes both.

**The honest one-liner, from the scout:**

> the three repos are three *portraits* of a model that was written down
> in `AI-Writings` on 2026-09-21 and has been drifting ever since — the
> opcodes mutate, the kind count is off by one, the address arrives in the
> game and never reaches the agent, and the "recognition" mechanism in
> the canon is actually `qthe`'s wormhole table wearing a different hat.

## 6. The rule this correction adds

| # | rule |
|---|---|
| 90 | **A number in a charter is a claim about a count, and a count drifts.** "26 kinds" is 25 in the repo, 25 in the zoo, 24 in the validator. "11 opcodes" is two different elevens, seven hours apart, one of them never revised. |
| 91 | **Before quoting a doc as the spec, check the doc and the spec are the same file.** `algebra.md` and the game's dialectic note are different elevens. I quoted the game for the charter. |
| 92 | **An implementation count is not a documentation count.** 6 of 11 implemented, 11 documented, 0 in Rust. "There are 11 opcodes" is true of a doc and false of every port. |
| 93 | **When two things met once and then diverged, the meeting is the artifact.** `_worker.js` welded a 24-kind roster to an 11-opcode whitelist in one file, 41 hours before the charter described both. The models are three portraits of a thing that was written once. |
