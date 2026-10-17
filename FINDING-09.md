# gitpp-c promotes an absent or null `kind` to `task` — an override, not a mis-rank

Found by running the 38-case v0.4.6 corpus against real ports rather
than against the in-repo readers. Three of 38 failed; two were
outstanding fixes on unmerged branches, and the third was new.

## The defect

`gitpp.c:450`:

```c
r.kind = (char *)obj_get_str(obj, "kind");
if (!r.kind) r.kind = "task";
```

`obj_get_str` returns `NULL` for anything that is not a JSON string. So
`kind: null`, `kind: ""`, **and an omitted `kind`** all arrive as `NULL`
and are rewritten to `"task"` — the **middle** class, not the lowest.

## Why that is an override

```json
[{"id":"a","kind":"witness","parents":[],"effects":{"x":"W"},"reason":null},
 {"id":"b","parents":["a"],"effects":{"x":"OMITTED"},"reason":null}]
```

| port | state | |
|---|---|---|
| **`gitpp-c`** | **`{"x":"OMITTED"}`** | **no** |
| `gitpp-js` / `cs` / `pl` | `{"x":"W"}` | yes |
| reader1-py / reader3-c / reader4-pl | `{"x":"W"}` | yes |

A record with **no `kind` field at all** displaces a `witness`. The
spec's rule is the opposite: an unclassifiable record "never displaces a
classifiable one."

`kind: "banana"` is handled correctly, because it is a real string and
`class_rank` returns 0 for it. Only the `NULL` path is wrong — which is
why the function looks correct on inspection and the bug is one line
upstream of it.

## Why no corpus case saw it

Every conformance case written before v0.4.6 used a valid `kind`. The
hole is specifically *absent* and *JSON-null* kind, which the spec only
made legal-and-specified in v0.4.6. Filed as
[gitpp-c#2](https://github.com/SuperInstance/gitpp-c/issues/2).

## The other three cases in this run

Running the merged 38-case corpus with real ports, the remaining
failures were **outstanding fixes on unmerged branches**, not defects:

| case | cause |
|---|---|
| `d33` | `gitpp-js` tie-break fix is on `fix/topo-order-tiebreak`, not `main` |
| `d34` | `gitpp-cs` null-kind fix is on `fix/null-kind-crash`, not `main` |
| `d38` | `gitpp-c` cycle-orphan defect — **the new finding** |

## A merge problem this surfaced

`gitpp-cs` now has **two independent branches that each fix a different
defect, and neither contains the other**:

- `fix/null-kind-crash` — `GetValueOrDefault(null)` throws
- `fix/cycle-descendant-orphans` — cycle descendants not orphaned

They **conflict on `src/Gitpp.cs` and `test/Program.cs`** (add/add), so
neither can merge cleanly into the other. I combined them by hand to
check they are semantically orthogonal — one changes the class
comparison, the other the orphan computation — and the combined source
builds clean and passes `d34`.

Worth noting that the semantic check was necessary: a textual merge
resolves the add/add conflicts, but only running the corpus shows the
two fixes do not contradict each other.
