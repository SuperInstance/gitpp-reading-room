# Correction: the Live Canon service did not change its shape. The client was written wrong.

I wrote this in `FINDING-24.md` §7 and `NPM-AUDIT.md`:

> **The canon service changed the SHAPE of its response** — not the data —
> and four published clients broke silently, reporting an outage that was
> not happening.

**The first half is wrong.** The service's response shape did not change.
The client was written against a shape the service never had. Let me be
precise about what the check actually shows.

---

## What the service returns, and has always returned

`SuperInstance/quilt-live-canon`, `worker.js`:

```js
if (path === "/api/canon") {
  return jsonResponse({
    papers: Object.values(CANON).map(p => ({ ... })),
    count: Object.keys(CANON).length,
  });
}
```

I checked the **earliest** commit that touches `worker.js` —
`3c230ebd`, 2026-09-08 — and the handler is already
`{ papers: [...] }`. There is no earlier map-shaped response to have
migrated from.

| commit | date | what it changed |
|---|---|---|
| `3c230ebd` | 2026-09-08 | earliest `worker.js` commit — already `{papers:[...]}` |
| `5c01e517` | 2026-09-08 | 6 new live endpoints |
| `b9a72fb4` | 2026-09-08 | `/api/vibe`, `/api/quilt/verify` |
| `371e07d8` | 2026-09-19 | canon drift closure: bundle the full 71-paper corpus |
| `3574d577` | 2026-09-24 | restore the node test seam |

The shape is constant across all of them.

## What the client was written against

`SuperInstance/canon-graph` and `SuperInstance/canon-claim` have exactly
**two** commits each: `Initial commit` on **2026-09-04**, and my fix.

**The clients were written four days BEFORE the service existed.** The
original client read the response with:

```js
Object.keys(payload).length
```

which counts the response's top-level **keys** — `papers` and `count`,
so 2 — and not its papers. The self-test then asserted `>= 70` and
reported:

```
expected at least 70 papers, got 2
```

**It has been wrong since the day it was written.** There was no
service-side change to break it.

## Why I got it wrong

Because the 404 and the "upstream connect error" body looked like a
service outage, and because the client's own code was evidence of a
"map" shape — but **the client was the broken artifact**, and I read
it as the witness.

That is the reading-room's own lesson, applied to me: *"every bug
found so far was in code that person wrote or in code written to that
person's reading."* I took my own client as the reference for what the
service should return.

## The deeper finding, which is better than the wrong one

The real story is not "the service changed." It is:

> **Two artifacts were written four days apart, on two different
> repositories, with no shared test and no CI, and were never checked
> against each other. A relationship between two artifacts does not
> exist until something tests it.**

And the reason the failure went unnoticed for 44 days is the most
ordinary thing in the fleet:

> **Neither repository had a CI workflow. The test existed, the
> failure was real, and nothing ran it.**

## What is fixed, and what the fix actually is

Both repositories now have CI, and both have an **offline** test that
needs no network — so it cannot flake and it would have caught this on
day one.

```
canon-graph   test/shape.test.js   6 checks   fails 3 of them on the pre-fix client
canon-claim   test/offline.test.js 7 checks
```

Verified: `canon-graph`'s offline test against the original `index.js`:

```
  FAIL the list shape yields 3 papers: Expected values to be strictly equal:
2 !== 3
  FAIL the list shape yields a map of papers: a value has no numeric `number`
  FAIL the list shape renders a graph: the graph did not mention paper-470
  ok   the map shape yields 3 papers
  ok   the map shape yields a map of papers
  ok   the map shape renders a graph
  3 checks FAILED
```

The **normalising fix is still correct** — accepting both shapes is
right regardless of which one the service uses. What changes is the
reason: it is not defensive coding against a service migration, it is
the correction of a client that was always broken.

## And a second, larger finding: two of the four endpoints never existed

While checking the routes, the route table of the deployed worker:

```
/api/canon              200
/api/canon/hash         200
/api/canon/claim        404
/api/canon/drill        404
/api/canon/papers       404
```

`grep -cE "claim|drill" worker.js` → **0**. The deployed service has
never had a `claim` or a `drill` route, and it has no topic-keyed
equivalent — `navigate`, `confluence`, `lineage`, `ghost`, `similar`,
`random` and `tick` are all keyed by paper number.

**So `@superinstance/canon-claim`'s two headline functions — `claim()`
and `drill()`, the ones the package is named for — have never worked
against the live service.** The old self-test expected
`claim('trust ladder')` to return F168. It has never passed, because
nothing ran it.

They now raise `CanonEndpointMissingError`, which says the endpoint was
never deployed and lists what is. A bare `HTTP 404` made this look
like a service outage, and it sent this investigation in the wrong
direction for a while.

`test/live.test.js` is now a **monitor**: it records what the service
actually deploys and goes **red** if that changes, because if the
service grows those routes then `claim()`/`drill()` should be wired up
for real.

---

## The rule this correction adds

| # | rule |
|---|---|
| 48 | **The broken artifact is not the witness.** When a client disagrees with a service, the client is the one under suspicion until history says otherwise — and history is a commit log, not a vibe. |
| 49 | **A relationship between two artifacts does not exist until something tests it.** Two things written four days apart, on different repositories, with no shared test, are two unrelated things. |
| 50 | **A test that no CI runs has not happened.** Both canon clients had a `test` script, a test file, and no workflow. That is the root cause, and it is the cheapest defect in the fleet to fix: add the workflow. |
