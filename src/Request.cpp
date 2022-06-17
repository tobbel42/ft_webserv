#include "Request.hpp"


/*Constructors----------------------------------------------------------------*/

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

/*Getter----------------------------------------------------------------------*/

const std::string & Request::get_methode() const { return m_methode; }
const std::string & Request::get_target() const { return m_target; }
const std::string & Request::getHttpVer() const { return m_httpVer; }
std::string Request::getHeaderEntry(const std::string & fieldName) const {
	std::map<std::string,std::string>::const_iterator iter;
	iter = m_header.find(fieldName);

	std::string fieldValue;

	if (iter != m_header.end())
		fieldValue = iter->second;
	return fieldValue;
}
const std::string & Request::getBody() const { return m_body; }
uint32_t Request::get_errCode() const { return m_errCode; }

/*Utils-----------------------------------------------------------------------*/

bool Request::isDone() {
	printRequest();
	if (m_errCode != 0) {
		std::cout << "ERR: " << m_errCode << std::endl;
		return true;
	}
	return true;
}

bool Request::checkInvalidChar(const std::string & s, char *c, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (s.find(c[i]) != std::string::npos)
			return true;
	}
	return false;
}

/*RequestLineParsing----------------------------------------------------------*/

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

bool Request::isValidRequestLine() {

	char c[4] = {'\t', '\r', '\n', ' '};

	if (checkInvalidChar(m_methode, c, 4))
		std::cout << "M" << std::endl;
	if (checkInvalidChar(m_target, c, 4))
		std::cout << "t" << std::endl;
	if (checkInvalidChar(m_httpVer, c, 4))
		return false;
	return true;
}


//reading request line
bool Request::parseFirstLine() {
		std::string line;
		getNextReqLine(line);
		std::cout << "Hello " <<  line << std::endl;
		while (line == "")
			getNextReqLine(line);
		if (parseRequestLine(line) == false ||
			isValidRequestLine() == false) {
			m_errCode = 400;
			return false;
		}

		return true;
}

/*RequestHeaderParsing--------------------------------------------------------*/

void Request::getNextHeaderLine(std::string & line) {
	size_t pos =  m_buffer.find("\r\n", m_offset);
	while (pos != std::string::npos && isLWS(m_buffer, pos))
	{
		if (isWS(m_buffer, pos))
			pos += 1;
		else
			pos += 2;
		pos = m_buffer.find("\r\n", pos);
	}
	if (pos == std::string::npos)
	{
		line = m_buffer.substr(m_offset);
		m_offset = m_buffer.size();
		return ;
	}
	line = m_buffer.substr(m_offset, pos - m_offset);
	m_offset = pos + 2;
}

//reading request header
bool Request::parseHeader() {
	std::string key, value, line;
	size_t pos, pos2;

	while (m_offset < m_buffer.size())
	{
		getNextHeaderLine(line);
		pos = line.find(":"); 
		if (pos == std::string::npos)
		{
			if (line.size() > 0)
			{
				m_errCode = 400;
				return false;
			}
			else
				break;
		}
		key = line.substr(0, pos);

		++pos;
	
		//skipping LWS
		while(isLWS(line, pos))
		{
			if (isWS(line, pos))
				pos += 1;
			else
				pos += 2;
		}

		//check if value is empty
		if (pos >= line.size())
		{
			m_errCode = 400;
			return false;
		}

		//remove trailing LWS
		pos2 = line.size() - 1;
		while (isRLWS(line, pos2))
		{
			if (isWS(line, pos2))
				pos2 -= 1;
			else
				pos2 -= 2;
		}

		value = line.substr(pos, pos2 - pos + 1);
		
		//httpRequest Header fieldnames are case insenitive
		str_tolower(key);

		std::pair<std::map<std::string, std::string>::iterator, bool> i;
		i = m_header.insert(std::make_pair(key, value));

		//multible instances of the same fieldnames are combined into a csv list
		if (i.second == false)
		{
			i.first->second.append(",");
			i.first->second.append(value);
		}
	}
	return true;
}

/*RequestBodyParsing----------------------------------------------------------*/

bool Request::parseBody() {
		//TODO smartify
		m_body.append(m_buffer.substr(m_offset));
		std::string len = getHeaderEntry("content-length");
		size_t i = std::strtoll(len.data(), NULL, 10);
		if (m_body.size() < i)
			return false;
		return true;
}

bool Request::appendRead(const char *buf) {
	
	m_buffer.append(buf);

	if (m_state == HEADER)
	{
		if (parseFirstLine() == false)
			return isDone();
		if (parseHeader() == false)
			return isDone();
		std::cout << getHeaderEntry("content-length") << std::endl;
		if (getHeaderEntry("content-length") != "")
			m_state = BODY;
		printRequest();
	}

	if (m_state == BODY)
	{
		return parseBody();
	}
	else if (m_state == CHUNKED_BODY)
	{
		std::cerr << "NOT JET IMPLEMENTED" << std::endl;
	}

	std::cout << m_body.size() << std::endl;

	return isDone();
}

/*Debug-----------------------------------------------------------------------*/

void Request::printRequest() {
	std::cout << m_methode << "##" << std::endl;
	std::cout << m_target << "##" << std::endl;
	std::cout << m_httpVer << "##" << std::endl;
	std::cout << "HEADER" << "##" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = m_header.begin();
		iter != m_header.end(); ++iter) {
		std::cout << (*iter).first << ": " 
			<< (*iter).second << "##" << std::endl;
	}
}
