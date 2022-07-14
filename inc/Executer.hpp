#pragma once

#include <sys/stat.h>


#include <iostream>
#include <string>
#include <fstream>


#include "typedefs.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Request.hpp"

class Executer
{
public: // methods
	Executer(const Executer& other);
	Executer(Server* server, char** envp, const Request& req);
	Executer(Server* server, char** envp, const Request& req, const std::string& filename);

	~Executer();



	void set_server(Server* server) { p_server = server; }


	std::string get_content() const { return m_content; }


	void run();

	std::string get_full_url() const;

	e_FileType get_file_type() const;

	Server::Location* find_location() const;
private: // methods
	Executer();

	Executer& operator=(const Executer& other);

	void read_from_file();

	void replace_dir(const Server::Location* loc);

	static std::string find_dir(const std::string& name);

	static bool is_dir(const std::string& name);

	static bool cgi_is_allowed(const StringArr& scripts, e_FileType type);


private: // attributes
	Server*			p_server;
	char**			p_env;
	int				m_status_code;
	const Request&	m_req;
	std::string		m_content;
	std::string		m_filename;
	// std::string		m_url;

};