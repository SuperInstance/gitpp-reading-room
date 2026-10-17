#!/usr/bin/env python3
"""Adapter: independent C99 reader (gitpp-rr-reader3) -> harness JSON.

Runs each probe in its own temp file; three readers share a directory
and a fixed path made them race, which showed up as ERROR and read as
disagreement. Isolation is per-invocation.
"""
import json, sys, subprocess, tempfile, os
recs = sys.stdin.read()
fd, tmp = tempfile.mkstemp(suffix=".json", dir=os.path.dirname(os.path.abspath(__file__)))
os.close(fd)
try:
    with open(tmp, "w") as f: f.write(recs)
    r = subprocess.run([os.path.join(os.path.dirname(os.path.abspath(__file__)), "fold"), tmp],
                       capture_output=True, text=True, timeout=60)
    if r.returncode != 0:
        print(json.dumps({"__error__": (r.stderr or r.stdout).strip().splitlines()[-1][:100]})); sys.exit(0)
    d = json.loads(r.stdout)
    print(json.dumps({k: d.get(k) for k in ("state", "order", "conflicts", "orphans")}))
finally:
    os.unlink(tmp)
