import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

REQUEST = "\r\nGET  HTTP/1.1\r\nlol: hello         \r\nlocal: \r\n\t\t   hihi   \r\nlocal: hehe \r\n\r\n"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(REQUEST.encode())
    data = s.recv(1024)

print(f"Received {data!r}")