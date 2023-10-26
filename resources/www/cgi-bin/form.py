import cgi

print("Content-Type: text/html\r\n\r\n")

form = cgi.FieldStorage()
if form.getvalue('name') is None:
    print("Enter your name:")
    print('<form action="form.py" method="post">')
    print('<input type="text" name="name" />')
    print('<input type="submit"></form>')
else:
    print(f"Your name is {cgi.FieldStorage.getvalue('name')}")
