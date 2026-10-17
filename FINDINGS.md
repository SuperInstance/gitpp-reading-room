========================================================================
  gitpp-reading-room — cross-reading report
========================================================================
  determined by the spec : 11 / 13 probes
  UNDERDETERMINED        : 2 / 13 probes

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

========================================================================
  A disagreement here is not a bug report. It is a clause the text
  does not determine -- two careful readers shipped different systems
  and neither can say the other is wrong.
========================================================================
