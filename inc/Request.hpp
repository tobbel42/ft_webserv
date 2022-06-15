#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <iostream>
#include "utils.hpp"

enum e_reqState{
	HEADER,
	BODY,
	CHUNKED_BODY
};

//TODo WHat about empty header

//Data class which takes the request as a string and parses it.
//Detects Malformed requests
class Request {

	public:
	
	Request();
	~Request();
	Request(const Request & cpy);
	Request & operator=(const Request & rhs);

	private:

	void getNextReqLine(std::string & line);
	bool parseRequestLine(const std::string & line);

	public:

	std::string	m_buffer;
	size_t		m_offset;
	e_reqState	m_state;
	uint32_t	m_errCode;

	std::string m_methode;
	std::string m_target;
	std::string m_httpVer;
	std::map<std::string, std::string> m_header; 
	std::string	m_body;

	public:

	bool	appendRead(const char *buf);
	bool	isDone();
	void	printRequest();
};

#endif