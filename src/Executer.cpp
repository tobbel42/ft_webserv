#include "Executer.hpp"


Executer::Executer(const Executer& other):
	p_server(other.p_server),
	p_loc(other.p_loc),
	m_status_code(other.m_status_code),
	m_req(other.m_req),
	m_content(other.m_content),
	m_filename(other.m_filename)
{}

Executer::Executer(const Server* server, const Server::Location* location,
					const Request& req):
	p_server(server),
	p_loc(location),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(req.get_target())
{}

Executer::Executer(const Server* server, const Server::Location* location,
					const Request& req, const std::string& filename):
	p_server(server),
	p_loc(location),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(filename)
{}


Executer::~Executer() {}

std::string
Executer::get_full_url() const
{
	//m_filename is modified to represent the file structure/path of the server
	//m_req.get_target() holds the filepath of the webserver
	return  m_req.get_host() + ":" +
			utils::to_string(m_req.get_port()) +
			m_req.get_target();
}

void
Executer::run()
{
	PRINT("EXEC FILENAME" << m_filename);
	if (p_server == nullptr)
	{
		EPRINT("no viable server found");
		m_status_code = 404;
		return;
	}
	else if (p_loc == nullptr)
	{
		run_server();
	}
	else
	{
		run_location();
	}
}

void
Executer::run_server()
{
	m_filename = utils::compr_slash(p_server->root + m_filename);
	e_FileType file_type = get_file_type();

	if (!utils::is_element_of(p_server->allowed_methods, m_req.get_methode()))
	{
		m_status_code = 405; // method not allowed
		return;
	}

	if (file_type == DIRECTORY && p_server->root == m_req.get_target())
	{
		m_filename = utils::compr_slash(m_filename + "/" + p_server->index);
		file_type = OTHER;
	}

	PRINT("in server:\n" << m_filename << " file type = " << file_type);
	switch (file_type)
	{
		case PHP:
		case PYTHON:
		case DIRECTORY:
			m_status_code = 403; // cgi and directory listing
			break; // are forbidden in server blocks
		case OTHER:
			read_from_file();
			break;
		default:
			break;
	}
}

void
Executer::run_location()
{
	// redirect the locations
	m_filename.replace(0, p_loc->prefix.size(), p_loc->root);

	m_filename = utils::compr_slash(p_server->root + m_filename);
	e_FileType file_type = get_file_type();

	if (!utils::is_element_of(p_loc->allowed_methods, m_req.get_methode()))
	{
		m_status_code = 405; // method not allowed
		return;
	}

	if (file_type == DIRECTORY && !p_loc->directory_listing_enabled &&
		p_loc->prefix == m_req.get_target()) // only append the root for exact matches
	{
		m_filename = utils::compr_slash(m_filename + "/" + p_loc->index);
		file_type = OTHER;
	}
	
	PRINT("in location: " << p_loc->root << '\n' << m_filename << " file type = " << file_type);

	switch (file_type)
	{
		case PHP:
		case PYTHON:
			if (cgi_is_allowed(p_loc->scripts, file_type))
				run_cgi(file_type);
			else
			{
				m_status_code = 403;
				// m_content = cgi not possible here
			}
			break;
		case DIRECTORY:
			run_directory_listing();
			break;
		case OTHER:
			read_from_file();
			break;

		default:
			break;
	}
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

void
Executer::run_directory_listing()
{
	DirectoryListing dir_list(m_filename, get_full_url());

	m_content = dir_list.run();
	m_status_code = dir_list.get_status_code();
}

void
Executer::run_cgi(e_FileType file_type)
{
	CGI cgi(m_filename, m_req);
	const ByteArr& req_body = m_req.get_body();
	std::string input(req_body.begin(), req_body.end());

	typedef std::map<std::string,std::string>::const_iterator MapIt;
	std::string executable;
	if (file_type == PYTHON)
	{
		MapIt it = p_loc->scripts.find("python");
		if (it != p_loc->scripts.end())
			executable = it->second;
		else
			executable = PYTHON_PATH;
	}
	else if (file_type == PHP)
	{
		MapIt it = p_loc->scripts.find("php");
		if (it != p_loc->scripts.end())
			executable = it->second;
		else
			executable = PHP_PATH;
	}

	m_content = cgi.run(file_type, input, executable);
	m_status_code = cgi.get_status_code();
}

e_FileType
Executer::get_file_type() const
{
	if (utils::is_dir(m_filename))
		return DIRECTORY;
	std::string extension = utils::get_file_ext(m_filename);
	if (extension == "py")
		return PYTHON;
	else if (extension == "php")
		return PHP;
	else
		return OTHER;
}

bool
Executer::cgi_is_allowed(const std::map<std::string,std::string>& scripts,
						e_FileType type)
{
	typedef std::map<std::string,std::string>::const_iterator	MapIt;

	for (MapIt it = scripts.begin(); it != scripts.end(); ++it)
	{
		if ((type == PYTHON && it->first == "python") ||
			(type == PHP && it->first == "php"))
			return true;
	}
	return false;
}
