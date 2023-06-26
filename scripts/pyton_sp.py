#!/usr/bin/env python3

print("Content-type: text/html\n")

print('''
<html>
<head>
<style>
  /* CSS styles here */
</style>
</head>
<body>
  <div class="container">
    <div class="brand-logo"></div>
    <div class="brand-title">INFORMATIONS</div>
    <div class="inputs">
  ''')

import cgi

form = cgi.FieldStorage()

if form.getvalue('name') and form.getvalue('email'):
    name = form.getvalue('name')
    email = form.getvalue('email')
    print("<p>You have successfully submitted the form.</p>")
    print("<p>Name: {}</p>".format(name))
    print("<p>Email: {}</p>".format(email))
else:
    print('''
    <form action="zulima.jpeg" method="post">
      <label for="name">NAME</label>
      <input type="text" id="name" name="name" required>
      <label for="email">EMAIL</label>
      <input type="email" id="email" name="email" placeholder="example@test.com" required>
      <button type="submit" value="Submit">LOGIN</button>
    </form>
    ''')

print('''
    </div>
  </div>
</body>
</html>
''')
