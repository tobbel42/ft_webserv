#!/usr/bin/python

# Import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')
if (isinstance(first_name, list)):
	first_name = first_name[0];
if (isinstance(last_name, list)):
	last_name = last_name[0];

cookie_data = ""

if (first_name is not None):
	cookie_data += "first_name=" + first_name
if (last_name is not None):
	if (cookie_data != ""):
		cookie_data += "&"
	cookie_data += "last_name=" + last_name


print "Content-type: text/html\r\n"
print "Status: 200\r\n"
if (cookie_data != ""):
	print "Cookie_data: " + cookie_data + "\r\n"
print "\r\n"
print "<html>"
print "<head>"
print "<title>Hello - Second CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Hello %s %s</h2>" % (first_name, last_name)
print "</body>"
print "</html>"