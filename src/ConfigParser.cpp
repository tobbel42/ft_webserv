#include "ConfigParser.hpp"

#include <climits>
#include <cctype>
#include <cstdlib>
#include <cstring>

#include "utils.hpp"

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[91m"
#define YELLOW "\033[33m"

ConfigParser::ConfigParser(ServerArr& servers):
	m_servers(servers),
	m_infile(),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
		PRINT("ConfigParser default constructor called");
	#endif
}

ConfigParser::ConfigParser(ServerArr& servers, const std::string& filename):
	m_servers(servers),
	m_infile(filename.c_str()),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
		PRINT("ConfigParser filename constructor called");
	#endif

	if (utils::get_file_ext(filename) != "conf")
		throw ConfigParser::InvalidConfig(m_line_number,
			"config files must end with .conf");

	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number,
			"couldnt open the config file", std::strerror(errno));
}

ConfigParser::~ConfigParser()
{
	m_infile.close();
}

void
ConfigParser::assign_file(const std::string& filename)
{
	if (utils::get_file_ext(filename) != "conf")
		throw ConfigParser::InvalidConfig(m_line_number,
			"config files must end with .conf");

	if (m_infile.is_open())
		m_infile.close();

	m_infile.open(filename.c_str());
	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number,
			"couldnt open the config file", std::strerror(errno));
}

void
ConfigParser::run()
{
	std::string word;

	while ((word = get_next_word()).empty() == false)
	{
		if (word != "server")
			throw ConfigParser::InvalidConfig(m_line_number,
				"invalid server starting identifier", word.c_str());

		if (gnw_protected(false) != "{")
			throw ConfigParser::InvalidConfig(m_line_number,
				"server blocks must be followed by {");

		std::pair<std::string, Server> server_ret = parse_server();
		std::string& token = server_ret.first;
		Server& server = server_ret.second;

		if (token != "}")
			throw ConfigParser::InvalidConfig(m_line_number,
				"invalid server identifier", token.c_str());

		const char* error_msg = server.check_attributes();
		if (error_msg != nullptr)
			throw ConfigParser::InvalidConfig(m_line_number, error_msg);

		m_servers.push_back(server);
	}

	check_configs();

	#ifdef VERBOSE
	for (size_t i = 0; i < m_servers.size(); ++i)
		PRINT(m_servers[i]);
	#endif
}

std::pair<std::string, Server>
ConfigParser::parse_server()
{
	Server server;
	std::string word;

	while ((word = gnw_protected(false)).empty() == false)
	{
		if (word == "server_name")
			server.set_server_name(gnw_protected());
		else if (word == "root")
		{
			if (!server.set_root(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one root shall be specified for each server");
		}
		else if (word == "index")
		{
			if (!server.set_index(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one index page shall be specified for each server");
		}
		else if (word == "errorpages")
		{
			if (!server.set_error_pages(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one errorpage directory shall be specified for each server");
		}
		else if (word == "max_client_body_size")
		{
			if (!server.set_max_client_body_size(check_uint(gnw_protected())))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one client body size shall be specified for each server");
		}
		else if (word == "ip-address")
		{
			if (!server.set_ip_address(check_ip_address()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"every server must have only one ip address");
		}
		else if (word == "port")
		{
			if (!server.set_port(check_uint(gnw_protected())))
				throw ConfigParser::InvalidConfig(m_line_number,
					"each port of a sever must be unique");

		}
		else if (word == "allowed_methods")
		{
			while (m_line_stream >> word)
			{
				if (word.empty() || word[0] == '#')
				{
					m_line_stream.str(std::string());
					break;
				}
				if (!server.set_method(word))
					throw ConfigParser::InvalidConfig(m_line_number,
						"invalid http-method type", word.c_str());
			}
		}
		else if (word == "redirection")
		{
			std::pair<std::string,std::string> pr = parse_key_value();

			if (!server.set_redirection(pr))
				throw ConfigParser::InvalidConfig(m_line_number,
					"ambiguous redirection detected",
					(pr.first + " = " + pr.second).c_str());
		}
		else if (word == "location")
		{
			Server::Location location;

			if (!location.set_prefix(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"location blocks must define the directory they operate in");

			if (gnw_protected(false) != "{")
				throw ConfigParser::InvalidConfig(m_line_number, 
					"location blocks must start with a {");

			std::string location_ret = parse_location(location);
			if (location_ret != "}")
				throw ConfigParser::InvalidConfig(m_line_number,
					"invalid location identifier", location_ret.c_str());

			const char* error_msg = location.check_attributes();
			if (error_msg != nullptr)
				throw ConfigParser::InvalidConfig(m_line_number, error_msg);

			server.locations.push_back(location);
		}
		else
			return std::make_pair(word, server);
	}
	return std::make_pair(std::string(), server);
}

std::string
ConfigParser::parse_location(Server::Location& location)
{
	std::string word;
	
	while ((word = gnw_protected(false)).empty() == false)
	{
		if (word == "root")
		{
			if (!location.set_root(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one root shall be specified for each location");
		}
		else if (word == "index")
		{
			if (!location.set_index(gnw_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one index page shall be specified for each location");
		}
		else if (word == "allowed_methods")
		{
			while (m_line_stream >> word)
			{
				if (word.empty() || word[0] == '#')
				{
					m_line_stream.str(std::string());
					break;
				}
				if (!location.set_method(word))
					throw ConfigParser::InvalidConfig(m_line_number,
						"invalid http-method type", word.c_str());
			}
		}
		else if (word == "allowed_scripts")
		{
			// first = srcipt identifier, second = executable name
			std::pair<std::string,std::string> pr = parse_key_value();
			if (!location.set_script(pr))
				throw ConfigParser::InvalidConfig(m_line_number,
					"invalid script name or binary",
					(pr.first + " = " + pr.second).c_str());
		}
		else if (word == "redirection")
		{
			std::pair<std::string,std::string> pr = parse_key_value();

			if (!location.set_redirection(pr))
				throw ConfigParser::InvalidConfig(m_line_number,
					"ambiguous redirection detected",
					(pr.first + " = " + pr.second).c_str());
		}
		else if (word == "max_client_body_size")
		{
			if (!location.set_max_client_body_size(check_uint(gnw_protected())))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one client body size shall be specified for each server");
		}
		else if (word == "directory_listing")
		{
			const char* error_msg = location.set_dir_listing(gnw_protected());
			if (error_msg != nullptr)
				throw ConfigParser::InvalidConfig(m_line_number, error_msg);
		}
		else
			return word;
	}
	return word;
}

std::string
ConfigParser::get_next_word()
{
	std::string line;
	std::string word;

	if (m_line_stream >> word)
	{
		if (!word.empty() && word[0] != '#')
			return word;
	}
	if (std::getline(m_infile, line))
	{
		++m_line_number;
		m_line_stream.clear();
		m_line_stream.str(line);
		return get_next_word();
	}
	return word;
}

std::string
ConfigParser::gnw_protected(bool is_on_same_line)
{
	size_t current_line = m_line_number;
	std::string word = get_next_word();
	
	if (is_on_same_line && current_line != m_line_number)
		throw ConfigParser::InvalidConfig(current_line,
			"values must be on the same line as their keyword");

	else if (word.empty())
		throw ConfigParser::InvalidConfig(m_line_number,
			"unexpected EOF encountered");
	return word;
}

std::pair<std::string,std::string>
ConfigParser::parse_key_value()
{
	std::string word;
	std::string first, second;
	size_t i = 0;

	for (; m_line_stream >> word; ++i)
	{
		if (word.empty() || word[0] == '#')
		{
			m_line_stream.str(std::string());
			break;
		}

		switch (i)
		{
		case 0:
			first = word;
			break;
		case 1:
			if (word != "=")
				throw ConfigParser::InvalidConfig(m_line_number,
					"key-value pairs must be separated by a =");
			break;
		case 2:
			second = word;
			break;
		default:
			break;
		}
	}
	if (i != 3)
		throw ConfigParser::InvalidConfig(m_line_number,
			"invalid layout for a pair. The correct layout is",
			"key = value");

	return std::make_pair(first, second);
}

uint32_t
ConfigParser::check_ip_address()
{
	std::string word = gnw_protected();
	size_t start = 0;
	size_t end = 0;
	int byte = 3;
	uint32_t ip_addr = 0;

	while (end != std::string::npos)
	{
		end = word.find('.', start);
		uint32_t ip_number = check_uint(word.substr(start, end - start));
		if (ip_number > UINT8_MAX)
			break;
		// shift the ip-byte to the appropriate byte in the address
		ip_addr += ip_number << (byte * 8);
		start = end + 1;
		--byte;
	}
	if (byte != -1)
		throw ConfigParser::InvalidConfig(m_line_number,
			"invalid ip address format", word.c_str());
	return ip_addr;
}

uint32_t
ConfigParser::check_uint(const std::string& word)
{
	for (std::string::const_iterator it = word.begin(); it != word.end(); ++it)
	{
		if (!isdigit(*it))
			throw ConfigParser::InvalidConfig(m_line_number,
				"invalid unsigned integer format", word.c_str());
	}

	uint64_t number = strtoul(word.c_str(), NULL, 10);
	if (number > UINT32_MAX)
		throw ConfigParser::InvalidConfig(m_line_number, 
			"number too big", word.c_str());
	return number;
}

/*
Checks for duplicate ip-address:port combinations
and server names across all server blocks

Thanks to C++98 this looks very ugly...
I miss
for (const auto& i : container)
{
	...
}
so much here :(
*/
void
ConfigParser::check_configs()
{
	typedef ServerArr::iterator				ServerIt;
	typedef std::vector<uint32_t>::iterator	PortIt;

	if (m_servers.empty())
		throw ConfigParser::InvalidConfig(m_line_number,
			"at least one server block must be specified");

	// loop through all the servers
	for (ServerIt server_it = m_servers.begin();
		server_it != m_servers.end(); ++server_it)
	{
		// and check each subsequent server
		for (ServerIt it = server_it + 1; it != m_servers.end(); ++it)
		{
			// for duplicate ip-addresses
			if (server_it->ip_address == it->ip_address)
			{
				// and ports
				for (PortIt port_it = server_it->ports.begin();
					port_it != server_it->ports.end(); ++port_it)
				{
					// if there is a match we throw an error
					if (utils::is_element_of(it->ports, *port_it))
						throw ConfigParser::InvalidConfig(m_line_number,
							"duplicate ip:port combination",
							std::string(
								utils::ip_to_string(server_it->ip_address) +
								":" + utils::to_string(*port_it)
							).c_str());
				}
			}

			// and all server names
			for (StringArr::iterator server_name_it = server_it->server_names.begin();
				server_name_it != server_it->server_names.end(); ++server_name_it)
			{
				// for duplicates aswell
				if (utils::is_element_of(it->server_names, *server_name_it))
						throw ConfigParser::InvalidConfig(m_line_number,
							"duplicated server names are not allowed",
							server_name_it->c_str());
			}
		}
	}
}


//-------------------------------------------------------------------------
//		InvalidConfig

ConfigParser::InvalidConfig::InvalidConfig(size_t line, const char *msg,
											const char *details):
	exception(),
	m_error_msg("\nconfig file error in line ")
{
	m_error_msg += utils::to_string(line);
	m_error_msg += ":\n";
	m_error_msg += RED;
	m_error_msg += BOLD;
	m_error_msg += msg;
	if (details != nullptr)
	{
		m_error_msg += ": ";
		m_error_msg += YELLOW;
		m_error_msg += details;
	}
	m_error_msg += RESET;
	m_error_msg += '\n';
}

ConfigParser::InvalidConfig::~InvalidConfig() throw() {}

const char*
ConfigParser::InvalidConfig::what() const throw()
{
	return m_error_msg.c_str();
}
