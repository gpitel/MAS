#!/usr/bin/env python3
"""Validate every record in MAS/data/*.ndjson against its schema (Python mirror of TestData.cpp Data suite)."""
import json, sys, glob
from jsonschema import Draft202012Validator
from referencing import Registry, Resource
ROOT='.'
MAP={
 'core_shapes':'schemas/magnetic/core/shape.json','bobbins':'schemas/magnetic/bobbin.json',
 'core_materials':'schemas/magnetic/core/material.json','wires':'schemas/magnetic/wire.json',
 'insulation_materials':'schemas/magnetic/insulation/material.json','wire_materials':'schemas/magnetic/wire/material.json',
 'cores_stock':'schemas/magnetic/core.json','cores':'schemas/magnetic/core.json',
}
def registry():
    res=[]
    for base in ['schemas','../PEAS/schemas']:
        for p in glob.glob(f'{base}/**/*.json',recursive=True):
            try: s=json.load(open(p))
            except: continue
            if isinstance(s,dict) and '$id' in s: res.append((s['$id'],Resource.from_contents(s)))
    return Registry().with_resources(res)
reg=registry(); rc=0
for name,schema_rel in MAP.items():
    path=f'data/{name}.ndjson'
    try: schema=json.load(open(schema_rel))
    except Exception as e: print(f'SKIP {name}: {e}'); continue
    v=Draft202012Validator(schema,registry=reg)
    n=bad=0; first=None
    for line in open(path):
        line=line.strip()
        if not line or line.startswith('version https'): continue
        n+=1
        try: doc=json.loads(line)
        except: bad+=1; first=first or 'JSON parse error'; continue
        errs=list(v.iter_errors(doc))
        if errs: bad+=1; first=first or f"{list(errs[0].absolute_path)}: {errs[0].message[:90]}"
    status='OK ' if bad==0 else 'FAIL'
    if bad: rc=1
    print(f'  {status} {name:18} {n-bad}/{n} valid' + (f'  e.g. {first}' if bad else ''))
sys.exit(rc)
