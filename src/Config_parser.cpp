/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_parser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/25 10:08:53 by skienzle          #+#    #+#             */
/*   Updated: 2022/06/04 16:29:29 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config_parser.hpp"

#ifdef VERBOSE
bool Config_parser::s_verbose = true;
#else
bool Config_parser::s_verbose = false;
#endif

Config_parser::Config_parser():
	m_servers(),
	m_infile(),
	m_word(),
	m_lineStream()
{
	if (s_verbose)
		std::cout << "Config_parser default constructor called" << std::endl;
}

Config_parser::Config_parser(const Config_parser& other):
	m_servers(other.m_servers),
	m_infile(), // the ofstream can't be copy constructed
	m_word(other.m_word),
	m_lineStream()
{
	if (s_verbose)
		std::cout << "Config_parser copy constructor called" << std::endl;
}

Config_parser::Config_parser(const char *filename):
	m_servers(),
	m_infile(filename),
	m_word(),
	m_lineStream()
{
	if (s_verbose)
		std::cout << "Config_parser filename constructor called" << std::endl;
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
		m_word = other.m_word;
		m_lineStream.str(other.m_lineStream.str());
	}
	if (s_verbose)
		std::cout << "Config_parser assignment operator called" << std::endl;
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

	while (!(word = m_get_next_word('{')).empty())
	{
		if (word != "server")
			throw Config_parser::Invalid_config(__LINE__, "invalid server identifier", word.c_str());
		if (m_get_next_word_protected(__LINE__, '}') != "{")
			throw Config_parser::Invalid_config(__LINE__, "server blocks must be followed by {");
		server_ret = m_read_server();
		if (server_ret.first != "}")
			throw Config_parser::Invalid_config(__LINE__, "invalid server identifier", server_ret.first.c_str());
		m_servers.push_back(server_ret.second);
	}
}

std::pair<std::string, Server_setup>
Config_parser::m_read_server()
{
	Server_setup setup;
	std::string word;

	while (!(word = m_get_next_word_protected(__LINE__, '}')).empty())
	{
		
		if (word == "root")
			setup.root = m_get_next_word_protected(__LINE__);
		else if (word == "server_name")
			setup.server_name = m_get_next_word_protected(__LINE__);
		else if (word == "index")
			setup.index = m_get_next_word_protected(__LINE__);
		else if (word == "location")
		{
			Location_setup location;
			location.location = m_get_next_word_protected(__LINE__);
			if (m_get_next_word_protected(__LINE__, '}') != "{")
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
	
	while (!(word = m_get_next_word_protected(__LINE__, '}')).empty())
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

		
	}
	return word;
}

std::string
Config_parser::m_get_next_word(char delim)
{
	std::string line;
	std::string word;

	if (m_lineStream >> word)
		return word;
	if (std::getline(m_infile, line, delim))
	{
		m_lineStream.clear();
		m_lineStream.str(line);
		return m_get_next_word(delim);
	}
	return word;
}

std::string
Config_parser::m_get_next_word_protected(int line, char delim)
{
	std::string word(m_get_next_word(delim));
	if (word.empty())
		throw Config_parser::Invalid_config(line, "unexpected EOF encountered");
	return word;
}

#if 0

std::string
Config_parser::m_get_next_word(int line, const char *error_msg, const char *strerr)
{
	std::string word;
	if (m_lineStream >> word)
		return word;
	throw Config_parser::Invalid_config(line, error_msg, strerr);
	return word;
}

// special overload for get_next_word if we don't expect the function to throw
std::string
Config_parser::m_get_next_word()
{
	std::string word;
	m_lineStream >> word;
	return word;
}

bool
Config_parser::m_read_next_line(char delim)
{
	std::string line;
	if (std::getline(m_infile, line, delim))
	{
		if (line.empty())
			return m_read_next_line(delim);
		m_lineStream.clear();
		m_lineStream.str(line);
		return true;
	}
	return false;
}

char
Config_parser::m_peek_next_char()
{
	char ret = EOF;
	if (m_lineStream >> ret)
		throw Config_parser::Invalid_config(__LINE__, "unexpected \\n encountered");
	if (m_lineStream.putback(ret))
		throw Config_parser::Invalid_config(__LINE__, "stringstream.putback() failed");
	return ret;
}

#endif // 0

Config_parser::Invalid_config::Invalid_config(int line, const char *msg,
											const char *strerr):
	exception(),
	m_errorMsg("error in line ")
{
	m_errorMsg += line;
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