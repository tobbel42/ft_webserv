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

#include "Executer.hpp"

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
		const Server		*p_server;
		const Server::Location* p_location;
		e_action	m_action;
		int			m_status_code;
		request_type		m_req;
		response_type		m_res;
		
		Connect( void ); //we do not allow default construction
		
		
	public:

		Connect(fd_type fd, uint32_t ip, uint32_t port);
		~Connect();
		Connect(const Connect &copy);

		Connect	&operator = (const Connect &rhs);

		fd_type		getFd() const;

		uint32_t 	getIp() const;
		uint32_t 	getPort() const;

		fd_type		getSockFd() const;
		const Server *	getServer() const;
		const Server::Location* get_location() const;
		e_action	getAction() const;
		std::string	get_hostname() const;
	
		void		setServer(const Server * server);
		void		set_location(const Server::Location* location);
		void		set_status(int status_code);
		void		find_location();
		#ifdef KQUEUE
		bool		readRequest(s_kevent & kevent);
		void		writeResponse(s_kevent & kevent);
		#else
		bool		readRequest(s_pollfd & poll);
		void		writeResponse(s_pollfd & poll);
		#endif
		void		composeResponse();

private:
	std::string find_dir(const std::string& name) const;
};

#endif
