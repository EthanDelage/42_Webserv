#!/usr/bin/python
import cgi
import html

print('Content-Type: text/html\r\n\r\n')

form = cgi.FieldStorage()
var1 = form.getvalue('var1')
var2 = form.getvalue('var2')

print('<html>')
print('<head>')
print('<title>CGI Result</title>')
print('</head>')
print('<body>')
print('<h1>Variables from URL:</h1>')

if var1 is not None:
    var1 = html.escape(var1)
else:
    var1 = "Variable 1 not provided"
print('<p>Variable 1: {0}</p>'.format(var1))

if var2 is not None:
    var2 = html.escape(var2)
else:
    var2 = "Variable 2 not provided"
print('<p>Variable 2: {0}</p>'.format(var2))

print('</body>')
print('</html>')
