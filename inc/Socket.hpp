#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <utils.hpp>
#include <Connect.hpp>
#include <Server.hpp>

/*
//a Socket listens to an ip and port and accepts incomming connection
//ToDo: has to be created from the config file
//maybe ToDo: assign a default Server to the port, to make the assignment of 
//incomming connections easier 
*/
class Socket
{
public:
	Socket(const Socket& other);
	Socket(uint32_t ip, uint32_t port);
	~Socket();
	Socket& operator=(const Socket& other);
	
	fd_type	getSockFd() const;
	fd_type	acceptConnect();

	bool	operator==( fd_type fd );

	uint32_t getIp() const;
	uint32_t getPort() const;
	
	bool m_init();
	
private:
	Socket();

	uint32_t				m_ip;
	uint32_t				m_port;
	sockaddr_in 			m_address;
	fd_type 				m_sockfd;
	socklen_t				m_addLen;

	
};