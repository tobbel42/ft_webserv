#!/usr/bin/python

# Import modules for CGI handling
import cgi, cgitb, os

# Create instance of FieldStorage
form = cgi.FieldStorage()

has_cookie = form.getvalue('cookie')

first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')
color = form.getvalue('color')

if (isinstance(first_name, list)):
	first_name = first_name[0];
if (isinstance(last_name, list)):
	last_name = last_name[0];
if (isinstance(color, list)):
	color = color[0];

cookie_data = ""

if (first_name is not None):
	cookie_data += "first_name=" + first_name
if (last_name is not None):
	if (cookie_data != ""):
		cookie_data += "&"
	cookie_data += "last_name=" + last_name
if (color is not None):
	if (cookie_data != ""):
		cookie_data += "&"
	cookie_data += "color=" + color




if (cookie_data == ""):
	print "Content-type: text/html\r\n"
	print "\r\n"
	print "<!DOCTYPE html>"
	print "<html>"
	print " <head>"
	print "		<meta charset=\"utf-8\"/>"
	print "		<title>Cookie Test</title>"
	print "		<style>"
	print "			* {"
	print "				font-family: \"Roboto\", sans-serif;"
	print "		</style>"
	print " </head>"
	print "	<body>"
	print "		<h2>Hello stranger please introduce yourself</h2>"
	print "		<div>"
	print "			<form action = \"/cookie_test/cookie_index.py\" method = \"get\">"
	print "				First Name: <input type = \"text\" name = \"first_name\" required>"
	print "				<br>"
	print "				<br>"
	print "				Last Name: <input type = \"text\" name = \"last_name\" required/>"
	print "				<br>"
	print "				<br>"
   	print "				Your favorite Color: <input type=\"color\" name = \"color\" id=\"color\" value=\"#12ddeb\">"
	print "				<br>"
	print "				<br>"
	print "				<input type = \"submit\" value = \"Submit\" />"
	print "			</form>"
	print "		</div>"
	print "	</body>"
	print "</html>"
else:
	if (has_cookie is None):
		print "Content-type: text/html\r\nStatus: 200\r\nCookie_data: " + cookie_data + "\r\n\r\n"
	else:
		print "Content-type: text/html\r\nStatus: 200\r\n\r\n"

	print "<!DOCTYPE html>"
	print "<html>"
	print " <head>"
	print "		<meta charset=\"utf-8\"/>"
	print "		<title>Cookie Test</title>"
	print "		<style>"
	print "			* {"
	print "				font-family: \"Roboto\", sans-serif;"
	print "				background: %s" % (color)
	print "			}"
	print "		</style>"
	print " </head>"
	print "	<body>"
	print "		<h2>Hello %s %s</h2>" % (first_name, last_name)
	print "		<button id=\"myButton1\" class=\"float-left submit-button\" >Reload</button>"
	print "		<script type=\"text/javascript\">"
	print "	    	document.getElementById(\"myButton1\").onclick = function () {"
	print "	        	location.href = \"/cookie_test/\";"
	print "	    	};"
	print "		</script>"
	print "		<button id=\"myButton2\" class=\"float-left submit-button\" >Back</button>"
	print "		<script type=\"text/javascript\">"
	print "	    	document.getElementById(\"myButton2\").onclick = function () {"
	print "	        	location.href = \"/\";"
	print "	    	};"
	print "		</script>"
	print "		</script>"
	print "		<button id=\"myButton3\" class=\"float-left submit-button\" >Delete Cookie</button>"
	print "		<script type=\"text/javascript\">"
	print "	    	document.getElementById(\"myButton3\").onclick = function () {"
	print "	        	document.cookie = document.cookie + \";expires=Thu 01 Jan 1970 00:00:01 GMT\";"
	print "				location.href = \"/cookie_test/\""
	print "	    	};"
	print "		</script>"
	print "	</body>"
	print "</html>"
