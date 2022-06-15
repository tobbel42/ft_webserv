#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <map>
#include <iostream>

class Response {

	public:

	Response();
	~Response();
	Response(const Response & cpy);
	Response & operator=(const Response & rhs);

	private:

	std::string 						m_buffer;	
	size_t								m_statusCode;
	std::string							m_httpVer;
	std::map<std::string, std::string>	m_header;
	std::string							m_body;

};

#endif