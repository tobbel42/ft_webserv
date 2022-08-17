#include "Server.hpp"

#include <unistd.h>

#include "utils.hpp"

Server::Server():
	server_names(),
	root(),
	index("index.html"),
	error_pages(DEFAULT_ERROR_PAGES),
	ip_address(),
	ports(),
	allowed_methods(),
	max_client_body_size(UINT32_MAX),
	redirections(),
	locations(),
	m_checks(9, false)
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
	allowed_methods(other.allowed_methods),
	max_client_body_size(other.max_client_body_size),
	redirections(other.redirections),
	locations(other.locations),
	m_checks(other.m_checks)
{}

Server::~Server() {}

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
		allowed_methods = other.allowed_methods;
		max_client_body_size = other.max_client_body_size;
		redirections = other.redirections;
		locations = other.locations;
		m_checks = other.m_checks;
	}
	return *this;
}


bool
Server::set_server_name(const std::string& name)
{
	if (!utils::is_element_of(server_names, name))
		server_names.push_back(name);
	return true;
}

bool
Server::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = utils::compr_slash(word);
	return true;
}

bool
Server::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = utils::compr_slash(word);
	return true;
}

bool
Server::set_error_pages(const std::string& word)
{
	if (m_checks[ERROR_PAGES])
		return false;
	m_checks[ERROR_PAGES] = true;
	error_pages = utils::compr_slash(word);
	return true;
}

bool
Server::set_ip_address(uint32_t ip)
{
	if (m_checks[IP_ADDRESS])
		return false;
	m_checks[IP_ADDRESS] = true;
	ip_address = ip;
	return true;
}

bool
Server::set_port(uint32_t port)
{
	if (utils::is_element_of(ports, port))
		return false;
	ports.push_back(port);
	return true;
}

bool
Server::set_method(const std::string& method)
{
	if (method == "GET" || method == "PUT" || method == "POST"||
		method == "DELETE")
	{
		if (!utils::is_element_of(allowed_methods, method))
			allowed_methods.push_back(method);
		return true;
	}
	else
		return false;
}

bool
Server::set_redirection(const std::pair<std::string,std::string>& file_pr)
{
	std::pair<std::map<std::string,std::string>::iterator, bool> ret;
	ret = redirections.insert(file_pr);
	const std::string& old_value = ret.first->second;
	if (ret.second == false && file_pr.second != old_value)
		return false;
	return true;
}

bool
Server::set_max_client_body_size(uint32_t n)
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
	out << "\nredirections:" << std::endl;
	for (std::map<std::string,std::string>::const_iterator it = rhs.redirections.begin();
		it != rhs.redirections.end(); ++it)
		out << it->first << " = " << it->second << '\n';
	out << std::endl << "locations:\n";
	for (size_t i = 0; i < rhs.locations.size(); ++i)
		out << rhs.locations[i] << std::endl;
	return out;
}


//-------------------------------------------------------------------------
//		Location

Server::Location::Location():
	prefix(),
	root(),
	index(),
	scripts(),
	redirections(),
	max_client_body_size(UINT32_MAX),
	directory_listing_enabled(false),
	m_checks(6, false)
{
	allowed_methods.reserve(3);
}

Server::Location::Location(const Location& other):
	prefix(other.prefix),
	root(other.root),
	index(other.index),
	allowed_methods(other.allowed_methods),
	scripts(other.scripts),
	redirections(other.redirections),
	max_client_body_size(other.max_client_body_size),
	directory_listing_enabled(other.directory_listing_enabled),
	m_checks(other.m_checks)
{}

Server::Location::~Location() {}

Server::Location&
Server::Location::operator=(const Location& other)
{
	if (this != &other)
	{
		prefix = other.prefix;
		root = other.root;
		index = other.index;
		allowed_methods = other.allowed_methods;
		scripts = other.scripts;
		redirections = other.redirections;
		max_client_body_size = other.max_client_body_size;
		directory_listing_enabled = other.directory_listing_enabled;
		m_checks = other.m_checks;
	}
	return *this;
}

bool
Server::Location::set_prefix(const std::string& word)
{
	if (word == "{")
		return false;
	prefix = utils::compr_slash(word);
	if (root.empty())
		root = prefix; // default value for the root
	return true;
}

bool
Server::Location::set_root(const std::string& word)
{
	if (m_checks[ROOT])
		return false;
	m_checks[ROOT] = true;
	root = utils::compr_slash(word);
	return true;
}

bool
Server::Location::set_index(const std::string& word)
{
	if (m_checks[INDEX])
		return false;
	m_checks[INDEX] = true;
	index = utils::compr_slash(word);
	return true;
}

bool
Server::Location::set_method(const std::string& method)
{
	if (method == "GET" || method == "PUT" || method == "POST"
		|| method == "DELETE")
	{
		if (!utils::is_element_of(allowed_methods, method))
			allowed_methods.push_back(method);
		return true;
	}
	else
		return false;
}

bool
Server::Location::set_script(const std::pair<std::string,std::string>& script)
{
	if (script.first != "php" && script.first != "python")
		return false;
	const std::string& binary = script.second;
	if (binary[0] == '/' || binary[0] == '.') // absolute path
	{
		scripts.insert(script);
		return access(binary.c_str(), X_OK) == 0;
	}
	else // relative path
	{
		std::string full_binary = utils::get_abs_path(binary);
		scripts.insert(std::make_pair(script.first, full_binary));
		return access(full_binary.c_str(), X_OK) == 0;
	}
}

bool
Server::Location::set_redirection(const std::pair<std::string,std::string>& file_pr)
{
	std::pair<std::map<std::string,std::string>::iterator, bool> ret;
	ret = redirections.insert(file_pr);
	const std::string& old_value = ret.first->second;
	if (ret.second == false && file_pr.second != old_value)
		return false;
	return true;
}

bool
Server::Location::set_max_client_body_size(uint32_t n)
{
	if (m_checks[MAX_CLIENT_BODY_SIZE])
		return false;
	m_checks[MAX_CLIENT_BODY_SIZE] = true;
	max_client_body_size = n;
	return true;
}

const char*
Server::Location::set_dir_listing(const std::string& state)
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
	if (!m_checks[INDEX])
		return "every location must have a index file";
	else
		return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Server::Location& rhs)
{
	out << "prefix: " << rhs.prefix << "\nroot: " << rhs.root
		<< "\nindex: " << rhs.index << std::endl;
	out << "\nallowed methods: ";
	for (size_t i = 0; i < rhs.allowed_methods.size(); ++i)
		out << rhs.allowed_methods[i] << ", ";
	out << "\nscripts:" << std::endl;
	for (std::map<std::string,std::string>::const_iterator it = rhs.scripts.begin();
		it != rhs.scripts.end(); ++it)
		out << it->first << " = " << it->second << '\n';
	out << "\nredirections:" << std::endl;
	for (std::map<std::string,std::string>::const_iterator it = rhs.redirections.begin();
		it != rhs.redirections.end(); ++it)
		out << it->first << " = " << it->second << '\n';
	out << "\nmax client body size: " << rhs.max_client_body_size
		<< "\ndirectory listing: " << rhs.directory_listing_enabled;
	return out;
}
