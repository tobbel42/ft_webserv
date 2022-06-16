
#pragma once

#include <map>
#include <string>
#include <utility>
#include <fstream>

#include "Server.hpp"
#include "utils.hpp"
#include "typedefs.hpp"


class Response
{
public: // methods
	Response();
	Response(const Response& other);
	Response(Server* server);
	~Response();

	Response& operator=(const Response& other);


	void set_server(Server* server);

	std::string get_payload() const;

private: // methods

	void GET_method();
	void POST_method();
	void DELETE_method();

	void m_generate_error_response(int error_code);

	void m_add_header_line(const std::string& key, const std::string& value);


	static std::map<int, std::string> s_init_status_codes();
	static std::string s_generate_status_line(int error_code);
private: // typedefs
	typedef void (Response::*method_ptr)();



private: // attributes
	std::string m_payload;

	Server* p_server;



	static std::map<int, std::string> s_status_codes;
};