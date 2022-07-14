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

Executer::Executer(Server* server, char** envp, const Request& req):
	p_server(server),
	p_env(envp),
	m_status_code(200),
	m_req(req),
	m_content(),
	m_filename(req.get_target() + "/")
{}

Executer::Executer(Server* server, char** envp, const Request& req,
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
	return "http://" + m_req.get_host() + ":" +
			utils::to_string(m_req.get_port()) +
			m_req.get_target();
}

void
Executer::run()
{
	Server::Location* location = find_location();
	if (location == nullptr)
	{
		EPRINT("no viable location found");
		m_status_code = 404;
		return;
	}
	m_filename.replace(0, location->location.size(), location->root);
	e_FileType file_type = get_file_type();
	switch (file_type)
	{
		case PHP:
		case PYTHON:
			if (cgi_is_allowed(location->allowed_scripts, file_type))
			{
				// execute cgi

			}
			else
			{
				m_status_code = 403;
				// m_content = cgi not possible here
			}
			break;
		case DIRECTORY:
			if (location->directory_listing_enabled)
			{
				// run directory listing

			}
			else
			{
				m_status_code = 403; // request forbidden
				// m_content = directory listing not allowed here
			}
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

Server::Location*
Executer::find_location() const
{

	if (p_server == nullptr)
		return nullptr;
	std::string directory = find_dir(m_filename);
	for (size_t i = 0; i < p_server->locations.size(); ++i)
	{
		Server::Location* loc = &p_server->locations[i];
		if (directory.compare(0, loc->location.size(), loc->location) == 0)
			return loc;
	}
	return nullptr;
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
Executer::find_dir(const std::string& name)
{
	if (is_dir(name))
		return name;
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

	if (stat(name.c_str(), &dir) == 0)
		return dir.st_mode & S_IFDIR; // checks whether the DIR bit is set
	else
		return false;
}

bool
Executer::cgi_is_allowed(const StringArr& scripts, e_FileType type)
{
	for (StringArr::const_iterator it = scripts.begin(); it != scripts.end(); ++it)
	{
		if ((type == PYTHON && *it == "python") ||
			(type == PHP && *it == "php"))
			return true;
	}
	return false;
}