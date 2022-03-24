/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 10:22:56 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/24 15:19:46 by tgrossma         ###   ########.fr       */
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

class Server
{
public:
	Server(const Server& other);
	Server(unsigned int ip, unsigned int port);
	~Server();
	
	Server& operator=(const Server& other);
	
	t_fd	getSockFd( void );

	
	
	
private:
	Server();

	unsigned int m_ip;
	unsigned int m_port;
	sockaddr_in m_address;
	int m_sockfd;
	void m_init();
	
};