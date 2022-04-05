#include "Server.hpp"

Server::Server( void )
{
	#ifdef VERBOSE
		std::cout << "Server: Default Constructor called" << std::endl;
	#endif
}

Server::Server( t_fd socket, std::string hostname, std::string directory):
m_socket(socket),
m_hostname(hostname),
m_directory(directory)
{
	#ifdef VERBOSE
		std::cout << "Server: Constructor called" << std::endl;
	#endif
}

Server::~Server( void )
{
	#ifdef VERBOSE
		std::cout << "Server: Destructor called" << std::endl;
	#endif
}

Server::Server( const Server & cpy )
{
	#ifdef VERBOSE
		std::cout << "Server: Copy Constructor called" << std::endl;
	#endif
	*this = cpy;
}

Server &
Server::operator=( const Server &rhs )
{
	#ifdef VERBOSE
		std::cout << "Server: Assignation operator called" << std::endl;
	#endif
	m_socket = rhs.getSocket();
	m_hostname = rhs.getHostname();
	m_directory = rhs.getDirectory();
	return (*this);
}

std::string
Server::getDirectory( void ) const
{
	return m_directory;
}

t_fd
Server::getSocket( void ) const
{
	return m_socket;
}

std::string
Server::getHostname( void ) const
{
	return m_hostname;
}