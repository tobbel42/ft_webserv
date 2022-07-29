import cgi, fileinput, os, sys


try:
	parsed = cgi.FieldStorage(
		sys.stdin,
    	headers={'content-type': os.environ['CONTENT_TYPE']},
    	environ={'REQUEST_METHOD': 'POST'})
	fileitem = parsed['filename']

	# Test if the file was uploaded
	if fileitem.filename:
	# strip leading path from file name to avoid
	# directory traversal attacks
		fn = os.path.basename(fileitem.filename)
		open(os.environ['PATH_INFO'][0:os.environ['PATH_INFO'].rfind("/")] + "/upload/" + fn, 'wb').write(fileitem.file.read())
		message = 'The file "' + fn + '" was uploaded successfully'
	
	else:
		message = 'No file was uploaded'
except:
	message = 'No file was uploaded'
 
print("""\
Content-Type: text/html\n\n
<html>
<body>
   <p>%s</p>
</body>
</html>
""" % (message))