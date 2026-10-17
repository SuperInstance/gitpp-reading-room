# A bug that deletes a conflict from the fold — not a hash bug

Every defect so far has been a **hash** bug: a port computes the wrong
leaf. `gitpp-c`'s `values_equal()` was different, and it is the most
interesting thing found in this session.

## The bug

`gitpp-c`, `gitpp.c`:

```c
int values_equal(...) {
    char ba[4096], bb[4096];
    /* build each side into a FIXED buffer */
    ...
    return strcmp(ba, bb) == 0;
}
```

Two **different** values that share a 4095-byte prefix compare
**EQUAL**. §5.3 uses that comparison to decide whether a key has a
genuine disagreement. So the fold concluded "no conflict" and **deleted
a real conflict entry from the ledger**:

```jsonc
// override-1 -> k = "AAAA…X", task-b -> k = "AAAA…Y"   (4201-byte shared prefix)
pre-fix : {"conflicts":[]}
ref     : {"conflicts":[{"key":"k","winner":"override-1","losers":["task-b"]}]}
```

**The ledger lost an entry, not a hash.**

## Why this is a different class

Every previous finding in this project is caught by comparing leaves or
fold outputs *between* ports. A hash bug changes the bytes, so a
cross-port comparison sees it.

**This one does not change a byte.** The state is right. The order is
right. The orphans are right. Only the *conflicts* list is short — and
only for values long enough to share a 4095-byte prefix.

A cross-port check that compares `state`, `order` and `orphans` but not
`conflicts` would never see it. And a conflict ledger is exactly the
thing a consumer reads to ask "what did I lose, and why?"

## The same bug, second site

`record_leaf` hashed into `char buf[8192]`. The truncation point is
**8191, not 8192** — `fmemopen(buf, 8192, "w")` reserves the last byte
for its own NUL terminator, so the port's real capacity was one byte
below what the code believed, and nothing said so.

Proven against two different sizes:

```
pre-fix C leaf            45c72d357e7192a5caca7fb7ee2d1ec81fa388ff808e37a33379c12e96ea4c59
sha256(canon[0:8191])     45c72d357e7192a5caca7fb7ee2d1ec81fa388ff808e37a33379c12e96ea4c59  <-- EXACT
sha256(canon[0:8192])     b3a88506e38d2a83e29e1cba63279b58f3534721147ce517c1d89565d819088c   (no)
reference                  a25c4aace6ef1a2049f8b8219175c1ca8df4025fa4884d82693c22ab46744a88
```

And the consequence there is an **aliasing collision**: two records
sharing their first 8191 canonical bytes hash identically.

## The fix, and why it is bounded

Both sites now go through one `canon_begin()`/`canon_end()` sink built
on **`open_memstream()`** — the same glibc extension the port already
required for `fmemopen`, so the Makefile's `-std=gnu99` note now covers
both. It reallocates as the emitter writes, so:

> **There is no constant to get wrong, and therefore no bound to prove.**

Allocation failure now exits loudly, because a short canonical form
hashes to a plausible-looking wrong leaf — which is the exact failure
mode being removed.

`values_equal` now compares lengths then `memcmp`, and frees both.

## The test-design detail worth stealing

The mutation table includes a row that a naive suite would miss:

| mutation | result |
|---|---|
| `record_leaf` back to `char buf[8192]` | 4 fail |
| `values_equal` back to `char ba[4096]` | 2 fail |
| **buffer raised to 1 MiB ("bigger buffer")** | **caught only by the 4 MiB fixture** |
| hash `c.len - 1` | 18 fail |

**A "just raise the constant" fix passes the 8 KiB and 200 KB checks** and is caught only by the 4 MiB one. The suite has to be big enough that a bigger constant is not enough.

## The sibling audit

Every other port was run on the 8 KB, 200 KB and 4 MiB records and compared:

| port | mechanism | verdict |
|---|---|---|
| `gitpp-js` | string concat + `TextEncoder` | unbounded |
| `gitpp-cs` | `StringBuilder`, **default capacity, no cap set** | unbounded |
| `gitpp-pl` | concat + `join` | unbounded |
| `git.pp-doctor` | `json.dumps` | unbounded (reference) |
| `gitpp-beam` | iodata → `IO.iodata_to_binary` | unbounded by construction (not executed) |

**`gitpp-c` was the only port with the defect.** A truncation-shaped
sweep (`substr`, `slice`, `binary_part`, `Substring`, `Capacity =`,
magic 4096/8192) returned only benign hits.

## One deliberate non-change

`closure_root()` truncates ids at 255 bytes into `closure_ids[][256]`.
That array is written by `main()` and never read — the ids feeding the
hash come from the untruncated `closure_set` — so the root is correct.
It is not an instance of this defect, and changing the public signature
for it would be a different change.
