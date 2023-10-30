#!/usr/bin/env python

import sys

def parse_post_data():
    post_data = sys.stdin.read()
    data_pairs = post_data.split('&')
    parsed_data = {}
    for pair in data_pairs:
        key, value = pair.split('=')
        parsed_data[key] = value
    return parsed_data

received_data = parse_post_data()

print("Content-Type: text/html\r\n\r\n")
print("<html>")
print("<head><title>Received Data</title></head>")
print("<body>")
print("<h1>Received Data:</h1>")
for key, value in received_data.items():
    print("<p>{0}: {1}</p>".format(key, value))
print("</body>")
print("</html>")
