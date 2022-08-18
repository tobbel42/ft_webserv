
#pragma once

#include <map>
#include <string>
#include <utility>

#include "Server.hpp"
#include "Request.hpp"
#include "typedefs.hpp"

#define DEFAULT_MIME_TYPE "application/octet-stream"

/*
The response class creates a HTTP response according to the status code
that was provided.
After generating the payload it may be sent to a client using the send
method.
*/
class Response
{
public: // methods
	Response();
	Response(const Response& other);
	Response(int status_code, const std::string& body, const Server* server, Request* request);
	~Response();

	Response& operator=(const Response& other);

	std::string		get_payload() const;
	size_t			get_body_size() const;

	void			set_server(const Server* server);
	void			set_location(const Server::Location* location);
	void			set_content_location(const std::string& content_loc);
	void			set_status_code(int status_code);
	void			set_body(const std::string& body);
	void			set_filename(const std::string& filename);
	void			set_cookie(const std::string& cookie);

	// first = payload, second = client body size
	std::pair<std::string, size_t> generate();

	#ifdef KQUEUE
	int32_t							send(const s_kevent& kevent);
	#else
	int32_t							send(const s_pollfd& poll);
	#endif


private: // methods

	void		switching_protocols();
	void		success();
	void		redirect();
	void		error();

	void		add_to_head(const std::string& key, const std::string& value);

	void		add_to_payload(const std::string& to_add);

	void		init_header(const std::string& header_lines = std::string());

	const char*	get_reason_phrase() const;


private: // typedefs

	typedef std::map<std::string, const char*>::iterator	MimeIter;
	typedef std::map<int, const char*>::iterator			StatusIter;


private: // attributes

	const Server*				p_server;
	const Server::Location*		p_loc;
	Request*					p_request;
	int							m_status_code;
	std::string					m_header;
	std::string					m_body;
	std::string					m_payload;
	std::string					m_filename;
	std::string					m_cookie;
	std::string					m_content_location;


private: // statics

	static std::string	get_http_time();
	static const char*	get_mime_type(const std::string& filename);

	static std::map<int, const char*>			init_status_codes();
	static std::map<std::string, const char*>	init_mime_types();
	static std::map<int, const char*>			s_status_codes;
	static std::map<std::string, const char*>	s_mime_types;
};
