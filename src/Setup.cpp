

#include "Setup.hpp"

//		Server Setup

ServerSetup::ServerSetup():
	server_names(),
	root("/"),
	index("index.html"),
	error_pages("./error_pages"),
	ip_address(),
	ports(),
	max_client_body_size(std::numeric_limits<uint32_t>::max()),
	locations(),
	m_checks(7, false)
	{}

ServerSetup::ServerSetup(const ServerSetup& other):
	server_names(other.server_names),
	root(other.root),
	index(other.index),
	error_pages(other.error_pages),
	ip_address(other.ip_address),
	ports(other.ports),
	max_client_body_size(other.max_client_body_size),
	locations(other.locations),
	m_checks(other.m_checks)
	{}

ServerSetup&
ServerSetup::operator=(const ServerSetup& other)
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
		locations = other.locations;
		m_checks = other.m_checks;
	}
	return *this;
}


bool
ServerSetup::set_server_name(const std::string& name)
{
	if (std::find(server_names.begin(), server_names.end(), name)
		== server_names.end())
		server_names.push_back(name);
	return true;
}

bool
ServerSetup::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = word;
	return true;
}

bool
ServerSetup::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = word;
	return true;
}

bool
ServerSetup::set_error_pages(const std::string& word)
{
	if (m_checks[ERROR_PAGES])
		return false;
	m_checks[ERROR_PAGES] = true;
	error_pages = word;
	return true;
}

bool
ServerSetup::set_ip_address(uint32_t ip)
{
	if (m_checks[IP_ADDRESS])
		return false;
	m_checks[IP_ADDRESS] = true;
	ip_address = ip;
	return true;
}

bool
ServerSetup::set_port(uint32_t port)
{
	ports.push_back(port);
	return true;
}

bool
ServerSetup::set_max_client_body_size(uint32_t n)
{
	if (m_checks[MAX_CLIENT_BODY_SIZE])
		return false;
	m_checks[MAX_CLIENT_BODY_SIZE] = true;
	max_client_body_size = n;
	return true;
}


const char*
ServerSetup::check_attributes() const
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

std::ostream& operator<<(std::ostream& out, const ServerSetup& rhs)
{
	out << "server names: ";
	for (size_t i = 0; i < rhs.server_names.size(); ++i)
		out << rhs.server_names[i] << ", ";
	out << "\nroot: " << rhs.root << "\nerror pages: " << rhs.error_pages
		<< "\nip address: " << rhs.ip_address << "\nmax client body size: "
		<< rhs.max_client_body_size << "\nports:" << std::endl;
	for (size_t i = 0; i < rhs.ports.size(); ++i)
		out << rhs.ports[i] << ", ";
	out << std::endl;
	for (size_t i = 0; i < rhs.locations.size(); ++i)
		out << rhs.locations[i] << std::endl;
	return out;
}



//		Location Setup

LocationSetup::LocationSetup():
	location(),
	root("/"),
	index("index.html"),
	allowed_methods(),
	allowed_scripts(),
	directory_listing_enabled(false),
	m_checks(5, false)
	{}

LocationSetup::LocationSetup(const LocationSetup& other):
	location(other.location),
	root(other.root),
	index(other.index),
	allowed_methods(other.allowed_methods),
	allowed_scripts(other.allowed_scripts),
	directory_listing_enabled(other.directory_listing_enabled),
	m_checks(other.m_checks)
	{}

LocationSetup&
LocationSetup::operator=(const LocationSetup& other)
{
	if (this != &other)
	{
		location = other.location;
		root = other.root;
		index = other.index;
		allowed_methods = other.allowed_methods;
		allowed_scripts = other.allowed_scripts;
		directory_listing_enabled = other.directory_listing_enabled;
		m_checks = other.m_checks;
	}
	return *this;
}

bool
LocationSetup::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = word;
	return true;
}

bool
LocationSetup::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = word;
	return true;
}

bool
LocationSetup::set_method(const std::string& method)
{
	if (method == "get" || method == "post" || method == "delete")
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
LocationSetup::set_script(const std::string& script)
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
LocationSetup::set_directory_listing(const std::string& state)
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
LocationSetup::check_attributes() const
{
	if (!m_checks[ROOT])
		return "every location must have a root directory";
	else if (!m_checks[INDEX])
		return "every location must have a index file";
	else
		return nullptr;
}

std::ostream& operator<<(std::ostream& out, const LocationSetup& rhs)
{
	out << "location: " << rhs.location << "\nroot: " << rhs.root
		<< "\nindex: " << rhs.index << "\nallowed methods:" << std::endl;
	for (size_t i = 0; i < rhs.allowed_methods.size(); ++i)
		out << rhs.allowed_methods[i] << ", ";
	out << "\nallowed scripts:" << std::endl;
	for (size_t i = 0; i < rhs.allowed_scripts.size(); ++i)
		out << rhs.allowed_scripts[i] << ", ";
	return out;
}