
#pragma once

#include <map>
#include <string>
#include <utility>
#include <fstream>

#include "Server.hpp"
#include "utils.hpp"

#define HTTP_VERSION 1.1


class Response
{
public: // methods
	Response();
	Response(const Response& other);
	~Response();

	Response& operator=(const Response& other);


	void set_server(Server* server);

	std::string get_payload() const;

private: // methods

	void GET_method();
	void POST_method();
	void DELETE_method();

	void m_generate_error_response(int error_code);


	static std::map<int, std::string> s_init_status_codes();
	static std::string s_generate_status_line(int error_code);
private: // typedefs
	typedef void (Response::*method_ptr)();



private: // attributes
	std::string m_payload;

	Server* p_server;



	static std::map<int, std::string> s_status_codes;
};