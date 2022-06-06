#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <map>
#include <iostream>

enum e_reqState{
	REQLINE,
	HEADER,
	BODY
};

class Request {

	public:
	
	Request();
	~Request();
	Request(const Request & cpy);
	Request & operator=(const Request & rhs);

	private:

	std::string getNextReqLine();
	bool parseRequestLine(const std::string & line);

	public:

	std::string	m_buffer;
	size_t		m_offset;
	e_reqState	m_state;

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