# RECOVERY-LUCIDDREAMER — the second tarball-only recovery

*2026-10-18. Companion to [FINDING-24.md](FINDING-24.md) §4 and
[NPM-AUDIT.md](NPM-AUDIT.md) §5.*

## The provenance

`@superinstance/luciddreamer@0.1.1` shipped:

```
luciddreamer/          10 modules, 3,464 lines, py.typed
  __init__.py  bathymetry.py  chart.py  cli.py  cocapn.py
  compiler.py   mesh.py       router.py simulators.py  tiles.py  training.py
docs/                  8 design documents, 149 KB
index.js               the Node shim
README.md  CHANGELOG.md  LICENSE
```

Its `repository` field pointed at **`SuperInstance/luciddreamer`** — which
is *luciddreamer.ai*, a marketing site with three files:

```
.wangler/   README.md   index.html
```

**"The substrate's dream home — where it dreams out loud. Brewcast.
Substrate MUD. Plainsong. Tensor-MIDI."**

So the 3,464 lines of Python that the npm package actually ships existed
in **no repository on the account**, and the tarball was the only copy.
npm garbage-collects packages nothing depends on and takes the tarball
with them.

Recovered to **`SuperInstance/luciddreamer-py`**. CI green.

## What writing the test suite turned up

The package shipped **no tests at all**. Writing them surfaced three
defects, each now fixed, each with a regression test *and* a mutation in
`sabotage.py`.

### 1. A save/load round trip silently disarmed the store

`Tile.to_dict()` wrote only the base fields. `Tile.from_dict()` returned
a base `Tile` regardless of the tile's type. So a `CommandTile` came
back as a `Tile` and **lost its `regex_pattern`** — the field compiled
matching is built on.

The export/import path reported the right number of tiles and the right
ids throughout. **The operation was green and the payload was worse.**

*Fixed:* `to_dict()` carries the subclass's own fields; `from_dict()`
dispatches to the class that owns the tile type; tuple fields are
restored as tuples.

### 2. One malformed tile regex killed the entire compile pass

`CompiledCommand` compiles its pattern eagerly, so a bad pattern raised
out of `RigidFinder.try_compile()` and out of `compile_all()`. Because
`compile_all()` iterates the store, **every other tile in the store went
uncompiled because of one bad one.** Measured: three good tiles, one
malformed regex, zero tiles compiled.

*Fixed:* `try_compile()` catches `re.error` and skips that tile.

### 3. `_auto_pattern()`'s two substitutions were dead code

It escaped the text first, then searched the escaped string for a literal
`\d` and a literal `e\s`. Neither is ever there:

```
re.escape("set depth to 12")  ->  'set\\ depth\\ to\\ 12'
  sub \d+ hits  : 0
  sub e\s hits  : 0
```

`re.escape` does not escape digits, and it escapes a space as `\ `, not
`\s`. So every auto-generated pattern was a plain literal match, and the
`n` named group — which the source plainly intended to carry the parsed
number — could never appear in a match result.

*Fixed:* digits become a named group (the first is `n`, later ones are
numbered so two groups cannot collide); a word ending in `e` may
optionally take an `s`.

### And a fourth, found by reading

`TileStore.add()` appended the tile id to the type index unconditionally,
so adding the same tile twice indexed it twice and `find_by_type()`
returned the same tile twice while `len(store)` said 1. A consumer
counting tiles by type double-counted them.

## The suite

```
python -m unittest discover -s tests     76 tests, 0 failures
python sabotage.py                       13 mutations, 13 caught
```

`sabotage.py` is the negative control. It breaks one real behaviour at a
time in a throwaway copy of the source and **requires the suite to go
red for every one**. It is the same contract the fleet's other
harnesses are held to.

One of the thirteen survived the first run — the router's ambiguous band
— and that survivor was not a broken mutation. It was a **gap in the
tests**: nothing exercised a tile below the ambiguous floor, so nothing
pinned the rule that a tile the captain never confirmed must not steer
the boat. The test was written, and the mutation died.

> **A surviving mutant is a finding about the suite, not about the
> mutation.**

## The npm side

`@superinstance/luciddreamer@0.1.2`:

- `repository` repointed from the marketing site to
  `SuperInstance/luciddreamer-py`
- the `__pycache__/*.pyc` that 0.1.1 published are gone
- `RECOVERY.md` ships in the tarball, so a consumer who installs the
  package learns where the source came from
- the fixes are in the published bytes, verified by md5 against the
  working tree

Published **from the tarball, not the directory**, per the rule in
[FINDING-24.md](FINDING-24.md) §8.
