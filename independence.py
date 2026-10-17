#!/usr/bin/env python3
"""Are these ports INDEPENDENT implementations, or transliterations?

The whole project rests on cross-port agreement. That claim is only
worth something if the implementations are independent -- if agreement
means "several readers read the same text the same way", not "one
implementation typed four times".

Independence is measurable in three ways that do not require guessing:

  1. SHARED VOCABULARY. Two ports derived from one another share
     identifiers, helper names, and comment phrasing. Ports written
     independently from a spec converge on the spec's words, not on each
     other's.

  2. SHARED STRUCTURE. The order in which clauses are applied, and the
     shape of the main loop, are choices. A transliteration keeps them.

  3. DISAGREEMENT HISTORY. Truly independent implementations of an
     ambiguous spec diverge. Ports that always agreed, on a spec this
     project has since found to be ambiguous in four places, are
     evidence of copying rather than of convergence.

This measures 1 and 2 mechanically. 3 is argued from the known defect
set rather than computed, and is labelled as such.
"""
import os, re, sys, json, glob

PORTS = {
  "js":  ("ports2/gitpp-js",    ["*.js", "*.mjs"]),
  "cs":  ("ports2/gitpp-cs",    ["*.cs"]),
  "pl":  ("ports2/gitpp-pl",    ["*.pm", "*.pl"]),
  "c":   ("ports2/gitpp-c",     ["*.c", "*.h"]),
  "py":  ("ports2/git.pp-doctor", ["*.py"]),
}

def sources(name):
    d, pats = PORTS[name]
    out = []
    for p in pats:
        out += glob.glob(os.path.join("/tmp", d, "**", p), recursive=True)
    return [f for f in out if not re.search(r'(test|spec|node_modules|_build|obj|bin|vendor|\.git/)', f)]

# Tokens that are the SPEC's vocabulary, not an implementation's. Counting
# these measures nothing about copying, so they are excluded.
SPEC_WORDS = {
  "state","order","conflict","conflicts","orphan","orphans","setter","setters",
  "override","task","witness","parent","parents","effects","reason","kind","id",
  "derive","record","records","leaf","closure","root","rank","class",
}

def tokens(path):
    try: s = open(path, encoding="utf8", errors="replace").read()
    except Exception: return set()
    s = re.sub(r'(?s)//.*?$|#.*?$', ' ', s, flags=re.M)      # comments
    s = re.sub(r'(?s)/\*.*?\*/|<!--.*?-->', ' ', s)          # block comments
    s = re.sub(r'["\'].*?["\']', ' ', s)                     # string literals
    ids = set(re.findall(r'\b[A-Za-z_][A-Za-z0-9_]{2,}\b', s))
    return ids - SPEC_WORDS

def structure(path):
    """A crude structural fingerprint: the sequence of control-flow and
    comparison keywords, which survives transliteration far better than
    identifiers do."""
    try: s = open(path, encoding="utf8", errors="replace").read()
    except Exception: return []
    s = re.sub(r'(?s)//.*?$|#.*?$', ' ', s, flags=re.M)
    s = re.sub(r'(?s)/\*.*?\*/|<!--.*?-->', ' ', s)
    s = re.sub(r'["\'][^"\']*["\']', ' ', s)
    return re.findall(r'\b(if|else|for|while|foreach|switch|case|return|match|func|fn|def|sub)\b', s)

def jaccard(a, b):
    if not a or not b: return 0.0
    return len(a & b) / len(a | b)

if __name__ == "__main__":
    names = list(PORTS)
    tok = {n: set().union(*[tokens(f) for f in sources(n)]) if sources(n) else set() for n in names}
    stc = {n: structure(max(sources(n), key=os.path.getsize)) if sources(n) else [] for n in names}

    print("  identifier vocabulary outside the spec's own words")
    print(f"  {'pair':<12} {'jaccard':>8}  {'shared tokens'}")
    pairs = []
    for i, a in enumerate(names):
        for b in names[i+1:]:
            j = jaccard(tok[a], tok[b])
            shared = sorted(tok[a] & tok[b])[:6]
            pairs.append((a, b, j, shared))
            print(f"  {a+'/'+b:<12} {j:>8.3f}  {', '.join(shared) if shared else '-'}")
    pairs.sort(key=lambda p: -p[2])
    print()
    print("  structure fingerprint (control-flow keyword sequence), longest common prefix ratio")
    for i, a in enumerate(names):
        for b in names[i+1:]:
            x, y = stc[a], stc[b]
            if not x or not y: continue
            k = 0
            while k < min(len(x), len(y)) and x[k] == y[k]: k += 1
            ratio = k / min(len(x), len(y))
            if ratio > 0.15:
                print(f"    {a}/{b}: shared prefix {k}/{min(len(x),len(y))} = {ratio:.2f}")
    json.dump([{"a":a,"b":b,"jaccard":round(j,4)} for a,b,j,_ in pairs],
              open("/tmp/rr2/independence.json","w"), indent=1)
