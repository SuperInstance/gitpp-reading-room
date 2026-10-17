#!/usr/bin/env python3
"""Two readings of SPEC 5.4's 'Kahn's algorithm with smallest-id-first'.

    A (tier/heap):  compute the ready set, ORDER IT, take the smallest.
    B (sequential): take the smallest ready, emit it, then RECOMPUTE
                    readiness and choose again.

These differ whenever emitting one node makes another node ready whose
id sorts BEFORE an already-ready peer.

    m (root)  z (root)  mm (parent m)

    ready = {m, z}.  smallest = m.
    B: emit m. Now mm is ready. ready = {z, mm}, smallest = mm.
       -> [m, mm, z]
    A: the tier was {m, z} ordered -> [m, z], then mm.
       -> [m, z, mm]

Both are 'smallest-id-first'. The spec does not say which.

A previous version of this file was itself buggy -- its 'tier' mode
released the tier but then only emitted one node per iteration, so it
was not a tier implementation at all. Fixed here, and the fix is why
only the discriminating case is reported.
"""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "readers/py"))
from fold import cp

R = lambda i, p, e: {"id": i, "kind": "task", "parents": p, "effects": e, "reason": None}

def topo(recs, mode):
    by = {r["id"]: r for r in recs}
    par = lambda k: (by[k].get("parents") or [])
    indeg = {k: sum(1 for p in par(k) if p in by) for k in by}
    done, order = set(), []
    while len(order) < len(by):
        ready = sorted([k for k in by if k not in done and indeg[k] == 0], key=cp)
        if not ready: break
        if mode == "tier":
            take = [ready[0]]
        else:
            take = [ready[0]]
        for k in take:
            order.append(k); done.add(k)
            for m in by:
                if m not in done and k in par(m):
                    indeg[m] -= 1
    return order

def topo_heap(recs):
    """A true tier/heap reading: the whole ready set is ordered, and the
    next node is the minimum of the CURRENT ready set, re-evaluated after
    every emission. That is what a heap-based Kahn does, and it is
    identical to 'sequential' -- which is the point."""
    return topo(recs, "seq")

CASES = {
 "d26: three roots, one has a child":
   [R("only",[],{"status":"wip"}), R("a",[],{"x":1}), R("b",["a"],{"x":2})],
 "DISCRIMINATOR: peer ready with a smaller id":
   [R("z",[],{"q":1}), R("m",[],{"r":2}), R("mm",["m"],{"s":3})],
 "plain chain":
   [R("a",[],{}), R("b",["a"],{}), R("c",["b"],{})],
 "diamond":
   [R("r",[],{}), R("l",["r"],{}), R("m",["r"],{}), R("j",["l","m"],{})],
}

if __name__ == "__main__":
    for name, recs in CASES.items():
        seq = topo(recs, "seq")
        # a heap Kahn and a sequential Kahn coincide; the only alternative
        # is 'release the whole tier then order it', which cannot be
        # expressed as a single pass, so model it as: round-based.
        indeg = {r["id"]: sum(1 for p in r["parents"] if any(x["id"]==p for x in recs)) for r in recs}
        byid = {r["id"]: r for r in recs}
        done, tier_order = set(), []
        while len(tier_order) < len(recs):
            tier = sorted([k for k in byid if k not in done and indeg[k]==0], key=cp)
            if not tier: break
            for k in tier:
                tier_order.append(k); done.add(k)
                for m in byid:
                    if m not in done and k in (byid[m]["parents"] or []):
                        indeg[m] -= 1
        same = seq == tier_order
        print(f"  {name}")
        print(f"      sequential : {seq}")
        print(f"      whole-tier  : {tier_order}")
        print(f"      {'SAME' if same else '*** SPLIT ***'}")
    print()
    print("  A heap-based Kahn IS the sequential one: both take the minimum")
    print("  of the current ready set and re-evaluate after every emission.")
    print("  The only genuinely different implementation is one that releases")
    print("  an entire tier, orders it, and then starts the next tier -- which")
    print("  the spec's 'Kahn's algorithm with smallest-id-first' does not")
    print("  exclude, and which the DISCRIMINATOR case separates.")
