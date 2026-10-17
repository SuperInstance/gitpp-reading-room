========================================================================
  gitpp-reading-room — cross-reading report
========================================================================
  determined by the spec : 7 / 13 probes
  UNDERDETERMINED        : 6 / 13 probes

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

  p12-kind-null
    clause    : 1.1 says kind defaults to null and other values are 'out of scope for the fold'; 5.1 ranks every record by class. What class does a null-kind record have?
    expected  : null ranks last -> x=from-override ; null throws ; null inherits witness -> x=from-null
    [reader1-py, reader3-c]
       {"conflicts": [{"key": "x", "losers": ["b"], "winner": "a"}], "order": ["a", "b"], "orphans": [], "state": {"x": "from-override"}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["a"], "winner": "b"}], "order": ["a", "b"], "orphans": [], "state": {"x": "from-null"}}

  p13-class-vs-ancestry-inversion
    clause    : 5.1's parenthetical mentions ancestry as an exception. Does that generalise to 'class beats ancestry' everywhere, or is ancestry only a tie-break within a class?
    expected  : class-absolute -> x=1 (witness loses despite descending) ; ancestry-first -> x=2
    [reader1-py, reader3-c]
       {"conflicts": [{"key": "x", "losers": ["b"], "winner": "a"}], "order": ["a", "b"], "orphans": [], "state": {"x": 1}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["a"], "winner": "b"}], "order": ["a", "b"], "orphans": [], "state": {"x": 2}}

========================================================================
  A disagreement here is not a bug report. It is a clause the text
  does not determine -- two careful readers shipped different systems
  and neither can say the other is wrong.
========================================================================
