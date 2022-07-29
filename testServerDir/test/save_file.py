import cgi, fileinput, os, sys

try:
	parsed = cgi.FieldStorage()
	fileitem = parsed['filename']
	# Test if the file was uploaded
	if fileitem.filename:
		# strip leading path from file name to avoid
		# directory traversal attacks
		fn = os.path.basename(fileitem.filename)
		path = os.environ['PATH_INFO'][0:os.environ['PATH_INFO'].rfind("/")]
		path += "/uploads/"
		path += fn
		if (os.path.exists(path)):
			message = 'File already exists'
		else:
			open(path, 'wb').write(fileitem.file.read())
			message = 'The file "' + fn + '" was uploaded successfully'

	else:
		message = 'No file was uploaded'
except:
	message = 'No file was uploaded'

print("""\
Content-Type: text/html\r\n\r\n
<html>
<body>
   <p>%s</p>
</body>
</html>
""" % (message))