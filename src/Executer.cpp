#include "Executer.hpp"

#include <algorithm>

#include <unistd.h>
#include <cstring>

#include "utils.hpp"
#include "CGI.hpp"
#include "DirectoryListing.hpp"

Executer::Executer(const Executer& other):
	p_server(other.p_server),
	p_loc(other.p_loc),
	m_status_code(other.m_status_code),
	m_req(other.m_req),
	m_content(other.m_content),
	m_filename(other.m_filename),
	m_is_cgi(other.m_is_cgi),
	m_cgi_header(other.m_cgi_header)
{}

Executer::Executer(const Server* server, const Server::Location* location,
					const Request& req):
	p_server(server),
	p_loc(location),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(req.get_target()),
	m_is_cgi(false),
	m_cgi_header()
{}

Executer::Executer(const Server* server, const Server::Location* location,
					const Request& req, const std::string& filename):
	p_server(server),
	p_loc(location),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(filename),
	m_is_cgi(false),
	m_cgi_header()
{}


Executer::~Executer() {}

const std::string&
Executer::get_content() const { return m_content; }

int
Executer::get_status_code() const {return m_status_code; }

const std::string&
Executer::get_filename() const { return m_filename; }

const StringMap&
Executer::get_cgi_header() const { return m_cgi_header; }

std::string
Executer::get_full_url() const
{
	//m_filename is modified to represent the file structure/path of the server
	//m_req.get_target() holds the filepath of the webserver
	return  m_req.get_host() + ":" +
			utils::to_string(m_req.get_port()) +
			m_req.get_target();
}

bool
Executer::is_cgi() const { return m_is_cgi; }

void
Executer::set_server(Server* server) { p_server = server; }

void
Executer::run()
{
	#ifdef VERBOSE
	PRINT("EXEC FILENAME " << m_filename);
	#endif
	if (p_server == nullptr)
	{
		EPRINT("no viable server found");
		m_status_code = 404;
		return;
	}

	if (p_loc == nullptr)
	{
		if (m_req.get_body().size() > p_server->max_client_body_size)
		{
			EPRINT("client body to large");
			m_status_code = 413;
			return;
		}
		run_server();
	}
	else
	{
		//we need to decide whats more important location or server max client body size
		if (m_req.get_body().size() > 
			(std::min(p_loc->max_client_body_size, p_server->max_client_body_size)))
		{
			EPRINT("client body to large");
			m_status_code = 413;
			return;
		}
		run_location();
	}
}

void
Executer::run_server()
{
	m_filename = utils::compr_slash(p_server->root + m_filename);
	e_FileType file_type = get_file_type();

	if (!utils::is_element_of(p_server->allowed_methods, m_req.get_method()))
	{
		m_status_code = 405; // method not allowed
		return;
	}

	// Apply local index
	if (file_type == DIRECTORY && 
		(m_req.get_target().empty() || m_req.get_target() == "/"))
	{
		m_filename = utils::compr_slash(m_filename + "/" + p_server->index);
		file_type = OTHER;
	}

	#ifdef VERBOSE
	PRINT("in server:\n" << m_filename << " file type = " << file_type);
	#endif


	//TODODO
	switch (file_type)
	{
		case CGI_SCRIPT:
		case DIRECTORY:
			m_status_code = 403; // cgi and directory listing
			break; // are forbidden in server blocks
		case OTHER:
			server_method_execution();
			break;
		default:
			break;
	}
}

void
Executer::server_method_execution()
{
	if (m_req.get_method() == "GET")
	{
		if (resource_exist())
			read_from_file();
		else
			m_status_code = 404;
	}
	else if (m_req.get_method() == "POST")
	{
		std::string content_type = m_req.get_header_entry("Content-Type").second;
		if (content_type == "text/plain")
			put_handler();
		else
			m_status_code = 404;
	}
	else if (m_req.get_method() == "PUT")
		put_handler();
	else if (m_req.get_method() == "DELETE")
		delete_handler();
}

void
Executer::run_location()
{
	// redirect the locations
	m_filename.replace(0, p_loc->prefix.size(), p_loc->root);

	m_filename = utils::compr_slash(p_server->root + m_filename);

	e_FileType file_type = get_file_type();

	if (!utils::is_element_of(p_loc->allowed_methods, m_req.get_method()))
	{
		m_status_code = 405; // method not allowed
		return;
	}
	// Apply local index 
	if (file_type == DIRECTORY && !p_loc->directory_listing_enabled)
	{
		m_filename = utils::compr_slash(m_filename + "/" + p_loc->index);
		file_type = get_file_type();
	}
	

	#ifdef VERBOSE
	PRINT("in location: " << p_loc->root << '\n' << m_filename << " file type = " << file_type);
	#endif

	if (m_req.get_method() == "GET")
		get_handler(file_type);
	else if (m_req.get_method() == "POST")
		post_handler(file_type);
	else if (m_req.get_method() == "PUT")
		put_handler();
	else if (m_req.get_method() == "DELETE")
		delete_handler();
}

void
Executer::get_handler(e_FileType file_type)
{
	if (resource_exist())
	{
		switch (file_type)
		{
		case CGI_SCRIPT:
				run_cgi(file_type);
			break;
		case DIRECTORY:
			if (p_loc->directory_listing_enabled)
				run_directory_listing();
			else
				m_status_code = 403;
			break;
		case OTHER:
			read_from_file();
			break;
		default:
			break;
		}
	}
	else
		m_status_code = 404;
}

void
Executer::post_handler(e_FileType file_type)
{
	if (file_type == CGI_SCRIPT)
		run_cgi(file_type);
	else
	{
		//here the file creation stuff is happening
		//only test/plain request are allowed to create files
		//the rest is getting 404 
		std::string content_type = m_req.get_header_entry("Content-Type").second;
		if (content_type == "text/plain")
			put_handler();
		else
			m_status_code = 404;
	}
}

void
Executer::put_handler()
{
	if (resource_exist())
		m_status_code = 204; // no content
	else
		m_status_code = 201; // created

	std::ofstream file(m_filename.c_str());
	if (file.is_open())
	{
		const ByteArr& payload = m_req.get_body();
		file.write(&(*payload.begin()), payload.size());
		if (!file.good())
			m_status_code = 500;
		file.close();
	}
	else
		m_status_code = 500;
}

void
Executer::delete_handler()
{
	if(!resource_exist())
	{
		m_status_code = 404;
		return ;
	}
	int status = std::remove(m_filename.c_str());
	if (status == 0)
		m_status_code = 204; // no content
	else
		m_status_code = 500;
}


void
Executer::run_cgi(e_FileType file_type)
{
	// find the executable of the cgi
	std::string extension = utils::get_file_ext(m_filename);
	std::string executable;
	StringMapIter it = p_loc->scripts.find(extension);

	if (it == p_loc->scripts.end())
	{
		m_status_code = 404;
		return;
	}
	else
		executable = it->second;

	m_is_cgi = true;

	CGI cgi(m_filename, m_req);

	// prepare the stdin of the cgi
	const ByteArr& req_body = m_req.get_body();
	std::string input(req_body.begin(), req_body.end());

	m_content = cgi.run(file_type, input, executable);

	m_cgi_header = cgi.get_cgi_header();

	// look for the status code of the CGI and use that if it's provided
	StringMapIter iter = m_cgi_header.find("Status");
	if (iter != m_cgi_header.end())
		m_status_code = utils::from_string<int>(iter->second);
	else
		m_status_code = cgi.get_status_code();
}

void
Executer::run_directory_listing()
{
	DirectoryListing dir_list(m_filename, get_full_url());

	m_content = dir_list.run();
	m_status_code = dir_list.get_status_code();
}

void
Executer::read_from_file()
{
	std::ifstream file(m_filename.c_str());

	if (file.is_open())
		m_content = utils::read_file(file, "\n");
	else
		m_status_code = 404;
}

bool
Executer::resource_exist() const
{
	int status = access(m_filename.c_str(), F_OK);

	if (status == 0)
		return true;
	EPRINT(strerror(errno));
	return false;
}

e_FileType
Executer::get_file_type() const
{
	if (utils::is_dir(m_filename))
		return DIRECTORY;
	std::string extension = utils::get_file_ext(m_filename);

	if (p_loc == nullptr)
		return OTHER;
	
	if (p_loc->scripts.find(extension) != p_loc->scripts.end())
		return CGI_SCRIPT;
	else
		return OTHER;
}
