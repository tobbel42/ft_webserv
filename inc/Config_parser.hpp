/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skienzle <skienzle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/25 09:47:23 by skienzle          #+#    #+#             */
/*   Updated: 2022/03/25 09:55:27 by skienzle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cctype>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

#include "Server.hpp"
#include "Engine.hpp"

typedef std::vector<std::string> StringVector;

struct Location_setup
{
	std::string location;
	std::string root;
	std::string index;
	StringVector allowed_methods;
	StringVector allowed_scripts;
};

struct Server_setup
{
	std::string server_name;
	std::string root;
	std::string index;
	std::string error_pages;
	uint32_t ip_address;
	int port;
	uint32_t max_client_body_size;
	std::vector<Location_setup> locations;
};

class Config_parser
{
public: // methods
	Config_parser(const char *filename);
	~Config_parser();

	void run();


private: // methods
	Config_parser();
	Config_parser(const Config_parser& other);

	Config_parser& operator=(const Config_parser& other);


	std::string m_get_next_word();
	std::string m_get_next_word_protected(int line, bool is_on_same_line = true);


	std::pair<std::string, Server_setup> m_read_server();
	std::string m_read_location(Location_setup& location);
	// std::string m_get_next_word();
	// std::string m_get_next_word(int line, const char *error_msg = "", const char *strerr = "");
	// bool m_read_next_line(char delim = '\n');
	// char m_peek_next_char();

	int m_check_int(const std::string& word);
	uint32_t m_check_ip_address();

private: // subclass
	class Invalid_config: std::exception
	{
	public:
		Invalid_config(int line, const char *msg, const char *strerr = "");
		~Invalid_config() throw();
		const char *what() const throw();
	private:
		std::string m_errorMsg;
	};
private: // attributes
	std::vector<Server_setup>		m_servers;
	std::ifstream			m_infile;
	std::string				m_word;
	std::stringstream		m_lineStream;
	size_t					m_line_num;

	static bool				s_verbose;
};