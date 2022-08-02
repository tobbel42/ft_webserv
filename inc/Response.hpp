
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
	Response(int status_code, const std::string& body, const Server* server, Request* request);
	~Response();

	Response& operator=(const Response& other);


	void			set_server(const Server* server);
	void			set_location(const Server::Location* location);
	void			set_status_code(int status_code);
	void			set_body(const std::string& body);
	void			set_filename(const std::string& filename);
	void			set_cookie(const std::string& cookie);
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

	void switching_protocols();
	void success();
	void redirect();
	void error();

	void add_to_head(const std::string& key, const std::string& value);

	void add_to_payload(const std::string& to_add);

	void init_header(const std::string& header_lines = std::string());

	const char*	get_reason_phrase() const;

private: // typedefs

	typedef std::map<std::string, const char*>::iterator	MimeIter;
	typedef std::map<int, const char*>::iterator			StatusIter;


private: // attributes

	const Server*						p_server;
	const Server::Location*				p_loc;
	Request*							p_request;
	int									m_status_code;
	std::string							m_header;
	std::string							m_body;
	std::string							m_payload;
	std::string							m_filename;
	std::string							m_cookie;


private: // statics

	static const char*	get_mime_type(const std::string& filename);

	static std::map<int, const char*>			init_status_codes();
	static std::map<std::string, const char*>	init_mime_types();
	static std::map<int, const char*>			s_status_codes;
	static std::map<std::string, const char*>	s_mime_types;
};
