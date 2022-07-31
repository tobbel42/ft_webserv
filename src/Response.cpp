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

Response::Response(int status_code, const std::string& body, const Server* server, Request* request):
	p_server(server),
	p_loc(),
	p_request(request),
	m_status_code(status_code),
	m_header(),
	m_body(body),
	m_payload(),
	m_filename()
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
		p_loc = rhs.p_loc;
		p_request = rhs.p_request;
		m_status_code = rhs.m_status_code;
		m_header = rhs.m_header;
		m_body = rhs.m_body;
		m_payload = rhs.m_payload;
	}
	return (*this);
}

void
Response::m_init_header()
{
	// status line:
	//	HTTP/[version] [status code] [reason phrase]
	m_header = "HTTP/";
	m_header += utils::to_string(HTTP_VERSION);
	m_header += ' ';


	m_header += utils::to_string(m_status_code);
	m_header += ' ';
	m_header += m_get_reason_phrase();

	m_header += "\r\n";

	// general header:
	//		Connection: (close or maybe keep-alive)
	//		Date: the timestamp in http time format
	m_add_to_head("Date", utils::get_http_time());
}


const char*
Response::s_get_mime_type(const std::string& filename)
{
	std::string extension = utils::get_file_ext(filename);
	if (extension.empty() ||						// directory listing and
		extension == "py" || extension == "php")	// CGI needs to be excluded
		return "text/html";

	MimeIter mime_type = s_mime_types.find(extension);
	if (mime_type == s_mime_types.end()) // file extension is not in the list
		return DEFAULT_MIME_TYPE;

	return mime_type->second;
}


void
Response::set_server(const Server* server) { p_server = server; }

void
Response::set_location(const Server::Location* location) { p_loc = location; }

void
Response::set_status_code(int status_code) { m_status_code = status_code; }

void
Response::set_body(const std::string& body) { m_body = body; }

void
Response::set_filename(const std::string& filename) { m_filename = filename; }


std::string
Response::get_payload() const { return m_payload; }

size_t
Response::get_body_size() const { return m_body.size(); }

#ifdef KQUEUE
void
Response::send(const s_kevent& kevent)
{
	if (p_server != nullptr)
	{
		if (p_server->max_client_body_size > m_body.size())
			write(kevent.ident, m_payload.c_str(), m_payload.size());
		else
		{
			set_status_code(413); // request too large
			m_payload.clear();
			generate();
			write(kevent.ident, m_payload.c_str(), m_payload.size());
		}
	}
	else
		write(kevent.ident, m_payload.c_str(), m_payload.size());
}
#else

void
Response::send(const s_pollfd& poll)
{
	if (p_server != nullptr)
	{
		if (p_server->max_client_body_size > m_body.size())
			write(poll.fd, m_payload.c_str(), m_payload.size());
		else
		{
			set_status_code(413); // request too large
			generate();
			write(poll.fd, m_payload.c_str(), m_payload.size());
		}
	}
	else
		write(poll.fd, m_payload.c_str(), m_payload.size());
}
#endif

std::pair<std::string, size_t>
Response::generate()
{
	switch (m_status_code)
	{
	case 100: // Just continue whatever you're doing
		m_init_header();
		break;
	case 101:
		m_switching_protocols();
		break;
	case 200 ... 206:
		m_success();
		break;
	case 300 ... 307:
		m_redirect();
		break;
	default: // 400 ... 417 && 500 ... 505
		m_error();
		break;
	}
	m_add_to_head("Server", "lil l and the beachboys 1.0");
	m_add_to_payload(m_header);
	m_payload += "\r\n";
	m_add_to_payload(m_body);
	return std::make_pair(m_payload, m_body.size());
}

void
Response::m_switching_protocols()
{
	m_init_header();

	std::string supported_http = "HTTP/";
	supported_http += utils::to_string(HTTP_VERSION);
	m_add_to_head("Upgrade", supported_http);
}


void
Response::m_success()
{
	m_init_header();

	if (p_loc != nullptr)
		m_add_to_head("Allow", utils::arr_to_csv(p_loc->allowed_methods, ", "));
	else if (p_server != nullptr)
		m_add_to_head("Allow", "GET");
	m_add_to_head("Content-Length", utils::to_string(m_body.size()));
	m_add_to_head("Content-Location", m_filename);
	m_add_to_head("Content-Type", s_get_mime_type(m_filename));

	switch (m_status_code)
	{
	case 200:
		
		break;
	case 201:

		break;
	case 202:
		break;
	
	default:
		break;
	}
	
}

void
Response::m_redirect()
{
	m_init_header();

	m_add_to_head("Retry-after", utils::to_string(120));
}

void
Response::m_error()
{
	std::ifstream file;
	std::string filename;

	if (p_server != nullptr)
	{
		filename = p_server->error_pages;

		filename += '/';
		filename += utils::to_string(m_status_code);
		filename += ".html";

		file.open(filename.c_str());
	}
	if (p_server == nullptr || !file.is_open())
	{
		// trying to use the default errror pages
		// if the provided ones don't work
		filename = DEFAULT_ERROR_PAGES;
		filename += '/';
		filename += utils::to_string(m_status_code);
		filename += ".html";
		std::cout << filename << std::endl;
		file.open(filename.c_str());
		if (!file.is_open())
		{
			// if no file can be found generate the status manually

			m_init_header();

			std::string reason = m_get_reason_phrase();
			m_body = "<html>\r\n<head>"
						"<title>" + reason +  "</title></head>\r\n"
						"<body><h1>errorcode " + utils::to_string(m_status_code) +
						": " + reason + "</h1> </body></html>\r\n";
			
			m_add_to_head("Content-Length", utils::to_string(m_body.size()));
			return;
		}
	}

	m_init_header();
	// Retry-after should be specified for Service Unavailable
	// so we just hardcode it to 120 seconds
	if (m_status_code == 503)
		m_add_to_head("Retry-after", utils::to_string(120));

	m_body = utils::read_file(file, "\r\n");
	
	if (p_loc != nullptr)
		m_add_to_head("Allow", utils::arr_to_csv(p_loc->allowed_methods, ", "));
	else if (p_server != nullptr)
		m_add_to_head("Allow", "GET");
	m_add_to_head("Content-Length", utils::to_string(m_body.size()));
	m_add_to_head("Content-Location", filename);
	m_add_to_head("Content-Type", s_get_mime_type(filename));

	// response-header:
	//		Accept-ranges: HTTP 1.1 is designed not to rely on those
	//		Retry-after: (maybe for 503 and 3XX status codes -> time in seconds)
	//		Server: lil l and the beachboys 1.0
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

void
Response::m_add_to_head(const std::string& key, const std::string& value)
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

const char*
Response::m_get_reason_phrase() const
{
	typedef std::map<int, const char*>::iterator Iter;

	Iter status_code = s_status_codes.find(m_status_code);
	if (status_code != s_status_codes.end())
		return status_code->second;
	else
		return s_status_codes[404];
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
	status_codes.insert(std::make_pair(418, "I'm a teapot"));

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
