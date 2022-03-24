/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 10:22:56 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/24 18:14:08 by tgrossma         ###   ########.fr       */
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

class Engine;

class Server
{
public:
	Server(const Server& other);
	Server(unsigned int ip, unsigned int port);
	~Server();
	
	Server& operator=(const Server& other);
	
	t_fd	getSockFd( void ) const;
	void	acceptConnect( Engine & engine );

	bool	operator==( t_fd fd );
	
	
private:
	Server();

	static bool				m_verbose;
	unsigned int			m_ip;
	unsigned int			m_port;
	sockaddr_in 			m_address;
	socklen_t				m_addLen;
	t_fd 					m_sockfd;
	std::vector<Connect>	m_connects;

	typedef std::vector<Connect>::iterator	CnctIter;

	void m_init();
	
};