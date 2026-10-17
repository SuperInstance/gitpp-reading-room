# The last unverified component now runs, and it is not conformant

The Elixir port was the one part of this project that had been pushed
and never executed. It has now been executed. **The premise behind
avoiding it was wrong twice, and the result is worse than "unverified."**

## The premise was wrong twice

**There was no Erlang either.** The project summary said OTP 25 was
installed (`erlang-base`, `erlang-crypto`, `erlang-dev`). Measured:
`which elixir mix iex erl escript` → all missing, `dpkg -l | grep
erlang` → empty. After `apt-get update`, Elixir 1.14 + OTP 25 installed
cleanly.

**And 1.14 is not enough.** The port needs **1.17** (`^decpt` in a
runtime-sized binary segment). CI's exact toolchain — OTP 27.3.4.9 +
Elixir 1.17.3 — was used, on pristine upstream `65b8a4d` with `lib/`,
`test/` and `mix.exs` byte-identical to the clone. No shims.

Also: the repo is `patchwork-quilt/gitpp-beam`, not `git.pp-beam`. The
URL in the original brief 404s.

## Result: 3 of 4 pinned leaves

| case | pinned | port | |
|---|---|---|---|
| case-1-minimal | `229f56e0…3124` | `229f56e0…3124` | **pass** |
| case-2-nested | `64a3cd29…6ad9` | `64a3cd29…6ad9` | **pass** |
| case-3-bool-string | `97fd12ee…2f7` | `97fd12ee…2f7` | **pass** |
| case-4-non-bmp | `83d31b81…e597` | `9bd5b9b1…e552` | **fail** |

**The pin is right and the port is wrong.** Verified by running the
other ports and by hashing the `ensure_ascii=False` form directly:

```
python3: json.dumps({"k":"é😀"}, ensure_ascii=False, separators=(", ",": "))
  ->  {"k": "é😀"}
  ->  sha256 0b138a10…  (matches the other ports)
port:    {"k": "\u00e9\ud83d\ude00"}
```

**And the port's own test asserts the wrong answer**, so the test
actively protects the defect:

```elixir
# test/canon_test.exs:30
assert Canon.encode!(%{"k" => "é😀"}) == ~S({"k": "\u00e9\ud83d\ude00"})
```

That is a test that would have to be *changed* as part of the fix. Worth
calling out separately: a suite where a failing expectation is the
specification is worse than no test, because it converts a defect into
an enforced behaviour.

## A fourth defect the first pass missed: the locale

Running the port at all surfaced something the analysis did not:

```
warning: the VM is running with native name encoding of latin1 ...
{"k": "\u00c3\u00a9\u00f0\u009f\u0098\u0080"}
```

Without `ELIXIR_ERL_OPTIONS="+fnu"` and a UTF-8 locale, the emoji is
re-encoded as Latin-1 before the canonicaliser sees it. A conforming
implementation of this spec is **locale-dependent**, which is
categorically wrong: the leaf must be a pure function of the record.
The correct output — `{"k": "\u00e9\ud83d\ude00"}` — only appears once
the locale is set.

## CI is red, and has been

`patchwork-quilt/gitpp-beam/.github/workflows/leaf-conformance.yml` ran
on `65b8a4d` and **failed**. Reproduced byte-for-byte:

```
1) test §3 integral float band 1e16 is SCIENTIFIC, not integer digits
   assert Canon.leaf_number(1.0e16) == "1e+16"
   left:  "10000000000000000.0"
   15 tests, 1 failure
```

So the "NOT EXECUTED" commit message is stale in the wrong direction:
the tests **do** run, and they **do** fail. The badge is honest.

Note the malformed output: `10000000000000000.0` is a number string no
other port emits. The pushed fix (`fix/null-kind-crash`'s Elixir
counterpart) moved the guard to `1e16` but left a **second, independent
stale threshold** at `canon.ex:171`:

```elixir
if exp < -4 or exp >= 17 do   # CPython uses 16
```

So the fix repaired `[1e17, 1e21)` and left `[1e16, 1e17)` broken, while
making its output malformed. `17` → `16` is the missing half.

## Two more

- **`:json.decode` on OTP 27 maps JSON `null` to the atom `:null`**, and
  `Canon.encode/1` has only a `nil` clause. Any record with an explicit
  null crashes: `FunctionClauseError … in Gitpp.Canon.encode/1 … :null`.
  The port's own fixtures never carry a null-valued key, so nothing
  caught it.
- **`gitpp-cross-port` has no Elixir wrapper, by design** —
  `wrappers/README.md` does not list one, and `README.md:91` says so
  explicitly. But `README.md:48` asserts the Elixir port "also produces"
  the pinned closure root. That claim is unverified and, on this
  evidence, doubtful for anything beyond the cases it does pass.

## What did pass

`mix test test/burn_test.exs` → 2 tests, 0 failures, and the closure-root
receipt reproduces exactly:

```
checkpoint_root(poc-quilt-1@task-c)
  = 60effe23970d8a5cc837636c44685502361288c1f00577e9addfa10cbd4fc91d
```

**The fold is sound. The divergence is confined to canonical encoding** —
which is the more important half, because encoding is what every other
port's conformance claim rests on.

## The three fixes, in priority order

1. `canon.ex:171` — `exp >= 17` → `>= 16`. Fixes the `[1e16, 1e17)` band
   and the failing test.
2. `canon.ex:113-141` — `encode_string` to `ensure_ascii=False`
   semantics, **and** `test/canon_test.exs:30`, which currently pins the
   wrong answer. Fixes case-4.
3. `canon.ex:98` — add a `:null` clause, or normalise at the decode
   boundary. Fixes the OTP 27 crash.

Plus the locale requirement, which is not a code fix but a **deployment
constraint that must be stated**: this port does not produce a
conforming leaf unless `ELIXIR_ERL_OPTIONS="+fnu"` and a UTF-8 locale
are set. A spec that says the leaf is a pure function of the record is
not satisfied by an implementation that needs environment variables to
be correct.
