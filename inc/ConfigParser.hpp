
#pragma once

#include <cctype>
#include <cstdlib>
#include <cstdint>

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


#ifndef nullptr
#define nullptr NULL
#endif

class ConfigParser
{
public: // methods
	ConfigParser(ServerArr& servers);
	ConfigParser(ServerArr& servers, const char *filename);
	~ConfigParser();

	void assign_file(const char *filename);

	void run();


private: // methods
	ConfigParser();
	ConfigParser(const ConfigParser&);

	ConfigParser& operator=(const ConfigParser&);


	std::string m_get_next_word();
	std::string m_get_next_word_protected(bool is_on_same_line = true);

	std::pair<std::string, Server>	m_read_server();
	std::string						m_read_location(Server::Location& location);

	void		m_check_server_configs();
	uint32_t	m_check_int(const std::string& word);
	uint32_t	m_check_ip_address();


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