/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/25 10:08:53 by skienzle          #+#    #+#             */
/*   Updated: 2022/06/09 18:35:05 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config_parser.hpp"

Config_parser::Config_parser():
	m_servers(),
	m_infile(),
	m_lineStream(),
	m_line_num()
{
	#ifdef VERBOSE
		std::cout << "Config_parser default constructor called" << std::endl;
	#endif
}

Config_parser::Config_parser(const Config_parser& other):
	m_servers(other.m_servers),
	m_infile(), // the ofstream can't be copy constructed
	m_lineStream(),
	m_line_num()
{
	#ifdef VERBOSE
	std::cout << "Config_parser copy constructor called" << std::endl;
	#endif
}

Config_parser::Config_parser(const char *filename):
	m_servers(),
	m_infile(filename),
	m_lineStream(),
	m_line_num()
{
	#ifdef VERBOSE
	std::cout << "Config_parser filename constructor called" << std::endl;
	#endif
	if (!m_infile.is_open())
		throw Config_parser::Invalid_config(__LINE__, "couldnt open the config file ", strerror(errno));
}

Config_parser::~Config_parser()
{
	m_infile.close();
}

Config_parser&
Config_parser::operator=(const Config_parser& other)
{
	if (this != &other)
	{
		m_servers = other.m_servers;
		// m_infile = other.m_infile; // the ofstream doesn't have an assignment operator
		m_lineStream.str(other.m_lineStream.str());
	}
	#ifdef VERBOSE
	std::cout << "Config_parser assignment operator called" << std::endl;
	#endif
	return *this;
}

// void
// Config_parser::run()
// {
// 	// std::string line;

// 	// while (std::getline(m_infile, line))
// 	// {
// 	// 	// std::cout << m_word << '\n';
// 	// 	m_lineStream.clear();
// 	// 	m_lineStream.str(line);
// 	// 	while (m_lineStream >> m_word)
// 	// 	{
// 	// 		if (m_word == "server")
// 	// 		{
				
// 	// 		}
// 	// 		else if ()
// 	// 	}
// 	// }
// 	std::string word;
// 	while (true)
// 	{
// 		word = m_get_next_word();
// 		if (word != "server")
// 			throw Config_parser::Invalid_config(__LINE__, "invalid identifier", word.c_str());
// 		if (m_get_next_word() != "{")
// 			throw Config_parser::Invalid_config(__LINE__, "server blocks must start with", "{");
// 		m_read_server();
		
// 	}
// }

void
Config_parser::run()
{
	std::string word;
	std::pair<std::string, Server_setup> server_ret;

	while (!(word = m_get_next_word()).empty())
	{
		if (word != "server")
			throw Config_parser::Invalid_config(__LINE__, "invalid server identifier", word.c_str());
		if (m_get_next_word_protected(__LINE__, false) != "{")
			throw Config_parser::Invalid_config(__LINE__, "server blocks must be followed by {");
		server_ret = m_read_server();
		if (server_ret.first != "}")
			throw Config_parser::Invalid_config(__LINE__, "invalid server identifier", server_ret.first.c_str());
		m_servers.push_back(server_ret.second);
	}
	for (size_t i = 0; i < m_servers.size(); ++i)
		std::cout << m_servers[i] << '\n';
}

std::pair<std::string, Server_setup>
Config_parser::m_read_server()
{
	Server_setup setup;
	std::string word;

	while (!(word = m_get_next_word_protected(__LINE__, false)).empty())
	{
		if (word == "root")
			setup.root = m_get_next_word_protected(__LINE__);
		else if (word == "server_name")
			setup.server_name = m_get_next_word_protected(__LINE__);
		else if (word == "index")
			setup.index = m_get_next_word_protected(__LINE__);
		else if (word == "max-client-body-size")
			setup.max_client_body_size = m_check_int(m_get_next_word_protected(__LINE__));
		else if (word == "port")
			setup.port.push_back(m_check_int(m_get_next_word_protected(__LINE__)));
		else if (word == "ip-address")
			setup.ip_address = m_check_ip_address();
		else if (word == "location")
		{
			Location_setup location;
			location.location = m_get_next_word_protected(__LINE__);
			if (m_get_next_word_protected(__LINE__, false) != "{")
				throw Config_parser::Invalid_config(__LINE__, "location blocks must start with a {");
			std::string location_ret = m_read_location(location);
			if (location_ret != "}")
				throw Config_parser::Invalid_config(__LINE__, "invalid location identifier", location_ret.c_str());
			setup.locations.push_back(location);
		}
		else
			return make_pair(word, setup);
	}
	return make_pair(std::string(), setup);
}

std::string
Config_parser::m_read_location(Location_setup& location)
{
	std::string word;
	
	while (!(word = m_get_next_word_protected(__LINE__, false)).empty())
	{
		if (word == "root")
			location.root = m_get_next_word_protected(__LINE__);
		else if (word == "allowed_methods")
		{
			while (m_lineStream >> word)
			{
				if (word == "GET" || word == "POST" || word == "DELETE")
					location.allowed_methods.push_back(word);
				else
					throw Config_parser::Invalid_config(__LINE__,
						"invalid http-method identifier", word.c_str());
			}
		}
		else if (word == "allowed_scripts")
		{
			while (m_lineStream >> word)
			{
				if (word == "php" || word == "python") // add any other allowed scrips here
					location.allowed_scripts.push_back(word);
				else
					throw Config_parser::Invalid_config(__LINE__,
						"invalid script type", word.c_str());
			}
		}
		else if (word == "directory_listing")
		{
			word = m_get_next_word_protected(__LINE__);
			if (word == "true")
				location.directory_listing_enabled = true;
			else if (word == "false")
				location.directory_listing_enabled = false;
			else
				throw Config_parser::Invalid_config(m_line_num,
					"directory listing can only be true or false and not", word.c_str());
		}
		else
			return word;
		
	}
	return word;
}

std::string
Config_parser::m_get_next_word()
{
	std::string line;
	std::string word;

	if (m_lineStream >> word)
	{
		if (!word.empty() && word[0] != '#')
			return word;
	}
	if (std::getline(m_infile, line))
	{
		++m_line_num;
		m_lineStream.clear();
		m_lineStream.str(line);
		return m_get_next_word();
	}
	return word;
}

std::string
Config_parser::m_get_next_word_protected(int line, bool is_on_same_line)
{
	size_t current_line = m_line_num;
	std::string word(m_get_next_word());
	
	if (is_on_same_line && current_line != m_line_num)
		throw Config_parser::Invalid_config(current_line, "values must be on the same line as their keyword");
	else if (word.empty())
		throw Config_parser::Invalid_config(line, "unexpected EOF encountered");
	return word;
}

uint32_t
Config_parser::m_check_int(const std::string& word)
{
	for (std::string::const_iterator it = word.begin(); it != word.end(); ++it)
	{
		if (!isdigit(*it))
			throw Config_parser::Invalid_config(m_line_num, "invalid unsigned integer format", word.c_str());
	}
	return atoi(word.c_str());
}

uint32_t
Config_parser::m_check_ip_address()
{
	std::string word(m_get_next_word_protected(__LINE__));
	size_t start = 0;
	size_t end = 0;
	int byte = 3;
	uint32_t ip_addr = 0;

	while (end != std::string::npos)
	{
		end = word.find('.', start);
		int ip_number = m_check_int(word.substr(start, end - start));
		if (ip_number > 255)
			break;
		ip_addr += ip_number << (byte * 8); // shift the ip-byte to the appropriate byte in the address
		start = end + 1;
		--byte;
	}
	if (byte != -1)
		throw Config_parser::Invalid_config(__LINE__, "invalid ip address format", word.c_str());
	return ip_addr;
}

Config_parser::Invalid_config::Invalid_config(size_t line, const char *msg,
											const char *strerr):
	exception(),
	m_errorMsg("error in line ")
{
	{
		std::stringstream ss;
		ss << line;
		m_errorMsg += ss.str();
	}

	(m_errorMsg += ":\n") += msg;
	if (strerr != nullptr)
		(m_errorMsg += ": ") += strerr;
}

Config_parser::Invalid_config::~Invalid_config() throw() {}

const char *
Config_parser::Invalid_config::what() const throw()
{
	return m_errorMsg.c_str();
}




std::ostream& operator<<(std::ostream& out, const Location_setup& rhs)
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

std::ostream& operator<<(std::ostream& out, const Server_setup& rhs)
{
	out << "server name: " << rhs.server_name << "\nroot: " << rhs.root
		<< "\nerror pages: " << rhs.error_pages << "\nip address: " << rhs.ip_address
		<< "\nmax client body size: " << rhs.max_client_body_size << "\nports:" << std::endl;
	for (size_t i = 0; i < rhs.port.size(); ++i)
		out << rhs.port[i] << ", ";
	for (size_t i = 0; i < rhs.locations.size(); ++i)
		out << rhs.locations[i] << std::endl;
	return out;
}