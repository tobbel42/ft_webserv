
#pragma once

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <limits>
#include <string>
#include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <algorithm>

#include "Server.hpp"
#include "utils.hpp"
#include "typedefs.hpp"


#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[91m"
#define YELLOW "\033[33m"

class ConfigParser
{
public: // methods
	ConfigParser(ServerArr& servers);
	ConfigParser(ServerArr& servers, const std::string& filename);
	~ConfigParser();

	void assign_file(const std::string& filename);

	void run();


private: // methods
	ConfigParser();
	ConfigParser(const ConfigParser&);

	ConfigParser& operator=(const ConfigParser&);

	std::string get_next_word();
	// gnw = get next word
	std::string gnw_protected(bool is_on_same_line = true);

	std::pair<std::string, Server>	parse_server();
	std::string						parse_location(Server::Location& location);
	std::pair<std::string,std::string>		parse_key_value();

	void		check_configs();
	uint32_t	check_uint(const std::string& word);
	uint32_t	check_ip_address();


private: // subclass

	class InvalidConfig: public std::exception
	{
	public:
		InvalidConfig(size_t line, const char *msg, const char *details = nullptr);
		~InvalidConfig() throw();
		const char *what() const throw();
	private:
		std::string m_errorMsg;
	};


private: // attributes
	std::vector<Server>&			m_servers; // reference to the engines' servers
	std::ifstream					m_infile;
	std::stringstream				m_line_stream;
	size_t							m_line_number;
};