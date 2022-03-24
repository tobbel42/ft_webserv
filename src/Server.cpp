/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/17 14:41:27 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/24 18:12:16 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#ifdef VERBOSE
	bool Server::m_verbose = true;
#else
	bool Server::m_verbose = false;
#endif

/*
//Careful son, ports 0-1024 are a big nono 
*/
Server::Server():
	m_ip(INADDR_ANY),
	m_port(80),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))//accept braucht den pointer 
{
	if (m_verbose)
		std::cout << "Server: Default Constructor called" << std::endl;
}

Server::Server(unsigned int ip, unsigned int port):
	m_ip(ip),
	m_port(port),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))
{
	if (m_verbose)
		std::cout << "Server: Constructor called" << std::endl;
	m_init();
}

Server::Server(const Server& other):
	m_ip(other.m_ip),
	m_port(other.m_port),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))
{
	if (m_verbose)
		std::cout << "Server: Copy Constructor called" << std::endl;
	*this = other;
}

Server::~Server()
{
	if (m_verbose)
		std::cout << "Server: Destructor called" << std::endl;
	//server closing moved to Engine Destructor for convenience
	for (CnctIter iter = m_connects.begin(); iter != m_connects.end(); ++iter)
		close((*iter).getFd());
}

Server&
Server::operator=(const Server& other)
{
	if (m_verbose)
		std::cout << "Server: Assignation operator called" << std::endl;
	if (this != &other)
	{
		m_address = other.m_address;
		m_sockfd = other.m_sockfd;
	}
	return (*this);
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
	m_address.sin_addr.s_addr = htonl(m_ip);
	m_address.sin_port = htons(m_port);
	if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_address), sizeof m_address) == -1)
	{
		// throw
	}
	
}

t_fd
Server::getSockFd( void ) const
{
	return( m_sockfd );
}

bool
Server::operator==( t_fd fd )
{
	return(m_sockfd == fd);
}

void
Server::acceptConnect( Engine & engine )
{
	t_fd		fd = accept(
					m_sockfd,
					reinterpret_cast<sockaddr*>(&m_address),
					&m_addLen);

	Connect		newConnect(fd);

	m_connects.push_back(newConnect);
	engine.setRead(fd);
}