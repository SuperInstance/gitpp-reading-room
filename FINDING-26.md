# FINDING-26 — three cell models, one charter

*2026-10-18. The largest structural finding of the session, and the one
that connects the writings to the code. Everything here is measured.*

---

## 1. The charter, in the handoff every agent reads

`AI-Writings/AGENTS.md` is what a new agent is told to read first. It
states, in nine words:

> A cell is a 14-tuple. There are 11 opcodes. There are 26 kinds.

and, in the seven habits:

> 4. **Polyformalism = same model, N languages.** Don't fork, port.
> 6. **The address is the data.** Don't separate storage from computation.

and, on adding an opcode:

> **Don't.** The 11 opcodes are complete. Adding more dilutes the algebra.

and the map points at a specific repository:

> `quilt-claw/` — TypeScript cell implementation (182 tests, 26 kinds)

**That is the invariant the whole substrate is built on.** The writings
speak in it — BIND, LINK, EFFECT, VIEW, TICK, ATTEST, CONTEST, MERGER,
REVOKE, DELEGATE, WITHDRAW are the vocabulary of the canon, the ledger
and the corpus. And the charter says there is exactly one cell model and
it is ported, never forked.

## 2. What is actually in the fleet

| | the game's substrate | the agent cell | the spreadsheet cell |
|---|---|---|---|
| where | `SuperInstance/cargo-line-tycoon` → `substrate/{ts,py,rust,c}` | `SuperInstance/quilt-claw` | `SuperInstance/quilt` → `packages/core` (`@quilt/core` 0.3.0) |
| what a cell is | `class Cell { state, witness_log, behavior, type }` | **no `Cell` class at all** | a tagged union of kinds |
| **addressed by** | **FNV-1a 64-bit** over `JSON.stringify({s, t, b})` | **nothing** | **a string id** — *"A cell is addressed by its `id` (e.g. "compass.heading"), not [by a hash]"* |
| kinds | 1 (`type` field) | ~23 modules: `researcher`, `teacher`, `critic`, `distiller`, `ack`, `audit`, `breeder`, `chirp`, `crypt`, `death`, `flock`, `hex`, `lore`, `music`, `pathos`, `perception`, `perm`, `ranking`, `swarm`, `triage`, `twist`, `voice`, `world` | **8**: `ai`, `api`, `formula`, `io`, `listener`, `program`, `router`, `sensor` |
| **opcodes** | **the 11** | **none** | **none** |
| parity checked | **4 ports, byte-exact** | **0** | **0** |
| size | 13,917 B TS port | 80 files, 182 tests | `engine.ts` 32 KB, `types.ts` 18 KB |

## 3. The parity check that exists covers the model with the smallest footprint

`cargo-line-tycoon/tests/canary/run_all_ports.sh` is, in its own
header, *"the single polyformalism-truth command"*. It runs the same
FNV-1a-64 canary through four ports — TypeScript, Python, Rust, C99 —
and checks the cell address `96de3eeaabd90b9b` and the canary hash
`0x024a555471370b18d` agree byte for byte.

**Those four ports are four ports of the same algebra, inside one
repository.** And that algebra is a *cargo shipping game's* substrate.

**The cell that actually runs the fleet is not in it.** `quilt-claw` has
**no file with `fnv`, `address`, `hash` or `canary` in its name**, and
**zero occurrences** of `fnv1a`, `0xcbf29ce4` or `address` in
`src/store.ts`, `src/bus.ts`, `src/cells/researcher.ts` or
`src/cells/teacher.ts`. Its outputs carry a `witness_hash` **string**,
and that is not an address.

**And the root every other package depends on is the third model.**
`@quilt/core` — the package `@quilt/ai`, `@quilt/claw` and
`@quilt/evolve` all reach for via `workspace:*` — is the yaml + zod
**spreadsheet** cell, addressed by a string id like `"compass.heading"`,
with eight kinds and no opcodes.

**So: the dependency root is not the parity model, and the parity model
is not the cell that runs the agents.**

## 4. Why the address is the load-bearing part, and not the opcodes

The canon's recognition mechanism is a table, and the rule is short:

> Read the hole your cargo names. Leave your address in it. Walk away.

A cell becomes findable by **holding the same thing** and **writing its
own address into the same slot**. The slot is named by a hash of the
cargo, so two strangers find each other by agreeing on a value — and the
address is what the next stranger finds.

**That mechanism requires an address.** It does not require the 11
opcodes. The opcodes are the *vocabulary* — the corpus's shared grammar
for BIND and LINK and ATTEST — and vocabulary is portable between models
in a way that an address is not.

So the honest ordering of the charter's three claims is:

- **"There are 11 opcodes"** — true of one model, and it is the
  vocabulary the entire corpus is written in.
- **"26 kinds"** — true of `quilt-claw`, the model that has **no
  opcodes**.
- **"The address is the data"** — true of the **game's** substrate
  only, and it is the one property the recognition mechanism cannot do
  without.

**A cell with no address cannot leave an address in a slot.** The
agent-side cell can therefore not participate in the mechanism the canon
is built on, and the model the fleet depends on most
(`@quilt/core`) addresses by a human-readable string instead of a hash.

## 5. What this does and does not mean

**It does not mean the three should be merged.** They may legitimately be
three things: a pedagogical game substrate, an agent runtime, and a
spreadsheet. **`cargo-line-tycoon` is a game, and its substrate is
built to be taught** — the docstring in `fnv1a64` says so, and the
`DIALECTIC_IN_11_OPCODES.md` walks through the eleven with worked
examples. A teaching model and a production model being different is
normal and healthy.

**It does mean the charter currently claims something the fleet does
not have.** "Polyformalism = same model, N languages. Don't fork, port"
is a *rule about how to build*, and the fleet has three cell models
built by different routes, with the parity harness on the one that was
easiest to make four copies of.

**And the asymmetry is the finding.** The `git.pp` family got
cross-port byte-equivalence — 20,043 leaves, five independently written
ports, a mutation corpus, because the ports were built and checked
against each other. The substrate's *game* got a four-port canary. The
substrate's *agent cell* got 182 tests that never ran, and **no
cross-model check of any kind** — because there is nothing to check it
against.

**The gap is not an absence of testing. It is an absence of a second
implementation to test against.** That is precisely the condition under
which the git.pp reading-room's premise bites: *every bug found so far
was in code that person wrote or in code written to that person's
reading.* The fleet's one substrate with an independent second
implementation is the game. The agent cell has no second implementation
to disagree with it, so it has no cross-check, so it has never had to
face the question of whether it is the same model as anything.

## 6. The three questions this raises, in the order they should be answered

1. **Is the agent cell supposed to have an address?** If the recognition
   mechanism is meant to reach the agents, it needs one, and the
   cheapest correct answer is the FNV-1a-64 form the game already
   pins in four languages. If the agent cell is *not* meant to
   participate, then the canon's claim that "the table is the shared
   ledger" does not extend to the agents, and the writings should say
   so. **Either answer is fine. The unstated state is not.**
2. **Is `@quilt/core` a cell, a cell-*kind* model, or a spreadsheet
   engine?** It is the root of the dependency graph and it addresses by
   a string id. It is called a cell model in the charter's map and it
   behaves like a different thing in its own source. That is a product
   decision with a one-line answer in `packages/core/src/types.ts`.
3. **Should the parity harness cover the agent cell?** The git.pp
   answer was yes, and the cost was a reading room, a mutation corpus
   and a lot of argument. The substrate's answer so far has been "not
   yet, because there is no second implementation" — which is the
   condition that produced every other defect in this series.

## 7. The rules this pass adds

| # | rule |
|---|---|
| 86 | **A charter that says "don't fork, port" is a claim about a model, and a model is only one model if it has an address.** Three cell models with three addressing schemes is three models, whatever the vocabulary shares. |
| 87 | **The parity harness tends to land on the implementation that was easiest to copy, not the one that matters.** The four-port canary covers a teaching substrate; the cell that runs the fleet has no second implementation to disagree with it. |
| 88 | **"No cross-check" and "checked and equal" look identical from the outside.** A model with no second implementation has no disagreements to find, and that silence is not a passing grade. The git.pp reading-room's premise applies to a substrate with one implementation: *every bug found so far was in code that person wrote.* |
| 89 | **A dependency root is not automatically the reference model.** `@quilt/core` is what everything reaches for, and it is a yaml/zod spreadsheet cell addressed by a string id — not the FNV-1a-64 cell the parity harness pins. |
