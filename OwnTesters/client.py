import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

#REQUEST = "\r\nGET / HTTP/1.1\r\nlol: \t\t \r\n hello         \r\nlocal: \r\n\t\t   hihi   \r\nlocal: hehe \r\n\r\n"

# chunkReq = [ "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\nTransfer-Encoding: twer\r\n\r\n7\r\nMozilla\r\n"
# 	,"9\r\nDeveloper\r\n"
# 	,"7\r\nNetwork\r\n"
# 	,"0\r\n\r\n" ]

# with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
# 	s.connect((HOST, PORT))
# 	for c in chunkReq:
# 		print(c)
# 		s.send(c.encode());
# 		x = input()
# 	data = s.recv(1024)



with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect( (HOST, PORT) )
	s.sendall( REQUEST.encode() )
	data = s.recv(1024)


print(f"Received {data!r}")