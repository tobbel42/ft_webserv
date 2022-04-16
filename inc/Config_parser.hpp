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

#include <string>
#include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Server.hpp"
#include "Engine.hpp"

typedef std::vector<std::string> StringVector;

struct Location_setup
{
	std::string path;

};

struct Server_setup
{
	std::string server_name;
	std::string root;
	std::string index;
	int ip_address;
	std::vector<int> ports;
	StringVector allowed_methods;
	unsigned int max_client_body_size;
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

	Server_setup m_read_server();
	std::string m_get_next_word(char delim = ';');

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

	static bool				s_verbose;
};