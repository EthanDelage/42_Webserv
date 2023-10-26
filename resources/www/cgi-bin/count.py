#!/usr/bin/python

import cgitb

cgitb.enable(display=0)
try:
    # Code that may raise an exception
    result = 1 / 0
except:
    output = cgitb.text()

# Insert proper HTTP headers
output = "Content-Type: text/html\r\n\r\n" + output

# Print the modified output
print(output)