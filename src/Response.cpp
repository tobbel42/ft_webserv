#include "Response.hpp"


std::map<int, const char*>
Response::s_status_codes(Response::s_init_status_codes());

std::map<std::string, const char*>
Response::s_mime_types(Response::s_init_mime_types());

Response::Response( void ) {
	#ifdef VERBOSE
		std::cout << "Response: Constructor called" << std::endl;
	#endif
}

Response::Response( const Response & cpy ) {
	#ifdef VERBOSE
		std::cout << "Response: Copy Constructor called" << std::endl;
	#endif
	*this = cpy;
}

Response::Response(int status_code, const std::string& body, Server* server):
	p_server(server),
	m_status_code(status_code),
	m_header(),
	m_body(body),
	m_payload()
{}

Response::~Response() {
	#ifdef VERBOSE
		std::cout << "Response: Destructor called" << std::endl;
	#endif
}

Response & Response::operator=( const Response & rhs ) {
	#ifdef VERBOSE
		std::cout << "Response: Assignation operator called" << std::endl;
	#endif
	if (this != &rhs)
	{
		p_server = rhs.p_server;
		m_status_code = rhs.m_status_code;
		m_header = rhs.m_header;
		m_body = rhs.m_body;
		m_payload = rhs.m_payload;
	}
	return (*this);
}

// general header:
//		Connection: (close or maybe keep-alive)
//		Date: the timestamp in http time format
std::string
Response::s_generate_general_header(int status_code)
{
	std::string header = "HTTP/";

	header += utils::to_string(HTTP_VERSION);
	header += ' ';
	header += utils::to_string(status_code);
	header += ' ';
	header += s_status_codes[status_code]; // creates new nodes for invalid status codes!
	header += "\r\n";
	header += "Date: ";
	header += utils::get_http_time();
	header += "\r\n";
	return header;
}


const char*
Response::s_get_mime_type(const std::string& filename)
{
	// find the last dot in the filename
	size_t pos = filename.find_last_of('.');
	if (pos == std::string::npos)
		return DEFAULT_MIME_TYPE;
	std::string file_extension = filename.substr(pos + 1);

	MimeIter mime_type = s_mime_types.find(file_extension);
	if (mime_type == s_mime_types.end()) // file extension is not in the list
		return DEFAULT_MIME_TYPE;

	return mime_type->second;
}


void
Response::set_server(Server* server) { p_server = server; }

void
Response::set_status_code(int status_code) { m_status_code = status_code; }

void
Response::set_body(const std::string& body) { m_body = body; }


std::string
Response::get_payload() const { return m_payload; }

size_t
Response::get_body_size() const { return m_body.size(); }

void
Response::m_add_header_line(const std::string& key, const std::string& value)
{
	m_header += key;
	m_header += ": ";
	m_header += value;
	m_header += "\r\n";
}

void
Response::m_add_to_payload(const std::string& to_add)
{
	m_payload += to_add;
	m_payload += "\r\n";
}

std::pair<std::string, size_t>
Response::generate_response()
{
	switch (m_status_code)
	{
	case 101:
		m_status_switching_protocols();
		break;
	case 200 ... 207:

		break;
	case 300 ... 308:

		break;
	default: // atm all error codes
		m_generate_error_response();
		break;
	}
	m_add_header_line("Server", "lil l and the beachboys 1.0");
	m_add_to_payload(m_header);
	m_payload += "\r\n";
	m_add_to_payload(m_body);
	return std::make_pair(m_payload, m_body.size());
}

void
Response::m_status_switching_protocols()
{
	m_header = s_generate_general_header(m_status_code);

	std::string supported_http = "HTTP/";
	supported_http += utils::to_string(HTTP_VERSION);
	m_add_header_line("Upgrade", supported_http);
}

void
Response::m_generate_error_response()
{
	std::string filename = p_server->error_pages;

	filename += '/';
	filename += utils::to_string(m_status_code);
	filename += ".html";

	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		// trying to use the default errror pages
		// if the provided ones don't work
		filename = DEFAULT_ERROR_PAGES;
		filename += '/';
		filename += utils::to_string(m_status_code);
		filename += ".html";
		file.open(filename.c_str());
		if (!file.is_open())
		{
			// if everything goes wrong just send 500
			char error_msg[] = "<html>\r\n<head>"
								"<title>internal server error</title></head>\r\n"
								"<body><h1>errorcode 500: internal server error</h1>"
								"</body></html>\r\n";

			m_status_code = 500;
			m_header = s_generate_general_header(m_status_code);
			m_add_header_line("Content-Length", utils::to_string(126));
			m_body.insert(m_body.end(), error_msg, error_msg + 126);
			return;
		}
	}
	m_header = s_generate_general_header(m_status_code);

	// Retry-after should be specified for all redirections and Service Unavailable
	// so we just hardcode it to 120 seconds
	if (m_status_code == 503 || (m_status_code >= 300 && m_status_code <= 307))
		m_add_header_line("Retry-after", utils::to_string(120));
	m_body = utils::read_file(file);
	
	m_add_header_line("Content-Length", utils::to_string(m_body.size()));
	m_add_header_line("Content-Location", filename);
	m_add_header_line("Content-Type", s_get_mime_type(filename));

	// response-header:
	//		Accept-ranges: (probably none)
	//		Retry-after: (maybe for 503 and 3XX status codes -> time in seconds)
	//		Server: webserv (or whatever name we want + version number)
	// entity-header:
	//		Allow: The allowed requests for the current server
	//		Content-Encoding: Must be provided when the coding is not "identity"
	//						If the coding is not accepted, status code 415 must be sent back
	//						If there are muliple encodings, the must be listed in order of usage
	//		Content-Length: The length of the response body. For chunked responses the field 
	//						should not be incuded (or included as 0)
	//		Content-Location: The path to the accessed resource. Optional.
	//		Content-MD5: Used for end-to-end encoding. Not neccessary.
	//		Content-Range: For multiple byte-ranges in multiple responses.
	//						Very complicated!
	//		Content-Type: The type of media sent in MIME format.
	//						Default is: "application/octet-stream"
	//		Expires: Date untill the response is valid and should be cached
	//						Dates in the past (or 0) count as instantly expired
	//		Last-Modified: Timestamp of the sent resource (e.g. file)
}


std::map<int, const char*>
Response::s_init_status_codes()
{
	std::map<int, const char*> status_codes;

	// 1XX: Informational
	status_codes.insert(std::make_pair(100, "Continue"));
	status_codes.insert(std::make_pair(101, "Switching Protocols"));

	// 2XX: Success
	status_codes.insert(std::make_pair(200, "OK"));
	status_codes.insert(std::make_pair(201, "Created"));
	status_codes.insert(std::make_pair(202, "Accepted"));
	status_codes.insert(std::make_pair(203, "Non-Authoritative Information"));
	status_codes.insert(std::make_pair(204, "No Content"));
	status_codes.insert(std::make_pair(205, "Reset Content"));
	status_codes.insert(std::make_pair(206, "Partial Content"));

	// 3XX: Redirection
	status_codes.insert(std::make_pair(300, "Multiple Choices"));
	status_codes.insert(std::make_pair(301, "Moved Permanently"));
	status_codes.insert(std::make_pair(302, "Found"));
	status_codes.insert(std::make_pair(303, "See Other"));
	status_codes.insert(std::make_pair(304, "Not Modified"));
	status_codes.insert(std::make_pair(305, "Use Proxy"));
	status_codes.insert(std::make_pair(306, "Switch Proxy")); // not used anymore
	status_codes.insert(std::make_pair(307, "Temporary Redirect"));

	// 4XX: Client Error
	status_codes.insert(std::make_pair(400, "Bad Request"));
	status_codes.insert(std::make_pair(401, "Unauthorized"));
	status_codes.insert(std::make_pair(402, "Payment Required")); // reserved for future use
	status_codes.insert(std::make_pair(403, "Forbidden"));
	status_codes.insert(std::make_pair(404, "Not Found"));
	status_codes.insert(std::make_pair(405, "Method Not Allowed"));
	status_codes.insert(std::make_pair(406, "Not Acceptable"));
	status_codes.insert(std::make_pair(407, "Proxy Authentication Required"));
	status_codes.insert(std::make_pair(408, "Request Time-out"));
	status_codes.insert(std::make_pair(409, "Conflict"));
	status_codes.insert(std::make_pair(410, "Gone"));
	status_codes.insert(std::make_pair(411, "Length Required"));
	status_codes.insert(std::make_pair(412, "Precondition Failed"));
	status_codes.insert(std::make_pair(413, "Request Entity Too Large"));
	status_codes.insert(std::make_pair(414, "Request-URI Too Large"));
	status_codes.insert(std::make_pair(415, "Unsupported Media Type"));
	status_codes.insert(std::make_pair(416, "Requested range not satisfiable"));
	status_codes.insert(std::make_pair(417, "Expectation Failed"));

	// 5XX: Server Error
	status_codes.insert(std::make_pair(500, "Internal Server Error"));
	status_codes.insert(std::make_pair(501, "Not Implemented"));
	status_codes.insert(std::make_pair(502, "Bad Gateway"));
	status_codes.insert(std::make_pair(503, "Service Unavailable"));
	status_codes.insert(std::make_pair(504, "Gateway Time-out"));
	status_codes.insert(std::make_pair(505, "HTTP Version not supported"));


	return status_codes;
}

std::map<std::string, const char*>
Response::s_init_mime_types()
{
	std::map<std::string, const char*> mime_types;

	// programming related
	mime_types.insert(std::make_pair("html", "text/html"));
	mime_types.insert(std::make_pair("htm", "text/html"));
	mime_types.insert(std::make_pair("shtml", "text/html"));
	mime_types.insert(std::make_pair("xml", "application/xml"));
	mime_types.insert(std::make_pair("css", "text/css"));
	mime_types.insert(std::make_pair("js", "text/javascript"));
	mime_types.insert(std::make_pair("json", "application/json"));
	mime_types.insert(std::make_pair("php", "application/x-httpd-php"));
	mime_types.insert(std::make_pair("phtml", "application/x-httpd-php"));
	mime_types.insert(std::make_pair("sql", "application/sql"));
	mime_types.insert(std::make_pair("py", "text/x-python"));
	mime_types.insert(std::make_pair("pyc", "application/x-python-code"));
	mime_types.insert(std::make_pair("pyo", "application/x-python-code"));
	mime_types.insert(std::make_pair("sh", "application/x-sh"));


	// images
	mime_types.insert(std::make_pair("png", "image/png"));
	mime_types.insert(std::make_pair("bmp", "image/bmp"));
	mime_types.insert(std::make_pair("ico", "image/x-icon"));
	mime_types.insert(std::make_pair("gif", "image/gif"));
	mime_types.insert(std::make_pair("jpg", "image/jpeg"));
	mime_types.insert(std::make_pair("jpeg", "image/jpeg"));


	// audio / video
	mime_types.insert(std::make_pair("wav", "audio/wav"));
	mime_types.insert(std::make_pair("mp3", "audio/mpeg"));
	mime_types.insert(std::make_pair("mp4", "audio/mp4"));


	// plain binary
	mime_types.insert(std::make_pair("exe", "application/octet-stream"));
	mime_types.insert(std::make_pair("bin", "application/octet-stream"));
	mime_types.insert(std::make_pair("txt", "text/plain"));


	// compression
	mime_types.insert(std::make_pair("zip", "application/zip"));
	mime_types.insert(std::make_pair("gzip", "application/gzip"));
	mime_types.insert(std::make_pair("gz", "application/gzip"));
	mime_types.insert(std::make_pair("pdf", "application/pdf"));
	mime_types.insert(std::make_pair("csv", "text/csv"));
	mime_types.insert(std::make_pair("tar", "application/x-tar"));
	mime_types.insert(std::make_pair("gz", "application/gzip"));

	return mime_types;
}
