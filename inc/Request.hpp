#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <iostream>
#include <algorithm>
#include <cstring>
#include "utils.hpp"

enum e_req_state{
	REQUEST_LINE,
	HEADER,
	BODY,
	CHUNKED_BODY
};

//TODo WHat about empty header

//Data class which parses the Requests and exposens the elements to the User
//Detects Malformed requests
//trivia: LWS(linearwhiteSpace) = (' ' or '\t' or "\r\n" followed by ' ' or '\t'
class Request {

	public:
	
	/*Constructors------------------------------------------------------------*/

	Request();
	Request(uint32_t expectedPort);
	~Request();
	Request(const Request &);
	Request & operator=(const Request &);

	private:

	/*Utils-------------------------------------------------------------------*/

	bool check_invalid_char(const std::string &, char *, size_t);
	bool is_done();
	void set_host();
	bool set_error(size_t);

	/*RequestLineParsing------------------------------------------------------*/

	bool parse_first_line();
	void get_next_req_line(std::string &);
	bool parse_request_line(const std::string &);
	bool is_valid_request_line();

	void parse_target();
	bool is_valid_http_ver();

	void parse_uri(const std::string &);

	/*RequestHeaderParsing----------------------------------------------------*/

	bool get_next_header_line(std::string &);
	bool parse_header();
	size_t remove_leading_LWS(const std::string &, size_t);
	size_t remove_trailing_LWS(const std::string &);

	/*RequestBodyParsing------------------------------------------------------*/

	bool parse_body();
	bool is_chunked();
	bool parse_chunked_body();

	/*Internal MemberVariabels------------------------------------------------*/

	std::vector<char> m_buffer;
	size_t		m_offset;
	
	e_req_state	m_state;
	bool		m_done;

	size_t		m_content_len;

	/*DataMemberVariabels-----------------------------------------------------*/

	uint32_t	m_err_code;
	std::string m_method;
	std::string m_uri;

	std::string m_host;

	uint32_t	m_expectedPort;
	uint32_t	m_port;
	std::string m_target;
	std::string m_query;

	std::string m_http_ver;
	std::map<std::string, std::string> m_header; 
	std::vector<char> m_body;

	public:

	/*Public Functions--------------------------------------------------------*/

	bool	append_read(std::vector<char>);

	/*Debug only--------------------------------------------------------------*/	

	void	print_request() const;

	/*Getter------------------------------------------------------------------*/

	const std::string & get_method() const;
	const std::string & get_target() const;
	const std::string & get_host() const;
	uint32_t get_port() const;
	const std::string & get_query() const;
	const std::string & get_http_ver() const;
	std::pair<bool, std::string> get_header_entry(std::string) const;
	const std::vector<char> & get_body() const;
	uint32_t get_err_code() const;
	const std::map<std::string, std::string> & get_header() const;

	/*Setter------------------------------------------------------------------*/

	//todo implement
	void substitute_default_target(const std::string &);

};

#endif