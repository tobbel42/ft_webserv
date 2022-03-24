/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 14:41:27 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/18 10:33:46 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): m_ip(INADDR_ANY), m_port(80), m_address(), m_sockfd() {}

Server::Server(unsigned int ip, unsigned int port): m_ip(ip), m_port(port),
	m_address(), m_sockfd()
{
	m_init();
}

Server::Server(const Server& other): m_ip(other.m_ip), m_port(other.m_port),
	m_address(), m_sockfd()
{
	
}

Server::~Server()
{
	close(m_sockfd);
}

Server&
Server::operator=(const Server& other)
{
	if (this != &other)
	{
		m_address = other.m_address;
		m_sockfd = other.m_sockfd;
	}
	return *this;
}

void
Server::m_init()
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd < 0)
	{
		// throw
	}
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = m_ip;
	m_address.sin_port = htons(m_port);
	if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_address), sizeof m_address) == -1)
	{
		// throw
	}
	
}