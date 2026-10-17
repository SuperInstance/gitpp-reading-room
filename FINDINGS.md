========================================================================
  gitpp-reading-room — cross-reading report
========================================================================
  determined by the spec : 6 / 10 probes
  UNDERDETERMINED        : 4 / 10 probes

  p01-ancestry-vs-class
    clause    : §5.1 'class precedence is absolute ... even when the witness is a descendant' vs §5.2 'descendant wins' within a class. Does an override that is an ANCESTOR beat a task that is its DESCENDANT?
    expected  : class-absolute -> x=override ; ancestry-first -> x=task
    [reader1-py, reader3-c]
       {"conflicts": [{"key": "x", "losers": ["t"], "winner": "o"}], "order": ["o", "t"], "orphans": [], "state": {"x": "override"}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["o"], "winner": "t"}], "order": ["o", "t"], "orphans": [], "state": {"x": "task"}}

  p02-witness-descendant-of-task
    clause    : Same question, the other direction: a witness that DESCENDS from the task. The spec's parenthetical suggests task wins; a naive reading of 'descendant wins' gives the witness.
    expected  : class-absolute -> x=task ; ancestry-first -> x=witness
    [reader1-py, reader3-c]
       {"conflicts": [{"key": "x", "losers": ["w"], "winner": "t"}], "order": ["t", "w"], "orphans": [], "state": {"x": "task"}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["t"], "winner": "w"}], "order": ["t", "w"], "orphans": [], "state": {"x": "witness"}}

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
