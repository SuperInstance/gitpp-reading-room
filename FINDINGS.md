========================================================================
  gitpp-reading-room — cross-reading report
========================================================================
  determined by the spec : 14 / 19 probes
  UNDERDETERMINED        : 5 / 19 probes

  p07-orphan-descendant
    clause    : §5.5 'the orphan's descendants are NOT auto-orphaned'. Does the descendant's OWN effects then apply, or is it excluded because its ancestor is missing?
    expected  : kid-applies -> y present ; whole-branch-excluded -> y absent
    [reader1-py, reader2-js]
       {"conflicts": [], "order": ["kid"], "orphans": ["orph"], "state": {"y": "from-kid"}}
    [reader3-c]
       {"conflicts": [], "order": [], "orphans": ["kid", "orph"], "state": {}}

  p10-id-whitespace-and-separators
    clause    : §1 says ids must be free of \n and :. Is that enforced (reject) or merely documented? The corpus cannot tell, but a port that rejects and a port that accepts will diverge on a hostile input.
    expected  : reject vs accept
    [reader1-py, reader2-js]
       {"conflicts": [], "order": ["a:b"], "orphans": [], "state": {"x": 1}}
    [reader3-c]
       {"conflicts": [], "order": [], "orphans": [], "state": {}}

  p17-cycle-descendant
    clause    : 5.5: 'cycle-affected records are orphans'. Does that include a record DOWNSTREAM of a cycle, or only the members? (Clause 2 of the two-clause rule: Kahn non-emission is transitive.)
    expected  : cycle members only -> orphans=[ca,cb], y enters state ; transitive -> orphans=[ca,cb,down]
    [reader1-py, reader3-c]
       {"conflicts": [], "order": [], "orphans": ["ca", "cb", "down"], "state": {}}
    [reader2-js]
       {"conflicts": [], "order": ["down"], "orphans": ["ca", "cb"], "state": {"y": 3}}

  p18-descendant-of-orphan-vs-descendant-of-cycle
    clause    : The PAIR that proves two clauses are needed. A descendant of a MISSING parent must apply; a descendant of a CYCLE must not. They differ only in whether the parent exists.
    expected  : both apply -> y AND z in state ; only the cycle branch excluded -> orphans=[ca,cb,dn], state={y:2}
    [reader1-py]
       {"conflicts": [], "order": ["kid"], "orphans": ["ca", "cb", "dn", "orph"], "state": {"y": 2}}
    [reader2-js]
       {"conflicts": [], "order": ["dn", "kid"], "orphans": ["ca", "cb", "orph"], "state": {"y": 2, "z": 3}}
    [reader3-c]
       {"conflicts": [], "order": [], "orphans": ["ca", "cb", "dn", "kid", "orph"], "state": {}}

  p19-self-loop-descendant
    clause    : A self-parented record: is it a cycle member, and is its child a cycle descendant?
    expected  : a orphan only -> k applies ; transitive -> orphans=[a,k]
    [reader1-py, reader3-c]
       {"conflicts": [], "order": [], "orphans": ["a", "k"], "state": {}}
    [reader2-js]
       {"conflicts": [], "order": ["k"], "orphans": ["a"], "state": {"y": 7}}

========================================================================
  A disagreement here is not a bug report. It is a clause the text
  does not determine -- two careful readers shipped different systems
  and neither can say the other is wrong.
========================================================================
