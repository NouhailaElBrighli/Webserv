#!/usr/bin/env python

import cgi

# Create an instance of FieldStorage for processing form data
form = cgi.FieldStorage()

# Retrieve the value entered in the 'name' field
name = form.getvalue('name')

# HTML content
print("<html>")
print("<head>")
print("<title>CGI Form Example</title>")
print("</head>")
print("<body>")
print("<h1>CGI Form Example</h1>")
print("<form method='post' action='file2.py'>")
print("<label for='name'>Name:</label>")
print("<input type='text' id='name' name='name' required>")
print("<input type='submit' value='Submit'>")
print("</form>")

# Display the entered name
if name:
    print("<p>Hello, {}!</p>".format(name))

print("</body>")
print("</html>")