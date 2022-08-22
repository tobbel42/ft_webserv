#pragma once

#include <string>
#include <map>

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "typedefs.hpp"


enum	e_action { READ, WRITE };


/*
The connect class represents a single connection to a socket
with one dedicated fd.
It processes the data in the fd as follows:
	1. The available data is read and passed to the
		request class for parsing
	2. The executer class performs actions according to the
		request (see Executer.hpp)
	3. A response is generated and written into the fd
		through the response class
*/ 
class Connect
{
private: // typedefs

	typedef	Request								request_type;
	typedef Response							response_type;
	typedef std::map<std::string, std::string>	cookies;


public: // attribute
	cookies*				p_cookie_base;


public: // methods

	Connect(const Connect& other);
	Connect(fd_type fd, uint32_t ip, uint32_t port, cookies* cookie_base);
	~Connect();

	Connect& operator = (const Connect &rhs);

	fd_type					get_fd() const;
	uint32_t 				get_ip() const;
	uint32_t 				get_port() const;
	const Server*			get_server() const;
	const Server::Location*	get_location() const;
	e_action				get_action() const;
	std::string				get_hostname() const;

	void		set_server(const Server * server);
	void		set_location(const Server::Location* location);
	void		set_status(int status_code);

	void		find_location();
	#ifdef KQUEUE
	int32_t		read_request(s_kevent & kevent);
	int32_t		write_response(s_kevent & kevent);
	#else
	int32_t		read_request(s_pollfd & poll);
	int32_t		write_response(s_pollfd & poll);
	#endif
	void		compose_response();


private: // methods

	std::string find_dir(const std::string& name) const;
	bool	check_redirect();


private: // attributes

	fd_type					m_fd;
	uint32_t				m_ip;
	uint32_t				m_port;
	const Server*			p_server;
	const Server::Location*	p_location;
	e_action				m_action;
	int						m_status_code;
	request_type			m_req;
	response_type			m_res;


private:
	Connect();
};
