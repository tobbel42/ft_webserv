#ifndef CONNECT_HPP
# define CONNECT_HPP

# include <iostream>
# include <utils.hpp>
# include <fstream>
# include <sstream>
# include <algorithm>

# include "Server.hpp"
# include "Engine.hpp"
# include "Request.hpp"
# include "Response.hpp"

enum	e_action { READ, WRITE };

struct s_kevent;

/*a connection represents a single connection
	-has a dedicated FD, to whose events it responds
	-has a specified action, whether its reading and writing
*/ 
class Connect
{
	private:

		// struct s_request {
		// 	std::string	methode;
		// 	std::string target;
		// 	std::string version;
		// 	std::map<std::string, std::string> header;
		// 	std::string body;
		// };
		// struct s_response {
		// 	std::string statusCode;
		// 	std::map<std::string, std::string> header;
		// 	std::string body;
		// };

		typedef	Request	request_type;
		typedef Response response_type;

		fd_type		m_fd;
		fd_type		m_sockFd;
		Server		*p_server;
		e_action	m_action;
		std::string	m_rawReq;
		std::string	m_response;

		request_type		m_req;
		response_type		m_res;
		
		Connect( void ); //we do not allow default construction
		
		
	public:

		Connect( fd_type fd, fd_type sockFd );
		~Connect();
		Connect( const Connect &copy );

		Connect	&operator = ( const Connect &rhs );

		fd_type		getFd( void ) const;
		fd_type		getSockFd( void ) const;
		Server *	getServer( void ) const;
		e_action	getAction( void ) const;	
	
		void		setServer( Server * server );
		bool		readRequest( s_kevent kevent );
		void		writeResponse( s_kevent kevent );
		void		composeResponse( void );
		
};

#endif
