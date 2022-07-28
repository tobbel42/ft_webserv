from __future__ import unicode_literals
from __future__ import print_function

import cgi
try:
    from StringIO import StringIO as IO
except ImportError:
    from io import BytesIO as IO


body=""

import fileinput, os, sys




parsed = cgi.FieldStorage(
	sys.stdin,
	# headers=os.environ,
    headers={'content-type': os.environ['CONTENT_TYPE']},
    environ={'REQUEST_METHOD': 'POST'})



fileitem = parsed['filename']

# Test if the file was uploaded
if fileitem.filename:
   # strip leading path from file name to avoid
   # directory traversal attacks
   fn = os.path.basename(fileitem.filename)
   open(os.environ['SAFEDIR'] + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + fn + '" was uploaded successfully'
 
else:
	message = 'No file was uploaded'
 
print("""\
Content-Type: text/html\n\n
<html>
<body>
   <p>%s</p>
   <p>%s</p>
</body>
</html>
""" % (message, parsed))