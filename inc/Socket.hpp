/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 10:22:56 by skienzle          #+#    #+#             */
/*   Updated: 2022/05/30 17:51:27 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	
	fd_type	getSockFd( void ) const;
	fd_type	acceptConnect( void );
	Server	*getServer( std::string hostname );

	bool	operator==( fd_type fd );

	void	setDefaultServer( Server *server );
	
	
private:
	Socket();

	unsigned int			m_ip;
	unsigned int			m_port;
	sockaddr_in 			m_address;
	fd_type 					m_sockfd;
	socklen_t				m_addLen;

	Server							*m_defaultServer;
	std::map<std::string, Server *>	m_Servers;

	void m_init();
	
};