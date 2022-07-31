#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <unistd.h>


#include "typedefs.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "DirectoryListing.hpp"

class Executer
{
public: // methods
	Executer(const Executer& other);
	Executer(const Server* server, const Server::Location* location,
				char** envp, const Request& req);
	Executer(const Server* server,  const Server::Location* location,
				char** envp, const Request& req, const std::string& filename);

	~Executer();



	void set_server(Server* server) { p_server = server; }


	std::string get_content() const { return m_content; }

	int get_status_code() const { return m_status_code; }

	std::string get_filename() const { return m_filename; }

	void run();

	std::string get_full_url() const;

	e_FileType get_file_type() const;

private: // methods
	Executer();

	Executer& operator=(const Executer& other);

	void run_server();
	void run_location();

	void run_cgi(e_FileType file_type);
	void run_directory_listing();
	void read_from_file();

	std::string find_dir(const std::string& name) const;

	static bool cgi_is_allowed(const StringArr& scripts, e_FileType type);

	static bool method_is_allowed(const StringArr& methods,
									const std::string& reqested_method);

	bool resource_exist();

	void put_handler();
	void delete_handler();

private: // attributes
	const Server*	p_server;
	const Server::Location* p_loc;
	char**			p_env;
	int				m_status_code;
	const Request&	m_req;
	std::string		m_content;
	std::string		m_filename;
	// std::string		m_url;

};