#ifndef CONNECT_HPP
# define CONNECT_HPP

# include <iostream>
# include "utils.hpp"
# include "Server.hpp"
# include "Engine.hpp"
# include <fstream>
# include <sstream>

enum	e_action { READ, WRITE };

struct s_kevent;

/*a connection represents a single connection
	-has a dedicated FD, to whose events it responds
	-has a specified action, whether its reading and writing
*/ 
class Connect
{
	private:

		t_fd			m_fd;
		unsigned int 	m_ip;
		unsigned int 	m_port;
		//t_fd			m_sockFd;
		Server			*p_server;
		e_action		m_action;
		std::string		m_request;
		std::string		m_response;
		
		Connect( void );
		
	public:

		Connect( t_fd fd, unsigned int ip, unsigned int port);
		~Connect();
		Connect( const Connect &copy );

		Connect	&operator = ( const Connect &rhs );

		t_fd		getFd( void ) const;

		unsigned int getIp() const;
		unsigned int getPort() const;

		t_fd		getSockFd( void ) const;
		Server *	getServer( void ) const;
		e_action	getAction( void ) const;	
		
		void		setServer( Server * server );
		void		readRequest( s_kevent kevent );
		void		writeResponse( s_kevent kevent );
		void		composeResponse( void );
		
};

#endif
