#include "Response.hpp"

#include <fstream>

#include <ctime>
#include <unistd.h>

#include "utils.hpp"

//				Layout of http response headers:
// status line:
//	HTTP/[version] [status code] [reason phrase]
// general header:
//		Connection: (close or maybe keep-alive)
//		Date: the timestamp in http time format
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
//		Content-Range: For chunked responses in case.
//						Not implementable due to the constraints of the subject
//		Content-Type: The type of media sent in MIME format.
//						Default is: "application/octet-stream"
//		Expires: Date untill the response is valid and should be cached
//						Dates in the past (or 0) count as instantly expired
//		Last-Modified: Timestamp of the sent resource (e.g. file)

std::map<int, const char*>
Response::s_status_codes(Response::init_status_codes());

std::map<std::string, const char*>
Response::s_mime_types(Response::init_mime_types());

Response::Response():
	p_server(),
	p_loc(),
	p_request(),
	m_status_code(200),
	m_header(),
	m_body(),
	m_payload(),
	m_filename(),
	m_cookie(),
	m_content_location()
{
	#ifdef VERBOSE
		PRINT("Response: Constructor called");
	#endif
}

Response::Response(const Response& other):
	p_server(other.p_server),
	p_loc(other.p_loc),
	p_request(other.p_request),
	m_status_code(other.m_status_code),
	m_header(other.m_header),
	m_body(other.m_body),
	m_payload(other.m_payload),
	m_filename(other.m_filename),
	m_cookie(other.m_cookie),
	m_content_location(other.m_content_location)
{
	#ifdef VERBOSE
		PRINT("Response: Copy Constructor called");
	#endif
}

Response::Response(int status_code, const std::string& body,
					const Server* server, Request* request):
	p_server(server),
	p_loc(),
	p_request(request),
	m_status_code(status_code),
	m_header(),
	m_body(body),
	m_payload(),
	m_filename(),
	m_cookie(),
	m_content_location()
{}

Response::~Response()
{
	#ifdef VERBOSE
		PRINT("Response: Destructor called");
	#endif
}

Response & Response::operator=( const Response & rhs )
{
	#ifdef VERBOSE
		PRINT("Response: Assignation operator called");
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
		m_filename = rhs.m_filename;
		m_cookie = rhs.m_cookie;
		m_content_location = rhs.m_content_location;
	}
	return (*this);
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

void
Response::set_cookie(const std::string & cookie) { m_cookie = cookie; }

void
Response::set_content_location(const std::string & content_loc) {
	m_content_location = content_loc;
}

std::string
Response::get_payload() const { return m_payload; }

size_t
Response::get_body_size() const { return m_body.size(); }

#ifdef KQUEUE
int32_t
Response::send(const s_kevent& kevent)
{
	ssize_t i;
	i = write(kevent.ident, m_payload.c_str(), m_payload.size());
	if (i == -1 || i == 0)
		return RW_ERROR;
	else if (i !=  static_cast<int32_t>(m_payload.size()))
		return RW_CONTINUE;
	else
		return RW_DONE;
}
#else

int32_t
Response::send(const s_pollfd& poll)
{
	// if (p_server != nullptr)
	// {
	// 	if (p_server->max_client_body_size > m_body.size())
	// 		write(poll.fd, m_payload.c_str(), m_payload.size());
	// 	else
	// 	{
	// 		set_status_code(413); // request too large
	// 		generate();
	// 		write(poll.fd, m_payload.c_str(), m_payload.size());
	// 	}
	// }
	// else
	ssize_t i;
	i = write(poll.fd, m_payload.c_str(), m_payload.size());
	if (i == -1 || i == 0)
		return RW_ERROR;
	else if (i !=  static_cast<int32_t>(m_payload.size()))
		return RW_CONTINUE;
	else
		return RW_DONE;
}
#endif

std::pair<std::string, size_t>
Response::generate()
{
	switch (m_status_code)
	{
	case 100: // Just continue whatever you're doing
		init_header();
		break;
	case 101:
		switching_protocols();
		break;
	case 200 ... 206:
		success();
		break;
	case 300 ... 307:
		redirect();
		break;
	default: // 400 ... 417 && 500 ... 505
		error();
		break;
	}
	// add_to_head("Server", "lil l and the beachboys 1.0");
	add_to_payload(m_header);
	m_payload += m_body;
	return std::make_pair(m_payload, m_body.size());
}

void
Response::switching_protocols()
{
	init_header();

	std::string supported_http = "HTTP/";
	supported_http += utils::to_string(HTTP_VERSION);
	add_to_head("Upgrade", supported_http);
}


void
Response::success()
{
	init_header();

	if (p_loc != nullptr)
		add_to_head("Allow", utils::arr_to_csv(p_loc->allowed_methods, ", "));
	else if (p_server != nullptr)
		add_to_head("Allow", utils::arr_to_csv(p_server->allowed_methods, ", "));
	add_to_head("Content-Length", utils::to_string(m_body.size()));

	if (m_content_location != "")
		add_to_head("Content-Location", m_content_location);

	add_to_head("Content-Type", get_mime_type(m_filename));
	if (m_cookie != "")
		add_to_head("Set-Cookie", m_cookie);

	switch (m_status_code)
	{
	case 200:
		
		break;
	case 201: 
		// add_to_head("Content-Location", m_filename);
		break;
	case 202:
		break;
	
	default:
		break;
	}
	
}

void
Response::redirect()
{
	init_header();

	//add_to_head("Retry-after", utils::to_string(120));
	if (m_content_location != "")
	{
		add_to_head("Location", m_content_location);
		add_to_head("Cache-Control", "no-cache");
	}

}

void
Response::error()
{
	std::ifstream file;
	std::string filename;

	if (p_server != nullptr)
	{
		// attempt to use the error pages of the config file
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

			init_header();

			std::string reason = get_reason_phrase();
			m_body = "<html>\r\n<head>"
						"<title>" + reason +  "</title></head>\r\n"
						"<body><h1>errorcode " + utils::to_string(m_status_code) +
						": " + reason + "</h1> </body></html>\r\n";
			
			add_to_head("Content-Length", utils::to_string(m_body.size()));
			return;
		}
	}

	init_header();

	// Retry-after should be specified for Service Unavailable
	// so we just hardcode it to 120 seconds
	if (m_status_code == 503)
		add_to_head("Retry-after", utils::to_string(120));

	m_body = utils::read_file(file, "\r\n");
	
	if (p_loc != nullptr)
		add_to_head("Allow", utils::arr_to_csv(p_loc->allowed_methods, ", "));
	else if (p_server != nullptr)
		add_to_head("Allow", utils::arr_to_csv(p_server->allowed_methods, ", "));
	add_to_head("Content-Length", utils::to_string(m_body.size()));

	if (m_content_location != "")
		add_to_head("Content-Location", m_content_location);

	add_to_head("Content-Type", get_mime_type(filename));
}

void
Response::add_to_head(const std::string& key, const std::string& value)
{
	m_header += key;
	m_header += ": ";
	m_header += value;
	m_header += "\r\n";
}

void
Response::add_to_payload(const std::string& to_add)
{
	m_payload += to_add;
	m_payload += "\r\n";
}

void
Response::init_header(const std::string& header_lines)
{
	m_header = "HTTP/";
	m_header += utils::to_string(HTTP_VERSION);
	m_header += ' ';


	m_header += utils::to_string(m_status_code);
	m_header += ' ';
	m_header += get_reason_phrase();

	m_header += "\r\n";

	// add_to_head("Date", get_http_time());

	if (!header_lines.empty())
		m_header += header_lines;
}

const char*
Response::get_mime_type(const std::string& filename)
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

const char*
Response::get_reason_phrase() const
{
	StatusIter status_code = s_status_codes.find(m_status_code);
	if (status_code != s_status_codes.end())
		return status_code->second;
	else
		return s_status_codes[404];
}

std::string
Response::get_http_time()
{
	char buffer[1000];
	time_t unix_time = time(NULL);
	tm current_time = *gmtime(&unix_time);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M%S %Z", &current_time);
	return std::string(buffer);
}


std::map<int, const char*>
Response::init_status_codes()
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
Response::init_mime_types()
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
