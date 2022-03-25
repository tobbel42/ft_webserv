/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 10:22:56 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/25 11:57:16 by tgrossma         ###   ########.fr       */
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

class Socket
{
public:
	Socket(const Socket& other);
	Socket(unsigned int ip, unsigned int port);
	~Socket();
	
	Socket& operator=(const Socket& other);
	
	t_fd	getSockFd( void ) const;
	void	acceptConnect( Engine & engine );

	bool	operator==( t_fd fd );
	
	
private:
	Socket();

	static bool				s_verbose;
	unsigned int			m_ip;
	unsigned int			m_port;
	sockaddr_in 			m_address;
	t_fd 					m_sockfd;
	socklen_t				m_addLen;
	std::vector<Connect>	m_connects;

	typedef std::vector<Connect>::iterator	CnctIter;

	void m_init();
	
};