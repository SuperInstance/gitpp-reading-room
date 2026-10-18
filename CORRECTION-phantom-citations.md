# CORRECTION — four citations I could not find, and two numbers I got wrong

**Date:** 2026-10-18
**Raised by:** the cutting-edge survey (`research/CUTTING-EDGE-2026-10-18.md`)
**Rule:** *a file cited by a charter may never have existed. Check the commits
API for the path, not the tree.* — and the same applies to a paper.

A research pass over the literature, with 71 arXiv citations and an explicit
"not confident" list, found that **several of the things I had been citing
do not exist.** This file is the correction, and it is kept in full because
a wrong citation is worse than a missing one: it is load-bearing, and the
next reader will build on it.

## 1. Three papers I cited that are not findable

| what I wrote | what the survey found |
|---|---|
| "Alachamy" (proof of execution) | **0 arXiv results**, 1 unrelated repository. Cannot be confirmed. |
| "Chain of Compute" | **No such project** in 19 GitHub results. |
| "CI Falsifier" | Two searches, nothing found. DBLP was anti-bot-blocked that day. |
| "Pitt & Voos 2003" (mutation testing, equivalent mutants) | **Zero in Crossref AND zero in the Uni-Lux mutation-testing index.** Go to the ICST 2003 proceedings directly. |
| "How Do Equivalent Mutants Affect Coverage?" | **No such title.** The real paper is Schuler & Zeller, *"(Un-)Covering Equivalent Mutants"* (ICST 2010). It measures **~45% of undetected mutants as equivalent**, and equivalent-mutant detection is **undecidable**. |

**The lesson, and it is the one that matters:** I did not invent a number.
I invented *citations*. **A citation is a claim that a thing exists**, and
the fleet's own rule — *the broken artifact is not the witness* — applies
to a bibliography too. **A reference list is a boundary surface.** The
house's rule for a repository README is that it must not misdescribe its
own state; the rule for a bibliography is that it must not misdescribe the
literature.

**The cure, and it is the same cure as everywhere else:** the survey
recorded, for every citation it could not confirm, an explicit entry in a
"citations I am NOT confident about" section. **Fifteen flagged.** That
list is more valuable than the confident list, because it is the part that
can be checked.

## 2. The "3.5 KB hidden state" number is wrong

I wrote that a per-hop hidden state transfer is ~3.5 KB, in the ESP32
argument for the 4D address.

**The 3.5 KB figure is a ZK *proof* size in arXiv:2603.18046, and an FPGA
*rule ROM* in arXiv:2608.20497. The real per-token Transformer hidden state
is ~8 KB.**

The *argument* it supported does not collapse — a hidden state is still
small enough to cross a wire between boards, and it is still far smaller
than the 3.82 MB/layer of weights — but the number attached to it was
borrowed from an unrelated paper. **A number attached to a true claim is
still a false claim.**

## 3. Kleppmann's paper has a different title

I wrote "A Highly-Available Move-to-Root Data Structure".

**The real title is "A highly-available move operation for replicated
trees,"** TPDS 33(7), DOI `10.1109/TPDS.2021.3118603`. It is still the
right paper for the point I was making (a move operation that is highly
available). It is not a move-to-*root* data structure paper, and there is a
real difference between the two.

## 4. The "judge moves 0.01–0.02" claim has no source

I recorded — and the reading-room repeats — that a judge's score moves
0.01–0.02 near a threshold.

**The survey could not find that claim in any form, in any paper.** What
*is* sourced (arXiv:2606.19544 — 541,000 judgments, 21 judges) is:

- **verbosity bias < 0.011**
- **kappa deflation of 33–41 percentage points**
- and a **consistency–bias paradox**: two production judges with
  **test–retest > 0.95 and position bias > 0.10**

**Our own measurement stands** — it is ours, measured in this fleet, 9×5
repeats, and the `jev-lab` repo has the numbers. **What was wrong was
implying it came from the literature.** A measured number and a cited
number are different kinds of claim, and conflating them is how a fleet
ends up with a bibliography it does not have.

## What is NOT corrected

**The measurements.** Every number this fleet produced — 20,043/20,043
leaf agreement, 29/29 fold, 131,499 mutants, 0/45 crossings, the 0.01–0.02
judge spread, the 0.99 empty-state launder, the 27/29 corpus sweep, the
0.909/0.909 triage — is ours, measured, and reproducible from the repos.
The survey found nothing wrong with any of them. **It found the citations
around them wrong.**

**And the direction of the correction is the interesting part:** the
survey's own summary — *"the 4D address is documented at `algebra.md:129`
and implemented zero times... it is our only genuinely unclaimed idea, and
it is currently a sentence in a markdown file"* — is more useful than
anything I wrote about it.
