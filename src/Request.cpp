#include "Request.hpp"

Request::Request( void ):
	m_state(HEADER) {
	m_offset = 0;
	m_errCode = 0;
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

const std::string & Request::get_methode() const { return m_methode; }
const std::string & Request::get_target() const { return m_target; }
const std::string & Request::getHttpVer() const { return m_httpVer; }
std::string Request::getHeaderEntry(const std::string & fieldName) const {
	std::map<std::string,std::string>::iterator iter = m_header.find(fieldName);

	std::string fieldValue;

	if (iter != m_header.end())
		fieldValue = iter->second;
	return fieldValue;
}
const std::string & Request::getBody() const { return m_body; }
uint32_t Request::get_errCode() const { return m_errCode; }

bool Request::isDone() {
	return true;
};


void Request::getNextReqLine(std::string & line) {
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
}

bool Request::parseRequestLine(const std::string & line){
	size_t pos, pos1; 

	pos = line.find(' ');
	if (pos == std::string::npos)
		return false;
	m_methode = line.substr(0, pos);
	pos1 = pos + 1;
	pos = line.find(' ', pos1);
	if (pos == std::string::npos)
		return false;
	m_target = line.substr(pos1, pos - pos1);
	m_httpVer = line.substr(pos + 1);

	return true;
}

bool Request::appendRead(const char *buf) {
	m_buffer.append(buf);

	if (m_state == HEADER)
	{

		//reading request line
		std::string line;
		getNextReqLine(line);
		if (parseRequestLine(line) == false) {
			m_errCode = 400;
			return true;
		}

		#ifdef VERBOSE
		printRequest();
		#endif

		//reading request header
		std::string key;
		std::string value;
		size_t pos;
		while (m_offset < m_buffer.size() && line.size() > 0)
		{
			getNextReqLine(line);
			pos = line.find(":");
			if (pos == std::string::npos)
			{
				if (line.size() > 0)
				{
					m_errCode = 400;
					return true;
				}
				else
					break;
			}
			key = line.substr(0, pos);
			if (pos + 2 > line.size())
			{
				m_errCode = 400;
				return true;
			}
			value = line.substr(pos + 2);

			//httpRequest are case insenitive, standardizing the input
			str_tolower(key);
			str_tolower(value);

			m_header.insert(std::make_pair(key, value));
		}

		if (m_header.find("content-length") != m_header.end())
			m_state = BODY;
		else if (m_header.find("content-lenght") != m_header.end())
			m_state = CHUNKED_BODY;

		//#ifdef VERBOSE
		printRequest();
		//#endif

	}

	if (m_state == BODY)
	{
		//TODO smartify
		std::cout << "BODY" << std::endl;
		m_body.append(m_buffer.substr(m_offset));
		std::string len = m_header.find("content-length")->second;
		size_t i = std::strtoll(len.data(), NULL, 10);
		if (m_body.size() < i)
			return false;
		std::cout << m_body << std::endl;
		return true;
	}
	else if (m_state == CHUNKED_BODY)
	{
		std::cerr << "NOT JET IMPLEMENTED" << std::endl;
	}


	return isDone();
	
}

void Request::printRequest() {
	std::cout << m_methode << std::endl;
	std::cout << m_target << std::endl;
	std::cout << m_httpVer << std::endl;
	std::cout << "HEADER" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = m_header.begin();
		iter != m_header.end(); ++iter) {
		std::cout << (*iter).first << ": " << (*iter).second << std::endl;
	}
}
