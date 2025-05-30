import cgi

form = cgi.FieldStorage()

expr = form.getvalue("operations", "")

rslt = eval(expr)

print(rslt)
