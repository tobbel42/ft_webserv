#from flask import Flask

#app = Flask(__name__)

#@app.route('/')
#def index():
#    return 'Web App with Python Flask!'


import fileinput

for line in fileinput.input():
	print(line)

print '<html>'
print '<head>'
print '<title>Hello World - First CGI Program</title>'
print '</head>'
print '<body>'
print '<h2>Hello World! This is my first CGI program</h2>'
print '</body>'
print '</html>'
