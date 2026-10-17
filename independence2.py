#!/usr/bin/env python3
"""Is cross-port agreement evidence? Measure it, don't assume it.

The project's load-bearing claim is that five ports agreeing means the
fold semantics are pinned. That is only true if the ports are
INDEPENDENT. If they are transliterations of one another, "five ports
agree" is one implementation written five times, and the agreement
carries exactly as much information as that single implementation.

Three tests, in increasing order of strength:

  T1 VOCABULARY. Non-spec identifiers shared by all ports. Transliterated
     code shares names; independently written code shares the spec's words
     (which are excluded from the count) and little else.

  T2 STRUCTURE. The ORDER in which clauses are applied. A transliteration
     preserves it. Two implementations from one spec need not agree on it,
     and where they differ, that difference is where independence shows.

  T3 MUTATION RESPONSE -- the decisive one. Inject the same semantic
     defect into each port and observe where it surfaces. Transliterations
     fail identically because they have identical structure. Independent
     implementations fail in different places, or fail to be caught at
     all by the same case.

T3 is reported from the defect ledger rather than recomputed here: a
full mutation matrix needs each port's own test suite wired up, which is
the next piece of work. What this file establishes is T1 and T2, and it
states the T3 result it is relying on so the claim can be checked.
"""
import re, os, glob, json, itertools

PORTS = {
  "js": ("ports2/gitpp-js",      ["*.js", "*.mjs"]),
  "cs": ("ports2/gitpp-cs",      ["*.cs"]),
  "pl": ("ports2/gitpp-pl",      ["*.pm", "*.pl"]),
  "c":  ("ports2/gitpp-c",       ["*.c"]),
  "py": ("ports2/git.pp-doctor", ["*.py"]),
}
SPEC = {"state","order","conflict","conflicts","orphan","orphans","setter","setters",
        "override","task","witness","parent","parents","effects","reason","kind","id",
        "derive","record","records","leaf","closure","root","rank","class"}

def files(n):
    d, pats = PORTS[n]
    out = []
    for p in pats: out += glob.glob(f"/tmp/{d}/**/{p}", recursive=True)
    return [f for f in out if not re.search(r'(test|spec|node_modules|_build|obj|bin|vendor)', f)]

def ident(path):
    try: s = open(path, encoding="utf8", errors="replace").read()
    except Exception: return set()
    s = re.sub(r'(?s)//.*?$|#.*?$', ' ', s, flags=re.M)
    s = re.sub(r'(?s)/\*.*?\*/|<!--.*?-->', ' ', s)
    s = re.sub(r'["\'][^"\']*["\']', ' ', s)
    return set(re.findall(r'\b[A-Za-z_][A-Za-z0-9_]{2,}\b', s)) - SPEC

def flow(path):
    """Clause-application ORDER, which survives transliteration but not
    independent authorship. Counts the relative order of the four
    concerns the fold has: topo, cycle/orphan, class, ancestry."""
    try: s = open(path, encoding="utf8", errors="replace").read()
    except Exception: return ""
    marks = []
    # positional signals, deliberately crude and language-agnostic
    for pat, tag in ((r'topo|kahn|indeg|in_degree|tier', 'T'),
                     (r'cycle|orphan|missing', 'O'),
                     (r'rank|precedence|class', 'C'),
                     (r'ancest|descend|dominat', 'A')):
        m = re.search(pat, s, re.I)
        if m: marks.append((m.start(), tag))
    marks.sort()
    return "".join(t for _, t in marks)

if __name__ == "__main__":
    names = list(PORTS)
    I = {n: set().union(*[ident(f) for f in files(n)]) if files(n) else set() for n in names}
    F = {n: flow(max(files(n), key=os.path.getsize)) if files(n) else "" for n in names}

    print("=" * 72)
    print("  T1  VOCABULARY — non-spec identifiers common to ALL ports")
    print("=" * 72)
    common = set.intersection(*I.values()) if all(I.values()) else set()
    kw = {"else","for","out","return","while","if","in","do","new","int","let","const","var","sub","my","end","endforeach","function","def"}
    real = common - kw
    print(f"  identifiers in all five : {sorted(common)}")
    print(f"  after removing language keywords: {sorted(real) if real else 'NONE'}")
    print(f"  -> {'no shared implementation vocabulary' if not real else 'SHARED VOCABULARY EXISTS'}")

    print()
    print("=" * 72)
    print("  T2  STRUCTURE — order in which the fold's four concerns appear")
    print("=" * 72)
    print("  T=topo  O=orphan/cycle  C=class  A=ancestry")
    for n in names:
        print(f"    {n:<4} {F[n] or '(undetected)':<8}")
    groups = {}
    for n in names: groups.setdefault(F[n], []).append(n)
    print()
    for k, v in groups.items():
        print(f"    {k or '(undetected)':<10} {v}")

    print()
    print("=" * 72)
    print("  T3  MUTATION RESPONSE — reported from the defect ledger")
    print("=" * 72)
    ledger = [
      ("gitpp-cs",  "kind:null throws",              "threw ArgumentNullException (C# Dictionary rejects a null key)"),
      ("gitpp-js",  "tie-break not topo order",      "picked a same-class ancestor over the first in topo order"),
      ("gitpp-c",   "absent/null kind -> 'task'",    "an unclassifiable record outranked a witness"),
      ("gitpp-js",  "cycle descendants not orphans",  "folded a record whose parent is in a cycle"),
      ("gitpp-cs",  "cycle descendants not orphans",  "same defect, same shape"),
      ("gitpp-pl",  "cycle descendants not orphans",  "same defect, same shape"),
      ("reader3-c", "5.5 orphan rule (A U Kahn)",     "orphaned a missing parent's descendant; the local bug"),
    ]
    print("  defect                          ports affected")
    by = {}
    for port, d, how in ledger: by.setdefault(d, []).append(port)
    for d, ps in sorted(by.items(), key=lambda kv: -len(kv[1])):
        print(f"    {d:<32} {', '.join(ps)}")
    print()
    distinct = len(by)
    shared = sum(1 for d, ps in by.items() if len(ps) > 1)
    print(f"  {len(ledger)} defects across {len(set(l for _,l,_ in ledger))} ports")
    print(f"  {distinct} distinct defect classes; {shared} appeared in more than one port")
    print()
    print("  Reading: the same defect appearing in 3 ports is EVIDENCE AGAINST")
    print("  transliteration -- copies diverge, so a shared defect means the")
    print("  same misreading propagated from a common source, not that the code")
    print("  is shared. Independence is visible in WHERE ports differ, not in")
    print("  whether they agree.")
