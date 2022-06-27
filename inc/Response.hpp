
#pragma once

#include <map>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <unistd.h>

#include "Server.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "typedefs.hpp"

#define DEFAULT_MIME_TYPE "application/octet-stream"

class Response
{
public: // methods
	Response();
	Response(const Response& other);
	Response(int status_code, const std::string& body, Server* server, Request* request);
	~Response();

	Response& operator=(const Response& other);


	void			set_server(Server* server);
	void			set_status_code(int status_code);
	void			set_body(const std::string& body);
	std::string		get_payload() const;
	size_t			get_body_size() const;

	// first = payload, second = client body size
	std::pair<std::string, size_t> generate();

	#ifdef KQUEUE
	void							send(const s_kevent& kevent);
	#else
	void							send(const s_pollfd & poll);
	#endif

private: // methods

	void m_switching_protocols();
	void m_success();
	void m_redirect();
	void m_error();

	void m_add_to_head(const std::string& key, const std::string& value);

	void m_add_to_payload(const std::string& to_add);

	void m_init_header();

private: // typedefs
	typedef void (Response::*method_ptr)();

	typedef std::map<std::string, const char*>::iterator	MimeIter;
	typedef std::map<int, const char*>::iterator			StatusIter;


private: // attributes

	Server*								p_server;
	Request*							p_request;
	int									m_status_code;
	std::string							m_header;
	std::string							m_body;
	std::string							m_payload;


private: // statics

	static const char*	s_get_mime_type(const std::string& filename);

	static std::map<int, const char*>			s_init_status_codes();
	static std::map<std::string, const char*>	s_init_mime_types();
	static std::map<int, const char*>			s_status_codes;
	static std::map<std::string, const char*>	s_mime_types;
};
