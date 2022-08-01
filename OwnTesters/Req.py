import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

REQUEST = "GET /test/echo.py?first_name=tobi HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n"
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect( (HOST, PORT) )
	s.sendall( REQUEST.encode() )
	data = s.recv(5000)


print(f"Received {data!r}")