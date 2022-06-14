/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                         :+:      :+:    :+:   */
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

#include <limits>
#include <string>
#include <exception>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

#include "Setup.hpp"
#include "utils.hpp"

typedef std::vector<std::string> StringVector;


class ConfigParser
{
public: // methods
	ConfigParser();
	ConfigParser(const char *filename);
	~ConfigParser();

	void assign_file(const char *filename);

	void run();

	inline const std::vector<ServerSetup>& getServerSetup() const { return m_servers; }

private: // methods
	ConfigParser(const ConfigParser& other);

	ConfigParser& operator=(const ConfigParser& other);


	std::string m_get_next_word();
	std::string m_get_next_word_protected(bool is_on_same_line = true);


	std::pair<std::string, ServerSetup> m_read_server();
	std::string m_read_location(LocationSetup& location);

	uint32_t m_check_int(const std::string& word);
	uint32_t m_check_ip_address();

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
	std::vector<ServerSetup>		m_servers;
	std::ifstream					m_infile;
	std::stringstream				m_line_stream;
	size_t							m_line_number;
};