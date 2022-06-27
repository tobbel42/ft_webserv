#ifndef CONNECT_HPP
# define CONNECT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Request.hpp"
#include "Response.hpp"
#include "Engine.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "typedefs.hpp"
#include "../MyFile.hpp"

enum	e_action { READ, WRITE };

struct s_kevent;

/*a connection represents a single connection
	-has a dedicated FD, to whose events it responds
	-has a specified action, whether its reading and writing
*/ 
class Connect
{
	private:

		typedef	Request	request_type;
		typedef Response response_type;

		fd_type		m_fd;
		uint32_t	m_ip;
		uint32_t	m_port;
		Server		*p_server;
		e_action	m_action;
		int			m_status_code;
		request_type		m_req;
		response_type		m_res;
		
		Connect( void ); //we do not allow default construction
		
		
	public:

		Connect( fd_type fd, uint32_t ip, uint32_t port);
		~Connect();
		Connect( const Connect &copy );

		Connect	&operator = ( const Connect &rhs );

		fd_type		getFd( void ) const;

		uint32_t getIp() const;
		uint32_t getPort() const;

		fd_type		getSockFd( void ) const;
		Server *	getServer( void ) const;
		e_action	getAction( void ) const;
		std::string	get_hostname() const;
	
		void		setServer( Server * server );
		void		set_status(int status_code);
		bool		readRequest( s_kevent kevent );
		void		writeResponse( s_kevent kevent );
		void		composeResponse( void );
		
};

#endif
