

#include "Server.hpp"


Server::Server():
	server_names(),
	root("/"),
	index("index.html"),
	error_pages(DEFAULT_ERROR_PAGES),
	ip_address(),
	ports(),
	max_client_body_size(std::numeric_limits<unsigned int>::max()),
	allowed_methods(),
	locations(),
	m_checks(7, false)
{
	// reserving memory beforehand to avoid copies
	server_names.reserve(3);
	ports.reserve(5);
	allowed_methods.reserve(3);
	locations.reserve(3);
}

Server::Server(const Server& other):
	server_names(other.server_names),
	root(other.root),
	index(other.index),
	error_pages(other.error_pages),
	ip_address(other.ip_address),
	ports(other.ports),
	max_client_body_size(other.max_client_body_size),
	allowed_methods(other.allowed_methods),
	locations(other.locations),
	m_checks(other.m_checks)
{}

Server&
Server::operator=(const Server& other)
{
	if (this != &other)
	{
		server_names = other.server_names;
		root = other.root;
		index = other.index;
		error_pages = other.error_pages;
		ip_address = other.ip_address;
		ports = other.ports;
		max_client_body_size = other.max_client_body_size;
		allowed_methods = other.allowed_methods;
		locations = other.locations;
		m_checks = other.m_checks;
	}
	return *this;
}


bool
Server::set_server_name(const std::string& name)
{
	if (std::find(server_names.begin(), server_names.end(), name)
		== server_names.end())
		server_names.push_back(name);
	return true;
}

bool
Server::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = word;
	return true;
}

bool
Server::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = word;
	return true;
}

bool
Server::set_error_pages(const std::string& word)
{
	if (m_checks[ERROR_PAGES])
		return false;
	m_checks[ERROR_PAGES] = true;
	error_pages = word;
	return true;
}

bool
Server::set_ip_address(unsigned int ip)
{
	if (m_checks[IP_ADDRESS])
		return false;
	m_checks[IP_ADDRESS] = true;
	ip_address = ip;
	return true;
}

bool
Server::set_port(unsigned int port)
{
	ports.push_back(port);
	return true;
}

bool
Server::set_method(const std::string& method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
	{
		if (std::find(allowed_methods.begin(), allowed_methods.end(), method)
			== allowed_methods.end())
			allowed_methods.push_back(method);
		return true;
	}
	else
		return false;
}

bool
Server::set_max_client_body_size(unsigned int n)
{
	if (m_checks[MAX_CLIENT_BODY_SIZE])
		return false;
	m_checks[MAX_CLIENT_BODY_SIZE] = true;
	max_client_body_size = n;
	return true;
}


const char*
Server::check_attributes() const
{
	if (!m_checks[ROOT])
		return "every server must have a root directory";
	else if (!m_checks[INDEX])
		return "every server must have a index file";
	else if (!m_checks[IP_ADDRESS])
		return "every server must have an ip address";
	else if (ports.empty())
		return "at least one port must be specified for each server";
	else
		return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Server& rhs)
{
	out << "server names: ";
	for (size_t i = 0; i < rhs.server_names.size(); ++i)
		out << rhs.server_names[i] << ", ";
	out << "\nroot: " << rhs.root << "\nerror pages: " << rhs.error_pages
		<< "\nip address: " << rhs.ip_address << "\nmax client body size: "
		<< rhs.max_client_body_size << "\nports:" << std::endl;
	for (size_t i = 0; i < rhs.ports.size(); ++i)
		out << rhs.ports[i] << ", ";
	out << "\nallowed methods: ";
	for (size_t i = 0; i < rhs.allowed_methods.size(); ++i)
		out << rhs.allowed_methods[i] << ", ";
	out << std::endl;
	for (size_t i = 0; i < rhs.locations.size(); ++i)
		out << rhs.locations[i] << std::endl;
	return out;
}



//		Location

Server::Location::Location():
	location(),
	root("/"),
	index("index.html"),
	allowed_scripts(),
	directory_listing_enabled(false),
	m_checks(5, false)
{}

Server::Location::Location(const Location& other):
	location(other.location),
	root(other.root),
	index(other.index),
	allowed_scripts(other.allowed_scripts),
	directory_listing_enabled(other.directory_listing_enabled),
	m_checks(other.m_checks)
{}

Server::Location&
Server::Location::operator=(const Location& other)
{
	if (this != &other)
	{
		location = other.location;
		root = other.root;
		index = other.index;
		allowed_scripts = other.allowed_scripts;
		directory_listing_enabled = other.directory_listing_enabled;
		m_checks = other.m_checks;
	}
	return *this;
}

bool
Server::Location::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = word;
	return true;
}

bool
Server::Location::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = word;
	return true;
}

bool
Server::Location::set_script(const std::string& script)
{
	if (script == "python" || script == "php")
	{
		if (std::find(allowed_scripts.begin(), allowed_scripts.end(), script)
			== allowed_scripts.end())
			allowed_scripts.push_back(script);
		return true;
	}
	else
		return false;
}

const char*
Server::Location::set_directory_listing(const std::string& state)
{
	if (m_checks[DIRECTORY_LISTING])
		return "directory listing shall only be specified once per location block";
	m_checks[DIRECTORY_LISTING] = true;
	if (state == "true")
		directory_listing_enabled = true;
	else if (state == "false")
		directory_listing_enabled = false;
	else
		return "directory listing can only be true or false";
	return nullptr;
}

const char*
Server::Location::check_attributes() const
{
	if (!m_checks[ROOT])
		return "every location must have a root directory";
	else if (!m_checks[INDEX])
		return "every location must have a index file";
	else
		return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Server::Location& rhs)
{
	out << "location: " << rhs.location << "\nroot: " << rhs.root
		<< "\nindex: " << rhs.index << std::endl;
	out << "\nallowed scripts:" << std::endl;
	for (size_t i = 0; i < rhs.allowed_scripts.size(); ++i)
		out << rhs.allowed_scripts[i] << ", ";
	return out;
}
