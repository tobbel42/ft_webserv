/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/13 10:22:56 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/18 10:33:31 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <csignal>




class Server
{
public:
	Server(const Server& other);
	Server(unsigned int ip, unsigned int port);
	~Server();
	
	Server& operator=(const Server& other);
	
	
	
	
private:
	Server();
	void m_init();

	unsigned int m_ip;
	unsigned int m_port;
	sockaddr_in m_address;
	int m_sockfd;
	
};