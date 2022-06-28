#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <utils.hpp>
#include <Engine.hpp>
#include <Connect.hpp>
#include <Server.hpp>

class Engine;

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
	Socket(unsigned int ip, unsigned int port);
	~Socket();
	Socket& operator=(const Socket& other);
	
	fd_type	getSockFd() const;
	fd_type	acceptConnect();

	bool	operator==( fd_type fd );

	unsigned int getIp() const;
	unsigned int getPort() const;
	
	bool m_init();
	
private:
	Socket();

	unsigned int				m_ip;
	unsigned int				m_port;
	sockaddr_in 			m_address;
	fd_type 				m_sockfd;
	socklen_t				m_addLen;

	
};