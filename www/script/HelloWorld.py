import cgi

form = cgi.FieldStorage()
name = form.getvalue("name", "")

print (f"You sent name = {name}")