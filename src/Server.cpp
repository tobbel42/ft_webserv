/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 14:41:27 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/24 15:49:55 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
//Careful son, ports 0-1024 are a big nono 
*/
Server::Server(): m_ip(INADDR_ANY), m_port(80), m_address(), m_sockfd() {
	std::cout << "Server default constructor" << std::endl;
}

Server::Server(unsigned int ip, unsigned int port): m_ip(ip), m_port(port),
	m_address(), m_sockfd()
{
	std::cout << "Constructor" << std::endl;
	m_init();
}

Server::Server(const Server& other): m_ip(other.m_ip), m_port(other.m_port),
	m_address(), m_sockfd()
{
	*this = other;
	std::cout << "test" << std::endl;
}

Server::~Server()
{
	std::cout << "Server destroyed" << std::endl;
	//close(m_sockfd);
}

Server&
Server::operator=(const Server& other)
{
	std::cout << "copy = " << std::endl;
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
	std::cout << m_sockfd << std::endl;
	if (m_sockfd < 0)
	{
		// throw
	}
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(m_ip);
	m_address.sin_port = htons(m_port);
	if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_address), sizeof m_address) == -1)
	{
		// throw
	}
	
}

t_fd
Server::getSockFd( void )
{
	std::cout << m_sockfd << std::endl;
	return( m_sockfd );
}