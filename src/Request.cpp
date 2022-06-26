#include "Request.hpp"

/*Constructors----------------------------------------------------------------*/

Request::Request( void ):
m_offset(0),
m_state(HEADER),
m_done(0),
m_content_len(0),
m_err_code(0) {
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

Request & 
Request::operator=( const Request & rhs ) {
	#ifdef VERBOSE
		std::cout << "Request: Assignation operator called" << std::endl;
	#endif
	m_buffer = rhs.m_buffer;
	m_offset = rhs.m_offset;
	m_state = rhs.m_state;
	m_done = rhs.m_done;
	m_content_len = rhs.m_content_len;
	m_err_code = rhs.m_err_code;
	m_methode = rhs.m_methode;
	m_target = rhs.m_target;
	m_http_ver = rhs.m_http_ver;
	m_header = rhs.m_header;
	m_body = rhs.m_body;
	return (*this);
}

/*Getter----------------------------------------------------------------------*/

const std::string & 
Request::get_methode() const { return m_methode; }

const std::string & 
Request::get_target() const { return m_target; }

const std::string & 
Request::get_http_ver() const { return m_http_ver; }

//returns an empty string when field_name not found
std::string 
Request::get_header_entry(std::string field_name) const {
	utils::str_tolower(field_name);
	std::map<std::string,std::string>::const_iterator iter;
	iter = m_header.find(field_name);

	std::string field_value;

	if (iter != m_header.end())
		field_value = iter->second;
	return field_value;
}

const std::vector<char> &
Request::get_body() const { return m_body; }

uint32_t
Request::get_err_code() const { return m_err_code; }

/*Utils-----------------------------------------------------------------------*/

bool 
Request::is_done() {
	#ifdef VERBOSE
	print_request();
	#endif
	if (m_err_code != 0)
	{
		return true;
	}
	else if (m_done && get_header_entry("host") == "")
		m_err_code = 400;
	std::cout << "ERRCODE: " << m_err_code << std::endl;
	return m_done;
}

bool 
Request::check_invalid_char(const std::string & s, char *c, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (s.find(c[i]) != std::string::npos)
			return true;
	}
	return false;
}

bool
Request::set_error(size_t err_code) {
	m_err_code = err_code;
	return false;
}

/*RequestLineParsing----------------------------------------------------------*/

void
Request::get_next_req_line(std::string & line) {
	std::vector<char> CRLF;
	CRLF.push_back('\r');
	CRLF.push_back('\n');
	std::vector<char>::iterator pos = std::search(
		m_buffer.begin() + m_offset, m_buffer.end(),
		CRLF.begin(), CRLF.end()); 
	if (pos == m_buffer.end())
	{
		line = std::string(m_buffer.begin() + m_offset, pos);
		m_offset = m_buffer.size();
	}
	else
	{
		line = std::string(m_buffer.begin() + m_offset, pos);
		m_offset = (pos - m_buffer.begin()) + 2;
	}
}

bool
Request::parse_request_line(const std::string & line){
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
	m_http_ver = line.substr(pos + 1);

	return true;
}

void
Request::parse_target() {
	char c[2];
	c[1] = '\0' ;
	for (size_t i = 0; i < m_target.size(); ++i) {
		if (m_target[i] == '%' && i <= m_target.size() - 2) {
			std::string hexcode = m_target.substr(i + 1, 2);
			c[0] = utils::hex_str_to_i(hexcode);
			m_target.erase(i, 3);
			m_target.insert(i, c);
		}
	}
}

//is stupid, maybe overhaul
bool
Request::is_valid_request_line() {

	char c[4] = {'\t', '\r', '\n', ' '};

	if (check_invalid_char(m_methode, c, 4))
		return false;
	if (check_invalid_char(m_target, c, 4))
		return false;
	if (check_invalid_char(m_http_ver, c, 4))
		return false;
	return true;
}


//reading request line
bool
Request::parse_first_line() {
		std::string line;
		get_next_req_line(line);

		//skiping leading empty Lines
		while (line == "")
			get_next_req_line(line);

		if (parse_request_line(line) == false ||
			is_valid_request_line() == false)
				return set_error(400);
		parse_target();
		return true;
}

/*RequestHeaderParsing--------------------------------------------------------*/

//header entrys can span multible lines -> custom lineParser
void
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
		m_offset = m_buffer.size();
	else
		m_offset = (pos - m_buffer.begin()) + 2;
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
		get_next_header_line(line);
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
Request::parse_chunked_body() {
	std::string line;
	get_next_req_line(line);

	//getting the chunksize
	u_int32_t  chunk_size = utils::hex_str_to_i(line);

	//checking if the chunksize matches actual size

	if (m_offset + chunk_size + 2 > m_buffer.size()
		|| m_buffer[m_offset + chunk_size] != '\r' 
		|| m_buffer[m_offset + chunk_size + 1] != '\n')
		return set_error(400);

	//checking for last chunk

	if (chunk_size == 0)
	{
		m_done = true;
		return true;
	}

	//appending chunk to body

	m_body.insert(m_body.end(),
		m_buffer.begin() + m_offset, m_buffer.begin() + m_offset + chunk_size);
	m_offset += chunk_size + 2; // skipping CRLF

	//check if more there are more chunks in the buffer;

	return (m_offset >= m_buffer.size())?false:true;
}

bool
Request::is_chunked() {
	std::string value = get_header_entry("transfer-encoding");
	//print_request();
	if (value == "")
		return false;
	
	StringArr split = utils::str_split(value, ",");
	for (size_t i = 0; i < split.size(); ++i) {
		if (split[i] == "chunked")
			return true;
	}
	return false;
}

/*MainFunction----------------------------------------------------------------*/

//todo: what about illigal bodys

bool
Request::append_read(std::vector<char> buf) {

	m_buffer.insert(m_buffer.end(), buf.begin(), buf.end());

	if (m_state == HEADER)
	{
		//parsing of the header
		if (parse_first_line() == false)
			return is_done();
		if (parse_header() == false)
			return is_done();

		//determining if a Body is present, if yes, determining BodyType
		if (is_chunked())
			m_state = CHUNKED_BODY;
		else if (get_header_entry("content-length") != "")
		{
			m_state = BODY;
			std::string len = get_header_entry("content-length");
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
Request::print_request() {
	std::cout << m_methode << "##" << std::endl;
	std::cout << m_target << "##" << std::endl;
	std::cout << m_http_ver << "##" << std::endl;
	std::cout << "HEADER" << "##" << std::endl;
	for (std::map<std::string, std::string>::iterator iter = m_header.begin();
		iter != m_header.end(); ++iter) {
		std::cout << (*iter).first << ": " 
			<< (*iter).second << "##" << std::endl;
	}
	std::cout << "BODY" << std::endl;
	for (size_t i = 0; i < m_body.size(); ++i) {
		std::cout << m_body[i];
	}
}
