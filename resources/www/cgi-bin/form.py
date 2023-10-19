import os
import sys

input_string = sys.stdin.read()
print("Content-type:text/html\r\n\r\n")
print('<html>')
print('<head>')
print('<title>Hello Word - First CGI Program</title>')
print('</head>')
print('<body>')
print('<h2>Hello Word! This is my first CGI program</h2>')
print('<p>' + os.environ['PATH_INFO'] + '</p>')
print('<p>' + os.environ['QUERY_STRING'] + '</p>')
print('<p>' + input_string + '</p>')
print('</body>')
print('</html>')