

#include "Response.hpp"


std::map<int, std::string> Response::s_status_codes(Response::s_init_status_codes());

std::map<int, std::string>
Response::s_init_status_codes()
{
	std::map<int, std::string> status_codes;

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

std::string
Response::s_generate_status_line(int error_code)
{
	std::string status_line = "HTTP/";

	status_line += utils::to_string(HTTP_VERSION);
	status_line += ' ';
	status_line += utils::to_string(error_code);
	status_line += ' ';
	status_line += s_status_codes[error_code]; // creates new nodes for invalid status codes!
	status_line += "\r\n";
	return status_line;
}





void
Response::set_server(Server* server) { p_server = server; }


std::string
Response::get_payload() const { return m_payload; }


void
Response::m_add_header_line(const std::string& key, const std::string& value)
{
	m_payload += key;
	m_payload += ": ";
	m_payload += value;
	m_payload += "\r\n";
}



void
Response::m_generate_error_response(int error_code)
{
	std::string filename = "testServerDir"; // p_server->get_error_pages();

	filename += '/';
	filename += utils::to_string(error_code);
	filename += ".html";

	std::ifstream file(filename.c_str());
	if (file.is_open())
	{
		m_payload = s_generate_status_line(error_code);
		// general header:
		//		Connection: (close or maybe keep-alive)
		//		Date: the timestamp in http time format
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
	else
	{
		// error 501: error file is not where it should be
		m_payload = s_generate_status_line(501);
	}
}