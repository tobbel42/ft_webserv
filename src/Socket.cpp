#include "Socket.hpp"

/*
//Careful son, ports 0-1024 are a big nono 
*/
Socket::Socket():
	m_ip(INADDR_ANY),
	m_port(80),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))//accept braucht den pointer 
{
	#ifdef VERBOSE
		std::cout << "Socket: Default Constructor called" << std::endl;
	#endif
}

Socket::Socket(unsigned int ip, unsigned int port):
	m_ip(ip),
	m_port(port),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))
{
	#ifdef VERBOSE
		std::cout << "Socket: Constructor called" << std::endl;
	#endif
	m_init();
}

Socket::Socket(const Socket& other):
	m_ip(),
	m_port(),
	m_address(),
	m_sockfd(),
	m_addLen(sizeof(m_address))
{
	#ifdef VERBOSE
		std::cout << "Socket: Copy Constructor called" << std::endl;
	#endif
	*this = other;
}

Socket::~Socket()
{
	#ifdef VERBOSE
		std::cout << "Socket: Destructor called" << std::endl;
	#endif
	//Socket closing moved to Engine Destructor for convenience
}

Socket&
Socket::operator=(const Socket& other)
{
	#ifdef VERBOSE
		std::cout << "Socket: Assignation operator called" << std::endl;
	#endif
	if (this != &other)
	{
		m_ip = other.m_ip;
		m_port = other.m_port;
		m_address = other.m_address;
		m_sockfd = other.m_sockfd;
		m_defaultServer = other.m_defaultServer;
	}
	return (*this);
}

void
Socket::m_init()
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

void
Socket::setDefaultServer( Server *server )
{
	m_defaultServer = server;
}

Server *
Socket::getServer( std::string hostname )
{
	//todo search m_Server for hostname

	return (m_defaultServer); 
}

fd_type
Socket::getSockFd( void ) const { return(m_sockfd); }

unsigned int
Socket::getIp () const { return m_ip; }

unsigned int
Socket::getPort () const { return m_port; }

bool
Socket::operator==( fd_type fd )
{
	return(m_sockfd == fd);
}

fd_type
Socket::acceptConnect( void )
{
	fd_type	fd = accept(
				m_sockfd,
				reinterpret_cast<sockaddr*>(&m_address),
				&m_addLen);
	return (fd);
}
