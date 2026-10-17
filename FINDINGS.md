========================================================================
  gitpp-reading-room — cross-reading report
========================================================================
  determined by the spec : 8 / 10 probes
  UNDERDETERMINED        : 2 / 10 probes

  p01-ancestry-vs-class
    clause    : §5.1 'class precedence is absolute ... even when the witness is a descendant' vs §5.2 'descendant wins' within a class. Does an override that is an ANCESTOR beat a task that is its DESCENDANT?
    expected  : class-absolute -> x=override ; ancestry-first -> x=task
    [reader1-py]
       {"conflicts": [{"key": "x", "losers": ["t"], "winner": "o"}], "order": ["o", "t"], "orphans": [], "state": {"x": "override"}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["o"], "winner": "t"}], "order": ["o", "t"], "orphans": [], "state": {"x": "task"}}

  p02-witness-descendant-of-task
    clause    : Same question, the other direction: a witness that DESCENDS from the task. The spec's parenthetical suggests task wins; a naive reading of 'descendant wins' gives the witness.
    expected  : class-absolute -> x=task ; ancestry-first -> x=witness
    [reader1-py]
       {"conflicts": [{"key": "x", "losers": ["w"], "winner": "t"}], "order": ["t", "w"], "orphans": [], "state": {"x": "task"}}
    [reader2-js]
       {"conflicts": [{"key": "x", "losers": ["t"], "winner": "w"}], "order": ["t", "w"], "orphans": [], "state": {"x": "witness"}}

========================================================================
  A disagreement here is not a bug report. It is a clause the text
  does not determine -- two careful readers shipped different systems
  and neither can say the other is wrong.
========================================================================
