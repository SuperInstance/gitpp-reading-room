# FINDING-27 — the green badge over nothing has a name, a prevalence, and two one-liners

**Date:** 2026-10-18
**Source:** `research/CUTTING-EDGE-2026-10-18.md` (71 arXiv citations)
**Status:** the fleet's signature defect, located in the literature

For a year the fleet has had a recurring, named-by-us failure: **a CI job
that passes while measuring nothing.** A green badge over an empty corpus.
A port that is absent turned into a value. A generator whose failure is
swallowed by a `||` fallback. A participant whose absence is compared as
agreement. A scheduled workflow that does not run on push, so the badge
describes a commit that no longer exists.

**It has a name, a number, and a mechanism.**

## The number

**arXiv:2509.14347** — *"On the Illusion of Success: An Empirical Study of
Job Reruns and Silent Failures in Industrial CI."*

- **142,387 jobs**
- **81 projects**
- **11% of successful jobs** are silent failures
- **11 categories** of silence

**One job in nine that reports success is a lie.** Not a rare pathology. A
**routine outcome** in industrial CI, measured at scale, by people who are
not us and have no stake in our fleet.

## The two one-liners that manufacture our exact defect

These are in the literature and in the tools' own documentation. **Either
one, pasted into a CI file, reproduces the fleet's signature defect
exactly.**

### 1. `pytest-custom_exit_code`

pytest's exit code **5** means *"no tests were collected."* That is a
**distinct, meaningful** value: it says the suite did not run.

**`pytest-custom_exit_code` exists to erase that distinction.** It exists
to make pytest exit 0 where it would exit 5 — because some CI systems
treat a non-zero exit as a failure and the author wanted a green job.

**That is not a workaround. That is a device for turning "the suite did not
run" into "the suite passed."** And it is a *published package*, so it can
be installed like any other.

### 2. `suppress_health_check=list(HealthCheck)`

pytest emits a `filter_too_much` health-check warning **exactly when
over-filtering produces a vacuous pass** — when the filters you wrote
removed every test, or every assertion, and pytest is telling you so.

**`suppress_health_check=list(HealthCheck)` disables that warning.**

So the tool was *designed* to notice our exact defect, and the suppression
is a supported, documented, one-line way to turn it off.

**Both should be BANNED BY NAME in this fleet's CI**, the way the house
bans a fallback that swallows a setup failure. A ban is enforceable
(grep the workflows); a convention is not.

## The stale badge, documented

GitHub's own documentation says:

> *"If there are no workflow runs on your default branch, it will display
> the status of the most recent run across all branches."*

**Our stale badge is the documented default, not a bug.** The fix is
`?event=push` on the badge URL, which restricts it to runs on the branch
the badge is for.

**The fleet's own memory already recorded this** — *"a workflow triggered on
schedule but not on push → stale badge describing a commit that no longer
existed"* — and filed it as our problem. **It is GitHub's documented
behaviour.** The fix is one query parameter, and it should be applied to
every badge the fleet publishes, because a badge that means "some run
somewhere" is a false claim at a boundary.

## The standards the witness log needs

Two, both named in the survey, and **both now adopted** in
`SuperInstance/witness-spec`:

- **RFC 6962 §2.1** — domain separation (`0x00` leaf, `0x01` node) plus a
  periodic **signed tree head**. The survey's blunt finding: *an FNV-1a-64
  chain with no signature has NO tamper-evidence at all — anyone who can
  append can rewrite the whole chain.* **The fleet's witness machine had
  exactly this defect and called it tamper-evident.**
- **RFC 8785 (JCS)** — canonicalisation. And it is **already a `MUST` in
  A2A v1.0 §8.4.1**, so this is interoperability, not pedantry.

## The four our-idea findings

**Three things we independently got right, with citations:**

1. **`did:crdt` (arXiv:2606.16223, 2026-06-15)** — a W3C-compliant
   identifier, **BLAKE3-256 content-addressed**, needing *"no network,
   registry, or synchronisation."* **That is our "no separate ID service"
   claim, already published.** We wrote it in the charter and someone else
   published it.
2. **`CanonicalMerge` (arXiv:2607.01308)** — a content-addressed
   state-based CvRDT whose **render is byte-identical under any
   permutation**, verified on real Qwen3 KV state. **That is the
   canonicalisation property our witness chain was reaching for**, solved
   in the model layer.
3. **arXiv:2504.04715** — paid-API audits are *"fundamentally
   unreliable"* because you must re-ask, and nondeterminism defeats the
   re-ask. **This is the strongest external support for the house's design
   decision to store the WITNESSED RECEIPT rather than the paid label.**
   Everyone stores a dataset of paid labels. Nobody stores the receipt.
   We do.

**And one place we are behind, named precisely:**

- **RFC 9153 (DRIP, IETF, 2022-02)** is a regulated **4-field broadcast
  wire format — id, position, timestamp, status** — scoped to *"network
  degraded, and network denied"*, and it draws the exact distinction we
  drew, calling an ICAO address *"arguably really an identifier rather than
  a locator."* **Our ID-versus-address distinction was already
  standardised two years before we wrote it down.**
  - And the instructive part: **OpenXR's answer is a PAIR, not a tuple.**
    `XrSpatialEntityIdEXT` is a scoped opaque atom, and **durability is a
    UUID you must store yourself.**
  - **Nobody ships a four-tuple.** Our `(x, y, t, sig)` is unclaimed *as a
    name* and unimplemented *as a thing*, and the survey says so in one
    line: *"documented at `algebra.md:129` and implemented zero times."*

## And the one we should read before writing more

**Yjs** addresses every CRDT item as `(client, random uint32, clock)` —
read from `src/utils/ID.js` today.

**That is effectively our `(x, y, t, sig)`, in production, in the most
widely used CRDT for collaborative editing.** We are not novel here. We are
*late*, and the cheap thing is to learn from it rather than to claim it.
