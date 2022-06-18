/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/25 10:08:53 by skienzle          #+#    #+#             */
/*   Updated: 2022/06/11 16:10:29 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser(ServerArr& servers):
	m_servers(servers),
	m_infile(),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
		std::cout << "ConfigParser default constructor called" << std::endl;
	#endif
}

ConfigParser::ConfigParser(ServerArr& servers, const char *filename):
	m_servers(servers),
	m_infile(filename),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
	std::cout << "ConfigParser filename constructor called" << std::endl;
	#endif

	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number,
			"couldnt open the config file", strerror(errno));
}

ConfigParser::~ConfigParser()
{
	m_infile.close();
}

void
ConfigParser::assign_file(const char *filename)
{
	if (m_infile.is_open())
		m_infile.close();

	m_infile.open(filename);
	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number,
			"couldnt open the config file", strerror(errno));
}

void
ConfigParser::run()
{
	std::string word;
	std::pair<std::string, Server> server_ret;

	while (!(word = m_get_next_word()).empty())
	{
		if (word != "server")
			throw ConfigParser::InvalidConfig(m_line_number,
				"invalid server starting identifier", word.c_str());

		if (m_get_next_word_protected(false) != "{")
			throw ConfigParser::InvalidConfig(m_line_number,
				"server blocks must be followed by {");

		server_ret = m_read_server();
		if (server_ret.first != "}")
			throw ConfigParser::InvalidConfig(m_line_number,
				"invalid server identifier", server_ret.first.c_str());

		const char *error_msg = server_ret.second.check_attributes();
		if (error_msg != nullptr)
			throw ConfigParser::InvalidConfig(m_line_number, error_msg);

		m_servers.push_back(server_ret.second);
	}

	m_check_server_configs();

	#ifdef VERBOSE
	for (size_t i = 0; i < m_servers.size(); ++i)
		std::cout << m_servers[i] << '\n';
	#endif
}

std::pair<std::string, Server>
ConfigParser::m_read_server()
{
	Server server;
	std::string word;

	while (!(word = m_get_next_word_protected(false)).empty())
	{
		if (word == "server_name")
			server.set_server_name(m_get_next_word_protected());
		else if (word == "root")
		{
			if (!server.set_root(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one root shall be specified for each server");
		}
		else if (word == "index")
		{
			if (!server.set_index(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one index page shall be specified for each server");
		}
		else if (word == "errorpages")
		{
			if (!server.set_error_pages(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one errorpage directory shall be specified for each server");
		}
		else if (word == "max_client_body_size")
		{
			if (!server.set_max_client_body_size(m_check_int(m_get_next_word_protected())))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one client body size shall be specified for each server");
		}
		else if (word == "ip-address")
		{
			if (!server.set_ip_address(m_check_ip_address()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"every server must have only one ip address");
		}
		else if (word == "port")
			server.set_port(m_check_int(m_get_next_word_protected()));
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
						"invalid http-method identifier", word.c_str());
			}
		}
		else if (word == "location")
		{
			Server::Location location;
			location.location = m_get_next_word_protected();
			if (location.location == "{")
				throw ConfigParser::InvalidConfig(m_line_number,
					"location blocks must define the directory they operate in");

			if (m_get_next_word_protected(false) != "{")
				throw ConfigParser::InvalidConfig(m_line_number, 
					"location blocks must start with a {");

			std::string location_ret = m_read_location(location);
			if (location_ret != "}")
				throw ConfigParser::InvalidConfig(m_line_number,
					"invalid location identifier", location_ret.c_str());

			const char* error_msg = location.check_attributes();
			if (error_msg != nullptr)
				throw ConfigParser::InvalidConfig(m_line_number, error_msg);

			server.locations.push_back(location);
		}
		else
			return make_pair(word, server);
	}
	return make_pair(std::string(), server);
}

std::string
ConfigParser::m_read_location(Server::Location& location)
{
	std::string word;
	
	while (!(word = m_get_next_word_protected(false)).empty())
	{
		if (word == "root")
		{
			if (!location.set_root(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one root shall be specified for each location");
		}
		else if (word == "index")
		{
			if (!location.set_index(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one index page shall be specified for each location");
		}
		else if (word == "allowed_scripts")
		{
			while (m_line_stream >> word)
			{
				if (word.empty() || word[0] == '#')
				{
					m_line_stream.str(std::string());
					break;
				}
				if (!location.set_script(word))
					throw ConfigParser::InvalidConfig(m_line_number,
						"invalid script type", word.c_str());
			}
		}
		else if (word == "directory_listing")
		{
			const char* error_msg = location.set_directory_listing(
										m_get_next_word_protected());
			if (error_msg != nullptr)
				throw ConfigParser::InvalidConfig(m_line_number, error_msg);
		}
		else
			return word;
	}
	return word;
}

std::string
ConfigParser::m_get_next_word()
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
		return m_get_next_word();
	}
	return word;
}

std::string
ConfigParser::m_get_next_word_protected(bool is_on_same_line)
{
	size_t current_line = m_line_number;
	std::string word = m_get_next_word();
	
	if (is_on_same_line && current_line != m_line_number)
		throw ConfigParser::InvalidConfig(current_line,
			"values must be on the same line as their keyword");

	else if (word.empty())
		throw ConfigParser::InvalidConfig(m_line_number,
			"unexpected EOF encountered");
	return word;
}

uint32_t
ConfigParser::m_check_int(const std::string& word)
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

uint32_t
ConfigParser::m_check_ip_address()
{
	std::string word = m_get_next_word_protected();
	size_t start = 0;
	size_t end = 0;
	int byte = 3;
	uint32_t ip_addr = 0;

	while (end != std::string::npos)
	{
		end = word.find('.', start);
		uint32_t ip_number = m_check_int(word.substr(start, end - start));
		if (ip_number > std::numeric_limits<uint8_t>::max())
			break;
		ip_addr += ip_number << (byte * 8); // shift the ip-byte to the appropriate byte in the address
		start = end + 1;
		--byte;
	}
	if (byte != -1)
		throw ConfigParser::InvalidConfig(m_line_number, "invalid ip address format", word.c_str());
	return ip_addr;
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
ConfigParser::m_check_server_configs()
{
	if (m_servers.empty())
		throw ConfigParser::InvalidConfig(m_line_number,
			"at least one server block must be specified");

	for (ServerArr::iterator server_it = m_servers.begin();
		server_it != m_servers.end(); ++server_it)
	{
		for (ServerArr::iterator it = server_it + 1; it != m_servers.end(); ++it)
		{
			if (server_it->ip_address == it->ip_address)
			{
				for (std::vector<uint32_t>::iterator port_it = server_it->ports.begin();
					port_it != server_it->ports.end(); ++port_it)
				{
					if (std::find(it->ports.begin(), it->ports.end(), *port_it)
						!= it->ports.end())
							throw ConfigParser::InvalidConfig(m_line_number,
								"duplicate ip:port combination",
								std::string(
									utils::ip_to_string(server_it->ip_address) +
									":" + utils::to_string(*port_it)
								).c_str());
				}
			}

			for (StringArr::iterator server_name_it = server_it->server_names.begin();
				server_name_it != server_it->server_names.end(); ++server_name_it)
			{
				if (std::find(it->server_names.begin(), it->server_names.end(), *server_name_it)
					!= it->server_names.end())
						throw ConfigParser::InvalidConfig(m_line_number,
							"duplicated server names are not allowed",
							server_name_it->c_str());
			}
		}
	}
}



ConfigParser::InvalidConfig::InvalidConfig(size_t line, const char *msg,
											const char *details):
	exception(),
	m_errorMsg("error in line ")
{
	m_errorMsg += utils::to_string(line);

	(m_errorMsg += ":\n") += msg;
	if (details != nullptr)
		(m_errorMsg += ": ") += details;
}

ConfigParser::InvalidConfig::~InvalidConfig() throw() {}

const char*
ConfigParser::InvalidConfig::what() const throw()
{
	return m_errorMsg.c_str();
}
