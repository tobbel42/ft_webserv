#include "Request.hpp"

#include <algorithm>

#include <cctype>

#include "utils.hpp"

/*Constructors----------------------------------------------------------------*/

Request::Request():
m_offset(0),
m_state(REQUEST_LINE),
m_done(0),
m_content_len(0),
m_reading_chunk_size(true),
m_err_code(0),
m_expectedPort(80)
{
	#ifdef VERBOSE
		PRINT("Request: Constructor called");
	#endif
}

Request::Request(uint32_t expectedPort):
m_offset(0),
m_state(REQUEST_LINE),
m_done(0),
m_content_len(0),
m_reading_chunk_size(true),
m_err_code(0),
m_expectedPort(expectedPort)
{
	#ifdef VERBOSE
		PRINT("Request: Constructor called");
	#endif
}

Request::Request(const Request & cpy)
{
	#ifdef VERBOSE
		PRINT("Request: Copy Constructor called");
	#endif
	*this = cpy;
}

Request::~Request()
{
	#ifdef VERBOSE
		PRINT("Request: Destructor called");
	#endif
}

Request & 
Request::operator=(const Request & rhs)
{
	#ifdef VERBOSE
		PRINT("Request: Assignation operator called");
	#endif

	if (this != &rhs)
	{
		m_buffer = rhs.m_buffer;
		m_offset = rhs.m_offset;
		m_state = rhs.m_state;
		m_done = rhs.m_done;
		m_content_len = rhs.m_content_len;
		m_reading_chunk_size = rhs.m_reading_chunk_size;
		m_err_code = rhs.m_err_code;
		m_method = rhs.m_method;
		m_uri = rhs.m_uri;

		m_host = rhs.m_host;
		m_expectedPort = rhs.m_expectedPort;
		m_port = rhs.m_port;
		m_target = rhs.m_target;
		m_query = rhs.m_query;

		m_http_ver = rhs.m_http_ver;
		m_header = rhs.m_header;
		m_body = rhs.m_body;
	}
	return *this;
}

/*Getter----------------------------------------------------------------------*/

const std::string & 
Request::get_method() const { return m_method; }

const std::string & 
Request::get_target() const { return m_target; }

const std::string &
Request::get_host() const { return m_host; }

uint32_t
Request::get_port() const { return m_port; }

const std::string & 
Request::get_http_ver() const { return m_http_ver; }

//returns an empty string when field_name not found
std::pair<bool, std::string>
Request::get_header_entry(std::string field_name) const
{
	utils::str_tolower(field_name);

	std::map<std::string,std::string>::const_iterator iter;
	iter = m_header.find(field_name);
	if (iter != m_header.end())
		return std::make_pair(true, iter->second);
	else
		return std::make_pair(false, "");
}

const ByteArr&
Request::get_body() const { return m_body; }

unsigned int
Request::get_err_code() const { return m_err_code; }

const std::string &
Request::get_query() const { return m_query; }

const std::map<std::string, std::string> &
Request::get_header() const { return m_header; }

/*Utils-----------------------------------------------------------------------*/

void
Request::set_host()
{
	std::pair<bool, std::string> hostField = get_header_entry("host");

	if (hostField.first == false)
		m_err_code = 400;
	else
		parse_uri(hostField.second);
	
	#ifdef VERBOSE
	PRINT("HOST: " << m_host);
	PRINT("PORT: " << m_port);
	PRINT("TARGET: " << m_target);
	PRINT("QUERY: " << m_query);
	#endif
}

bool 
Request::is_done()
{
	//deleting already parsed buffer
	m_buffer.erase(m_buffer.begin(), m_buffer.begin() + m_offset);
	m_offset = 0;
	#ifdef VERBOSE
	print_request();
	PRINT("ErrorCode: " << m_err_code);
	#endif
	if (m_err_code != 0)
		return true;
	else if (m_done)
		set_host();
	return m_done;
}

bool 
Request::check_invalid_char(const std::string & s, char *c, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		if (s.find(c[i]) != std::string::npos)
			return true;
	}
	return false;
}

bool
Request::set_error(size_t err_code)
{
	m_err_code = err_code;
	return false;
}

/*RequestLineParsing----------------------------------------------------------*/

void
Request::parse_uri(const std::string & host_field)
{
	std::string port;
	if (m_uri.substr(0, 7) == "http://")
	{
		size_t pos = m_uri.find(':', 7);
		size_t pos1 = m_uri.find('/', 7);
		size_t pos2 = m_uri.find('?', 7);

		m_host = m_uri.substr(7, std::min(pos, pos1) - 7);

		if (pos != std::string::npos)
			port = m_uri.substr(pos + 1, std::min(pos1, pos2) - pos - 1);
		if (pos1 != std::string::npos)
			m_target = m_uri.substr(pos1, pos2 - pos1);
		if (pos2 != std::string::npos)
			m_query = m_uri.substr(pos2 + 1);
	}
	else
	{
		size_t pos = host_field.find(':');
		m_host = host_field.substr(0, pos);
		if (pos != std::string::npos)
			port = host_field.substr(pos + 1);

		pos = m_uri.find('?');
		m_target = m_uri.substr(0, pos);
		if (pos != std::string::npos)
			m_query = m_uri.substr(pos + 1); 
	}
	if (port == "")
		m_port = 80;
	else
	{
		m_port = utils::from_string<uint32_t>(port);
		if (m_port != m_expectedPort)
			m_err_code = 400;
	}
}

bool
Request::get_next_req_line(std::string & line)
{
	ByteArr CRLF;
	CRLF.push_back('\r');
	CRLF.push_back('\n');
	ByteArr::iterator pos = std::search(
		m_buffer.begin() + m_offset, m_buffer.end(),
		CRLF.begin(), CRLF.end());
	if (pos == m_buffer.end())
		return false;
	else
	{
		line = std::string(m_buffer.begin() + m_offset, pos);
		m_offset = (pos - m_buffer.begin()) + 2;
		return true;
	}
}

bool
Request::parse_request_line(const std::string & line)
{
	size_t pos, pos1; 

	pos = line.find(' ');
	if (pos == std::string::npos)
		return false;
	m_method = line.substr(0, pos);
	pos1 = pos + 1;
	pos = line.find(' ', pos1);
	if (pos == std::string::npos)
		return false;
	m_uri = line.substr(pos1, pos - pos1);
	m_http_ver = line.substr(pos + 1);

	return true;
}

void
Request::parse_target()
{
	char c[2];
	c[1] = '\0' ;
	for (size_t i = 0; i < m_uri.size(); ++i)
	{
		if (m_uri[i] == '%' && i <= m_uri.size() - 2)
		{
			std::string hexcode = m_uri.substr(i + 1, 2);
			c[0] = utils::hex_str_to_i(hexcode);
			m_uri.erase(i, 3);
			m_uri.insert(i, c);
		}
	}
}

bool
Request::is_valid_http_ver()
{
	if (m_http_ver.substr(0, 5) != "HTTP/")
		return false;

	const char* ptr = m_http_ver.c_str() + 5;
	if (!isdigit(*ptr))
		return false;
	while(isdigit(*ptr))
		++ptr;
	if (*ptr != '.')
		return false;
	++ptr;
	if (!isdigit(*ptr))
		return false;
	while(isdigit(*ptr))
		++ptr;
	if (*ptr != '\0')\
		return false;
	return true;
}

//is stupid, maybe overhaul
bool
Request::is_valid_request_line() {

	if (!is_valid_http_ver())
		return false;

	char c[4] = {'\t', '\r', '\n', ' '};

	if (check_invalid_char(m_method, c, 4))
		return false;
	if (check_invalid_char(m_uri, c, 4))
		return false;
	if (check_invalid_char(m_http_ver, c, 4))
		return false;
	return true;
}


//reading request line
bool
Request::parse_first_line() {
		std::string line;
		bool done_read = get_next_req_line(line);

		if (!done_read)
			return false;

		//skiping leading empty Lines
		while (line == "" && done_read == true)
			done_read = get_next_req_line(line);

		if (!done_read)
			return false;

		if (parse_request_line(line) == false ||
			is_valid_request_line() == false)
				return set_error(400);
		parse_target();
		return true;
}

/*RequestHeaderParsing--------------------------------------------------------*/

//header entrys can span multible lines -> custom lineParser
bool
Request::get_next_header_line(std::string & line) {
	std::vector<char> CRLF;
	CRLF.push_back('\r');
	CRLF.push_back('\n');

	std::vector<char>::iterator pos = std::search(
		m_buffer.begin() + m_offset, m_buffer.end(),
		CRLF.begin(), CRLF.end());
	
	while (pos != m_buffer.end() &&
		utils::isLWS(m_buffer, (pos - m_buffer.begin())))
	{
		if (utils::isWS(m_buffer, (pos - m_buffer.begin())))
			pos += 1;
		else
			pos += 2;
		pos = std::search(
			pos, m_buffer.end(),
			CRLF.begin(), CRLF.end());
	}

	line = std::string(m_buffer.begin() + m_offset, pos);

	if (pos == m_buffer.end())
	{
		if (line != "")
			return false;
		m_offset = m_buffer.size();
	}
	else
		m_offset = (pos - m_buffer.begin()) + 2;
	return true;
}

size_t
Request::remove_leading_LWS(const std::string & line, size_t pos) {
	while(utils::isLWS(line, pos))
	{
		if (utils::isWS(line, pos))
			pos += 1;
		else
			pos += 2;
	}
	return pos;
}
size_t
Request::remove_trailing_LWS(const std::string & line) {
	size_t pos = line.size() - 1;
	while (utils::isRLWS(line, pos))
	{
		if (utils::isWS(line, pos))
			pos -= 1;
		else
			pos -= 2;
	}
	return pos;
}

//reading request header
bool
Request::parse_header() {
	std::string key, value, line;
	size_t pos;

	while (m_offset < m_buffer.size())
	{
		if (get_next_header_line(line) == false)
			return false;
		if (line == "")
			break;
		
		//parse the FieldName
		pos = line.find(":"); 
		if (pos == std::string::npos)
			return set_error(400);
		key = line.substr(0, pos);

		//parse the FieldValue
		pos = remove_leading_LWS(line, ++pos);

		//check for empty value
		if (pos >= line.size())
			return set_error(400);

		//Fieldvalue, with LWS trimmed
		value = line.substr(pos, remove_trailing_LWS(line) - pos + 1);
		
		//httpRequest Header fieldnames are case insenitive
		utils::str_tolower(key);

		//trying to insert the headerline
		std::pair<std::map<std::string, std::string>::iterator, bool> i;
		i = m_header.insert(std::make_pair(key, value));

		//multible instances of the same fieldnames are combined into a csv list
		if (i.second == false)
		{
			if (key == "host")
				return set_error(400);
			i.first->second.append("," + value);
		}
	}
	return true;
}

/*RequestBodyParsing----------------------------------------------------------*/

bool
Request::parse_body() {
		std::vector<char> vec(m_buffer.begin() + m_offset, m_buffer.end());
		m_body.insert(m_body.end(), vec.begin(), vec.end());
		m_offset += vec.size();
		if (m_body.size() < m_content_len)
			return false;
		return true;
}

bool
Request::parse_chunked_body(){
	std::string line;
	if (m_reading_chunk_size)
	{
		if (!get_next_req_line(line))
			return false;
		if (line == "")
			return true;
		m_chunk_size = utils::hex_str_to_i(line);
		PRINT("##" << line << "##" << "CHUNKSIZE:" << m_chunk_size);
		m_reading_chunk_size = false;
	}
	else
	{
		PRINT("CHUNKBODY" << m_chunk_size << " " << m_buffer.size() - m_offset);
		//check if the whole chunk has been read;
		if (m_chunk_size + 2 > m_buffer.size() - m_offset)
			return false;
		
		if (m_chunk_size == 0)
		{
			m_done = true;
			return false;
		}

		std::string chunk(m_buffer.begin() + m_offset, m_buffer.begin() + m_offset + m_chunk_size);

		m_body.insert(m_body.end(),
			m_buffer.begin() + m_offset, m_buffer.begin() + m_offset + m_chunk_size);
		PRINT("CHUNK:##" << chunk << "##");
		m_offset += m_chunk_size + 2;
		m_reading_chunk_size = true;
	}
	return true;
}

// bool
// Request::parse_chunked_body() {
// 	std::string line;
// 	bool done_line_read = get_next_req_line(line);

// 	if (done_line_read)
// 		return false;
	
// 	//getting the chunksize
// 	u_int32_t  chunk_size;
// 	if (line != "")
// 		chunk_size = utils::hex_str_to_i(line);
// 	else
// 		chunk_size = 0;

// 	//checking if the chunksize matches actual size
// 	if (m_offset + chunk_size + 2 > m_buffer.size()
// 		|| m_buffer[m_offset + chunk_size] != '\r' 
// 		|| m_buffer[m_offset + chunk_size + 1] != '\n')
// 		return set_error(400);

// 	//checking for last chunk

// 	if (chunk_size == 0)
// 	{
// 		m_done = true;
// 		return true;
// 	}
// 	//appending chunk to body

// 	m_body.insert(m_body.end(),
// 		m_buffer.begin() + m_offset, m_buffer.begin() + m_offset + chunk_size);
// 	m_offset += chunk_size + 2; // skipping CRLF

// 	//check if more there are more chunks in the buffer;

// 	return (m_offset >= m_buffer.size())?false:true;
// }

bool
Request::is_chunked() {

	std::pair<bool, std::string> value = get_header_entry("transfer-encoding");
	if (value.first == false)
		return false;

	StringArr split = utils::str_split(value.second, ",");
	for (size_t i = 0; i < split.size(); ++i) {
		if (split[i] == "chunked")
			return true;
	}
	return false;
}

/*MainFunction----------------------------------------------------------------*/

//todo: what about illigal bodys

bool
Request::append_read(ByteArr buf) {

	m_buffer.insert(m_buffer.end(), buf.begin(), buf.end());
	if (m_state == REQUEST_LINE)
	{
		//parsing of the header
		if (parse_first_line() == false)
			return is_done();
		m_state = HEADER;
	}
	if (m_state == HEADER)
	{
		if (parse_header() == false)
			return is_done();
		//determining if a Body is present, if yes, determining BodyType
		if (is_chunked()){
			m_state = CHUNKED_BODY;
		}
		else if (get_header_entry("content-length").first)
		{
			m_state = BODY;
			std::string len = get_header_entry("content-length").second;
			m_content_len = std::strtoll(len.data(), NULL, 10);
		}
		else
			m_done = true;
	}
	//Handling of BodyParsing
	if (m_state == BODY){
		m_done = parse_body();
	}
	else if (m_state == CHUNKED_BODY)
	{
		while (m_done != true)
		{
			if (parse_chunked_body() == false)
				break;
		}
	}

	#ifdef VERBOSE
	print_request();
	#endif

	return is_done();
}

/*Debug-----------------------------------------------------------------------*/

void
Request::print_request() const
{
	PRINT(m_method << "##");
	PRINT(m_uri << "##");
	PRINT(m_http_ver << "##");
	PRINT("HEADER" << "##");
	for (std::map<std::string, std::string>::const_iterator iter = m_header.begin();
		iter != m_header.end(); ++iter)
	{
		PRINT(iter->first << ": " << iter->second << "##");
	}
	// PRINT("BODY");
	// for (size_t i = 0; i < m_body.size(); ++i)
	// 	std::cout << m_body[i];
}

/*Setter----------------------------------------------------------------------*/

void
Request::decrypt_cookie(std::string & cookie_value)
{
	PRINT(m_query);
	if (m_query != "")
		m_query += "&";
	m_query += "cookie=true&";
	m_query += cookie_value;
}

void
Request::substitute_default_target(const std::string & serverDefault)
{
	if (m_target == "" || m_target == "/")
		m_target = serverDefault;
}
