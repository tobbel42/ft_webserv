#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#include "typedefs.hpp"

/*
//a Socket listens to an ip and port and accepts incomming connection
//maybe ToDo: assign a default Server to the port, to make the assignment of 
//incomming connections easier 
*/
class Socket
{
public:

	Socket(uint32_t ip, uint32_t port);
	Socket(const Socket& other);

	~Socket();

	Socket&	operator=(const Socket& other);
	bool	operator==(fd_type fd) const;

	fd_type		get_sock_fd() const;
	uint32_t	get_ip() const;
	uint32_t	get_port() const;

	bool	init();
	fd_type	accept_connect();
	
	
private:
	Socket();

	uint32_t		m_ip;
	uint32_t		m_port;
	sockaddr_in 	m_address;
	fd_type 		m_sockfd;
	socklen_t		m_add_len;

};
