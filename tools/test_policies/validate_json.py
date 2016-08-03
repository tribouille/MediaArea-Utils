#!/usr/bin/env python
import json
import sys

def correctly_sorted(obj):
    if isinstance(obj, dict):
        return sorted((k, correctly_sorted(v)) for k, v in obj.items())
    if isinstance(obj, list):
        return sorted(correctly_sorted(x) for x in obj)
    else:
        return obj

if len(sys.argv) != 3:
    print "not enough arguments"
    sys.exit(1)

got = json.loads(sys.argv[1])
wanted = json.loads(sys.argv[2])

if correctly_sorted(got) == correctly_sorted(wanted):
    sys.exit(0)
else:
    sys.exit(1)
