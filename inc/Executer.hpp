#pragma once

#include <string>
#include <map>

#include "typedefs.hpp"
#include "Server.hpp"
#include "Request.hpp"

class Executer
{
public: // methods

	Executer(const Executer& other);
	Executer(const Server* server, const Server::Location* location,
				const Request& req);
	Executer(const Server* server, const Server::Location* location,
				const Request& req, const std::string& filename);

	~Executer();

	const std::string&							get_content() const;
	int											get_status_code() const;
	const std::string&							get_filename() const;
	const std::map<std::string,std::string>&	get_cgi_header() const;
	std::string									get_full_url() const;
	bool										is_cgi() const;

	void set_server(Server* server);

	void run();


private: // methods

	Executer();

	Executer& operator=(const Executer& other);

	void	run_server();
	void	run_location();

	void	run_cgi(e_FileType file_type);
	void	run_directory_listing();

	void	server_method_execution();
	void	get_handler();
	void	post_handler();
	void	put_handler();
	void	delete_handler();

	e_FileType	get_file_type() const;
	bool		resource_exist() const;
	void		read_from_file();

	static bool cgi_is_allowed(const std::map<std::string,std::string>& scripts,
								e_FileType type);


private: // attributes

	const Server*						p_server;
	const Server::Location*				p_loc;
	int									m_status_code;
	const Request&						m_req;
	std::string							m_content;
	std::string							m_filename;
	bool								m_is_cgi;
	std::map<std::string, std::string>	m_cgi_header;

};
