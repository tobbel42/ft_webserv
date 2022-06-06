#include "Request.hpp"

Request::Request( void ):
	m_state(REQLINE) {
	#ifdef VERBOSE
		std::cout << "Request: Constructor called" << std::endl;
	#endif
}

Request::Request( const Request & cpy ) {
	#ifdef VERBOSE
		std::cout << "Request: Copy Constructor called" << std::endl;
	#endif
	*this = cpy;
}

Request::~Request() {
	#ifdef VERBOSE
		std::cout << "Request: Destructor called" << std::endl;
	#endif
}

Request & Request::operator=( const Request & rhs ) {
	#ifdef VERBOSE
		std::cout << "Request: Assignation operator called" << std::endl;
	#endif
	m_buffer = rhs.m_buffer;
	m_offset = rhs.m_offset;
	m_state = rhs.m_state;

	m_methode = rhs.m_methode;
	m_target = rhs.m_target;
	m_httpVer = rhs.m_httpVer;
	m_header = rhs.m_header;
	m_body = rhs.m_body;
	return (*this);
}

bool Request::isDone() {
	return true;
};


std::string Request::getNextReqLine() {
	std::string line;
	size_t pos =  m_buffer.find("\r\n", m_offset);
	if (pos == std::string::npos)
	{
		line = m_buffer.substr(m_offset);
		m_offset = m_buffer.size();
	}
	else
	{
		line = m_buffer.substr(m_offset, pos - m_offset);
		m_offset = pos + 2;
	}


	return line;
}

bool Request::parseRequestLine(const std::string & line){
	size_t pos = 0;

	pos = line.find(' ');
	std::cout << line << pos << "$$" << std::endl;
	m_methode = line.substr(0, pos);
	m_target = line.substr(pos + 1, line.find(' ', pos + 1) - pos - 1);
	pos = pos + 2 + m_target.size();
	m_httpVer = line.substr(pos);
	std::cout << m_methode << "$$" << m_target << "$$" << m_httpVer << "$$" << std::endl;

	return true;
}

bool Request::appendRead(const char *buf) {
	m_buffer.append(buf);

	// std::string key, value;
	try {
	std::string line = getNextReqLine();
	
	parseRequestLine(line);
	}
	catch (std::exception & e) { std::cout << "ERR\n" << buf << std::endl; printRequest();
		std::cout << e.what() << std::endl; } 
	// while (m_offset < m_buffer.size() && line.size() > 0)
	// {
	// 	try {
	// 	line = getNextReqLine();
	// 	key = line.substr(0, line.find(":"));
	// 	if (key == line)
	// 		break;
	// 	value = line.substr(key.size() + 2);
	// 	m_header.insert(std::make_pair(key, value));
	// 	}
	// catch (const std::exception & e) { std::cout << "WHile" << e.what() << std::endl;};
	// }
	// std::cout << m_buffer << m_buffer.size() << std::endl;

	m_buffer.clear();

	return isDone();
	
}

void Request::printRequest() {
	std::cout << m_methode << std::endl;
	std::cout << m_target << std::endl;
	std::cout << m_httpVer << std::endl;
	for (std::map<std::string, std::string>::iterator iter = m_header.begin();
		iter != m_header.end(); ++iter) {
		std::cout << (*iter).first << ": " << (*iter).second << std::endl;
	}
}