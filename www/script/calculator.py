#!/usr/bin/env python3

import cgi
import sys

# print()


form = cgi.FieldStorage()

expr = form.getvalue("operations", "")
if isinstance(expr, list):
    expr = expr[0]
if expr is None:
    expr = ""

try:
    rslt = eval(expr)
except Exception as e:
    rslt = "Error: " + str(e)

print(rslt, end="")
sys.stdout.flush()
