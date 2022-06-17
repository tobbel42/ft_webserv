
#pragma once

#include <map>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "Server.hpp"
#include "utils.hpp"
#include "typedefs.hpp"

#define DEFAULT_MIME_TYPE "application/octet-stream"

class Response
{
public: // methods
	Response();
	Response(const Response& other);
	Response(int status_code, Server* server);
	~Response();

	Response& operator=(const Response& other);


	void set_server(Server* server);

	std::string generate_response();

	std::string get_payload() const;
	static const char* s_get_mime_type(const std::string& filename);

private: // methods

	void GET_method();
	void POST_method();
	void DELETE_method();

	void m_generate_error_response(int error_code);
	void m_status_switching_protocols();

	void m_add_header_line(const std::string& key, const std::string& value);


	static std::map<int, const char*> s_init_status_codes();
	static std::map<std::string, const char*> s_init_mime_types();

	static std::string s_generate_general_header(int error_code);
private: // typedefs
	typedef void (Response::*method_ptr)();

	typedef std::map<std::string, const char*>::iterator	MimeIter;
	typedef std::map<int, const char*>::iterator			StatusIter;


private: // attributes
	std::string m_payload;

	Server* p_server;

	std::string 						m_buffer;
	int								m_status_code;
	std::string							m_httpVer;
	std::map<std::string, std::string>	m_header;
	std::string							m_body;


	static std::map<int, const char*> s_status_codes;
	static std::map<std::string, const char*> s_mime_types;
};
