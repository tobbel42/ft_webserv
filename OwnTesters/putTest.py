import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

#Specify the file to be created or updated
target = "/test/uploads/hello_put.html"

#the content (maybe read from file if you feel fancy)
content = "<p>Hello Tobias<p>"

REQUEST = ("PUT " + target + " HTTP/1.1\r\n"
			"Host: 127.0.0.1:8080\r\n"
			"Content-type: text/html\r\n"
			"Content-length: " + str(len(content)) + "\r\n"
			"\r\n" + content
)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect( (HOST, PORT) )
	s.sendall( REQUEST.encode() )
	data = s.recv(5000)


print(f"Received {data!r}")