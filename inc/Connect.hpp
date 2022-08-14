#pragma once

#include <string>
#include <map>

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "typedefs.hpp"


enum	e_action { READ, WRITE };


/*a connection represents a single connection
	-has a dedicated FD, to whose events it responds
	-has a specified action, whether its reading and writing
*/ 
class Connect
{
private: // typedefs

	typedef	Request								request_type;
	typedef Response							response_type;
	typedef std::map<std::string, std::string>	cookies;


public: // methods

	Connect(const Connect& other);
	Connect(fd_type fd, uint32_t ip, uint32_t port, cookies* cookie_base);
	~Connect();

	Connect& operator = (const Connect &rhs);

	fd_type					getFd() const;
	uint32_t 				getIp() const;
	uint32_t 				getPort() const;
	fd_type					getSockFd() const;
	const Server*			getServer() const;
	const Server::Location*	get_location() const;
	e_action				getAction() const;
	std::string				get_hostname() const;

	void		set_server(const Server * server);
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


private: // methods

	Connect();

	std::string find_dir(const std::string& name) const;


private: // attributes

	fd_type					m_fd;
	uint32_t				m_ip;
	uint32_t				m_port;
	const Server			*p_server;
	const Server::Location*	p_location;
	e_action				m_action;
	int						m_status_code;
	request_type			m_req;
	response_type			m_res;
	cookies*				p_cookie_base;
};
