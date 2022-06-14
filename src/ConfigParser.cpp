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

ConfigParser::ConfigParser():
	m_servers(),
	m_infile(),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
		std::cout << "ConfigParser default constructor called" << std::endl;
	#endif
}

ConfigParser::ConfigParser(const ConfigParser& other):
	m_servers(other.m_servers),
	m_infile(), // the ofstream can't be copy constructed
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
	std::cout << "ConfigParser copy constructor called" << std::endl;
	#endif
}

ConfigParser::ConfigParser(const char *filename):
	m_servers(),
	m_infile(filename),
	m_line_stream(),
	m_line_number()
{
	#ifdef VERBOSE
	std::cout << "ConfigParser filename constructor called" << std::endl;
	#endif
	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number, "couldnt open the config file", strerror(errno));
}

ConfigParser::~ConfigParser()
{
	m_infile.close();
}

ConfigParser&
ConfigParser::operator=(const ConfigParser& other)
{
	if (this != &other)
	{
		m_servers = other.m_servers;
		// m_infile = other.m_infile; // the ofstream doesn't have an assignment operator
		m_line_stream.str(other.m_line_stream.str());
	}
	#ifdef VERBOSE
	std::cout << "ConfigParser assignment operator called" << std::endl;
	#endif
	return *this;
}

void
ConfigParser::assign_file(const char *filename)
{
	if (m_infile.is_open())
		m_infile.close();
	m_infile.open(filename);
	if (!m_infile.is_open())
		throw ConfigParser::InvalidConfig(m_line_number, "couldnt open the config file", strerror(errno));
}

void
ConfigParser::run()
{
	std::string word;
	std::pair<std::string, ServerSetup> server_ret;

	while (!(word = m_get_next_word()).empty())
	{
		if (word != "server")
			throw ConfigParser::InvalidConfig(m_line_number, "invalid server starting identifier", word.c_str());
		if (m_get_next_word_protected(false) != "{")
			throw ConfigParser::InvalidConfig(m_line_number, "server blocks must be followed by {");
		server_ret = m_read_server();
		if (server_ret.first != "}")
			throw ConfigParser::InvalidConfig(m_line_number, "invalid server identifier", server_ret.first.c_str());
		const char *error_msg = server_ret.second.check_attributes();
		if (error_msg != nullptr)
			throw ConfigParser::InvalidConfig(m_line_number, error_msg);
		m_servers.push_back(server_ret.second);
	}
	if (m_servers.empty())
		throw ConfigParser::InvalidConfig(m_line_number, "at least one server block must be specified");

	#ifdef VERBOSE
	for (size_t i = 0; i < m_servers.size(); ++i)
		std::cout << m_servers[i] << '\n';
	#endif
}

std::pair<std::string, ServerSetup>
ConfigParser::m_read_server()
{
	ServerSetup setup;
	std::string word;

	while (!(word = m_get_next_word_protected(false)).empty())
	{
		if (word == "server_name")
			setup.set_server_name(m_get_next_word_protected());
		else if (word == "root")
		{
			if (!setup.set_root(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one root shall be specified for each server");
		}
		else if (word == "index")
		{
			if (!setup.set_index(m_get_next_word_protected()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one index page shall be specified for each server");
		}
		else if (word == "max-client-body-size")
		{
			if (!setup.set_max_client_body_size(m_check_int(m_get_next_word_protected())))
				throw ConfigParser::InvalidConfig(m_line_number,
					"only one client body size shall be specified for each server");
		}
		else if (word == "ip-address")
		{
			if (!setup.set_ip_address(m_check_ip_address()))
				throw ConfigParser::InvalidConfig(m_line_number,
					"every server must have only one ip address");
		}
		else if (word == "port")
			setup.set_port(m_check_int(m_get_next_word_protected()));
		else if (word == "location")
		{
			LocationSetup location;
			location.location = m_get_next_word_protected();
			if (m_get_next_word_protected(false) != "{")
				throw ConfigParser::InvalidConfig(m_line_number, "location blocks must start with a {");
			std::string location_ret = m_read_location(location);
			if (location_ret != "}")
				throw ConfigParser::InvalidConfig(m_line_number, "invalid location identifier", location_ret.c_str());
			const char* error_msg = location.check_attributes();
			if (error_msg != nullptr)
				throw ConfigParser::InvalidConfig(m_line_number, error_msg);
			setup.locations.push_back(location);
		}
		else
			return make_pair(word, setup);
	}
	return make_pair(std::string(), setup);
}

std::string
ConfigParser::m_read_location(LocationSetup& location)
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
						"invalid http-method identifier", word.c_str());
			}
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
		throw ConfigParser::InvalidConfig(current_line, "values must be on the same line as their keyword");
	else if (word.empty())
		throw ConfigParser::InvalidConfig(m_line_number, "unexpected EOF encountered");
	return word;
}

uint32_t
ConfigParser::m_check_int(const std::string& word)
{
	for (std::string::const_iterator it = word.begin(); it != word.end(); ++it)
	{
		if (!isdigit(*it))
			throw ConfigParser::InvalidConfig(m_line_number, "invalid unsigned integer format", word.c_str());
	}
	uint64_t number = strtoul(word.c_str(), NULL, 10);
	if (number > std::numeric_limits<uint16_t>::max())
		throw ConfigParser::InvalidConfig(m_line_number, "number too big", word.c_str());
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
