#include "Executer.hpp"


Executer::Executer(const Executer& other):
	p_server(other.p_server),
	p_env(other.p_env),
	m_status_code(other.m_status_code),
	m_req(other.m_req),
	m_content(other.m_content),
	m_filename(other.m_filename)
	// m_url(other.m_url)
{}

Executer::Executer(const Server* server, char** envp, const Request& req):
	p_server(server),
	p_env(envp),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(req.get_target())
{}

Executer::Executer(const Server* server, char** envp, const Request& req,
					const std::string& filename):
	p_server(server),
	p_env(envp),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(filename)
	// m_url(url)

{}


Executer::~Executer() {}

std::string
Executer::get_full_url() const
{
	//m_filename is modified to represent the file structure/path of the server
	//m_req.get_target() holds the filepath of the webserver
	return "http://" + m_req.get_host() + ":" +
			utils::to_string(m_req.get_port()) +
			m_req.get_target();
	// return "http://" + m_req.get_host() + ":" +
	// 		utils::to_string(m_req.get_port()) + "/" +
	// 		m_filename;
}

void
Executer::run()
{
	if (p_server == nullptr)
	{
		EPRINT("no viable server found");
		m_status_code = 404;
		return;
	}

	PRINT("EXEC FILENAME" << m_filename);

	const Server::Location* location = find_location();
	if (location == nullptr)
	{
		run_server();
	}
	else
	{
		

		run_location(location);
	}
}

void
Executer::run_server()
{
	m_filename = utils::compr_slash(p_server->root + m_filename);
	e_FileType file_type = get_file_type();

	if (file_type == DIRECTORY)
	{
		m_filename = utils::compr_slash(m_filename + p_server->index);
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
Executer::run_location(const Server::Location* p_loc)
{
	// redirect the locations
	m_filename.replace(0, p_loc->prefix.size(), p_loc->root);
	m_filename = utils::compr_slash(p_server->root + m_filename);
	e_FileType file_type = get_file_type();

	if (file_type == DIRECTORY && !p_loc->directory_listing_enabled)
	{
		m_filename = utils::compr_slash(m_filename + p_loc->index);
		file_type = OTHER;
	}
	
	PRINT("in location:\n" << m_filename << " file type = " << file_type);

	switch (file_type)
	{
		case PHP:
		case PYTHON:
			if (cgi_is_allowed(p_loc->allowed_scripts, file_type))
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
	MyDirectory dir(m_filename, get_full_url());
	m_content = dir.list_content();
	// TODO: pass the status code of the directory listing
	// m_status_code = dir.get_status_code();
}

void
Executer::run_cgi(e_FileType file_type)
{
	CGI cgi(m_filename, m_req, p_env);
	const ByteArr& req_body = m_req.get_body();
	std::string input(req_body.begin(), req_body.end());


	m_content = cgi.run(file_type, input);
	m_status_code = cgi.get_status_code();
}

e_FileType
Executer::get_file_type() const
{
	if (is_dir(m_filename))
		return DIRECTORY;
	std::string extension = utils::get_file_ext(m_filename);
	if (extension == "py")
		return PYTHON;
	else if (extension == "php")
		return PHP;
	else
		return OTHER;
}

const Server::Location*
Executer::find_location() const
{
	if (p_server == nullptr)
		return nullptr;
	const Server::Location* ret = nullptr;
	PRINT("filename = " << m_filename);
	std::string directory = utils::compr_slash(find_dir(m_filename));

	for (size_t i = 0; i < p_server->locations.size(); ++i)
	{
		const Server::Location* loc = &p_server->locations[i];
		PRINT("location = " << loc->prefix << " directory = " << directory);
		if (directory.compare(0, loc->prefix.size(), loc->prefix) == 0)
		{
			if (ret == nullptr || loc->prefix.size() > ret->prefix.size())
				ret = loc;
		}
	}
	return ret;
}


void
Executer::replace_dir(const Server::Location* loc)
{
	if (loc == nullptr)
		return;
	// std::string filename = m_filename.substr(loc->location.size());
	// m_filename = (loc->root + filename);
}

std::string
Executer::find_dir(const std::string& name) const
{
	if (name.empty())
		return "/";
	else if (is_dir(p_server->root + name))
	{
		PRINT("is a directory");
		return name + "/";
	}
	else
	{
		size_t pos = name.find_last_of('/');
		if (pos == std::string::npos)
			return "/"; // is this appropriate?
		return name.substr(0, pos);
	}
}

bool
Executer::is_dir(const std::string& name)
{
	struct stat dir;

	PRINT("path = " << name);

	if (stat(name.c_str(), &dir) == 0)
	{
		// if ((dir.st_mode & S_IFDIR) != 0)
		// 	return true;
		// else
		// 	return false;
		return dir.st_mode & S_IFDIR; // checks whether the DIR bit is set
	}
	else
		return false;
}

bool
Executer::cgi_is_allowed(const StringArr& scripts, e_FileType type)
{
	for (StringArr::const_iterator it = scripts.begin();
		it != scripts.end(); ++it)
	{
		if ((type == PYTHON && *it == "python") ||
			(type == PHP && *it == "php"))
			return true;
	}
	return false;
}