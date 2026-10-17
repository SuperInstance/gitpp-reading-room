#!/usr/bin/env python3
import json, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fold import derive
print(json.dumps(derive(json.load(sys.stdin)), ensure_ascii=False))
