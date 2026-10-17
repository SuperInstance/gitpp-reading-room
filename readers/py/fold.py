#!/usr/bin/env python3
"""Reader 1 — git.pp fold, Python, stdlib only.

READING: §5.1 class precedence is ABSOLUTE. §5.2 uses ancestry only as a
TIE-BREAK when two records are in the SAME class. So on p01 an override
that is an ANCESTOR still beats a task that is its DESCENDANT.

NOTE ON PROVENANCE: this reader and readers/js were written by the same
author, who is also the author of the spec and the corpus. Their
agreement with each other is therefore evidence of nothing. They are kept
in-repo because the experiment must be reproducible, not because their
mutual agreement is meaningful. readers/c is the independent one.
"""
import json, sys, hashlib

RANK = {"override": 3, "task": 2, "witness": 1}
cp = lambda s: [ord(c) for c in str(s)]

def esc(s):
    o = ['"']
    for c in s:
        if c == '"': o.append('\\"')
        elif c == "\\": o.append("\\\\")
        elif c == "\n": o.append("\\n")
        elif c == "\r": o.append("\\r")
        elif c == "\t": o.append("\\t")
        elif c == "\b": o.append("\\b")
        elif c == "\f": o.append("\\f")
        elif ord(c) < 0x20: o.append("\\u%04x" % ord(c))
        else: o.append(c)
    o.append('"')
    return "".join(o)

def sci(v):
    for d in (15, 16, 17):
        s = "%.*g" % (d, v)
        if float(s) == v:
            m, _, e = s.partition("e")
            if "." in m: m = m.rstrip("0").rstrip(".")
            return f"{m}e{'+' if not e.startswith('-') else '-'}{abs(int(e)):02d}"
    raise ValueError("no round-trip")

def canonical(v):
    if v is None: return "null"
    if v is True: return "true"
    if v is False: return "false"
    if isinstance(v, int): return str(v)
    if isinstance(v, float):
        if v != v or v in (float("inf"), float("-inf")):
            raise ValueError("non-finite: out of pinned domain")
        if v == 0: return "0"
        a = abs(v)
        if a < 1e-4: return sci(v)
        if v.is_integer() and a < 1e16: return str(int(v))
        if v.is_integer(): return sci(v)
        return sci(v) if a >= 1e16 else repr(v)
    if isinstance(v, str): return esc(v)
    if isinstance(v, list): return "[" + ", ".join(canonical(x) for x in v) + "]"
    if isinstance(v, dict):
        return "{" + ", ".join(esc(k) + ": " + canonical(v[k])
                               for k in sorted(v, key=cp)) + "}"
    raise ValueError(f"unencodable {type(v)}")

def leaf(rec):
    canon = {"effects": rec.get("effects") or {}, "id": rec.get("id", ""),
             "kind": rec.get("kind"),
             "parents": sorted(rec.get("parents") or [], key=cp),
             "reason": rec.get("reason")}
    return hashlib.sha256(canonical(canon).encode("utf-8")).hexdigest()

def derive(records):
    by = {r["id"]: r for r in records if r.get("id")}
    par = lambda r: (r.get("parents") or [])

    def anc(rid):
        seen, st = set(), [rid]
        while st:
            x = st.pop()
            if x in seen: continue
            seen.add(x)
            for p in par(by.get(x, {})):
                if p in by: st.append(p)
        seen.discard(rid)
        return seen

    # §5.5 — three clauses, because two are not enough.
    #   1. a parent id is missing from the applied set
    #   2. the record cannot be topologically ordered   (cycle member)
    #   3. a parent cannot be topologically ordered       (cycle DESCENDANT)
    # Clause 3 is what separates a cycle descendant from a missing-parent
    # descendant: the former's parent EXISTS but is unorderable, the latter's
    # does not exist. A rule keyed on ancestry alone gets the first wrong; a
    # rule keyed on existence alone gets the second wrong.
    #
    # `orderable` is Kahn over the full set with in-degree counting only
    # parents that EXIST -- an absent parent cannot block a record, which is
    # exactly why a missing-parent's child stays orderable.
    def orderable_set():
        indeg0 = {k: sum(1 for p in par(r) if p in by) for k, r in by.items()}
        done, avail = set(), dict(by)
        while True:
            ready = sorted([k for k, d in indeg0.items()
                            if d == 0 and k in avail and k not in done], key=cp)
            if not ready:
                break
            pick = ready[0]
            done.add(pick); avail.pop(pick)
            for k in avail:
                if pick in par(avail[k]):
                    indeg0[k] -= 1
        return done

    _ord = orderable_set()
    orphans = set()
    for k, r in by.items():
        ps = par(r)
        if any(p not in by for p in ps):                    # 1
            orphans.add(k)
        elif k not in _ord:                                 # 2
            orphans.add(k)
        elif any(p not in _ord for p in ps if p in by):      # 3
            orphans.add(k)

    applied = {k: v for k, v in by.items() if k not in orphans}

    # §5.4 — Kahn, smallest id first
    indeg = {k: 0 for k in applied}
    for k, r in applied.items():
        indeg[k] = sum(1 for p in par(r) if p in applied)
    order, pool = [], dict(applied)
    while True:
        ready = [k for k, d in indeg.items() if d == 0 and k in pool]
        if not ready: break
        pick = min(ready, key=cp)
        order.append(pick); pool.pop(pick)
        for k in pool:
            if pick in par(applied[k]): indeg[k] -= 1

    A = {k: anc(k) for k in applied}
    state, setters, conf = {}, {}, {}
    for k in order:
        for key, v in (applied[k].get("effects") or {}).items():
            if key not in state:
                state[key] = v; setters[key] = k; continue
            cur = setters[key]
            rk, ck = RANK.get(applied[k].get("kind"), 0), RANK.get(applied[cur].get("kind"), 0)
            # CLASS FIRST (absolute); ancestry only breaks a class tie
            takes = rk > ck or (rk == ck and cur in A[k])
            c = conf.setdefault(key, {"key": key, "winner": cur, "losers": []})
            if takes:
                if canonical(state[key]) != canonical(v) and cur not in c["losers"]:
                    c["losers"].append(cur)
                c["winner"] = k
                state[key] = v; setters[key] = k
            elif k not in c["losers"]:
                c["losers"].append(k)

    # §5.3 — only when 2+ DISTINCT values exist
    conflicts = []
    for c in conf.values():
        vals = {canonical((applied[k].get("effects") or {})[c["key"]])
                for k in order if c["key"] in (applied[k].get("effects") or {})}
        wv = canonical(state[c["key"]])
        losers = [l for l in c["losers"]
                  if canonical((applied[l].get("effects") or {})[c["key"]]) != wv]
        if len(vals) < 2 or not losers: continue
        conflicts.append({"key": c["key"], "winner": c["winner"], "losers": losers})
    return {"state": state, "order": order, "conflicts": conflicts,
            "orphans": sorted(orphans, key=cp)}

if __name__ == "__main__":
    print(json.dumps(derive(json.load(sys.stdin)), ensure_ascii=False))
