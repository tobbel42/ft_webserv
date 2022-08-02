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
				const Request& req);
	Executer(const Server* server, const Server::Location* location,
				const Request& req, const std::string& filename);

	~Executer();



	void set_server(Server* server) { p_server = server; }


	std::string get_content() const { return m_content; }

	int get_status_code() const { return m_status_code; }

	std::string get_filename() const { return m_filename; }

	void run();

	std::string get_full_url() const;

	e_FileType get_file_type() const;

	bool is_cgi() const { return m_is_cgi; }

	const std::map<std::string, std::string> &
	get_cgi_header() const { return m_cgi_header; }

private: // methods
	Executer();

	Executer& operator=(const Executer& other);

	void run_server();
	void run_location();

	void run_cgi(e_FileType file_type);
	void run_directory_listing();
	void read_from_file();

	std::string find_dir(const std::string& name) const;

	static bool cgi_is_allowed(const std::map<std::string,std::string>& scripts,
								e_FileType type);

	bool resource_exist();

	void server_method_execution();

	void get_handler();
	void post_handler();
	void put_handler();
	void delete_handler();


private: // attributes
	const Server*	p_server;
	const Server::Location* p_loc;
	int				m_status_code;
	const Request&	m_req;
	std::string		m_content;
	std::string		m_filename;


	bool m_is_cgi;
	std::map<std::string, std::string> m_cgi_header;

};