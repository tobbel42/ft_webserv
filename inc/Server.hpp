#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iostream>

#include "typedefs.hpp"

/*
The server class is a dataclass containing all the information of
one server block in the config file.
The setters perform the first group of checks on the input data
and check_attributes() checks all the mandatory parts of the
class in the end
*/
class Server
{
public: // methods

	Server();
	Server(const Server& other);
	~Server();

	Server& operator=(const Server& other);

	bool	set_server_name(const std::string& name);
	bool	set_root(const std::string& word);
	bool	set_index(const std::string& word);
	bool	set_error_pages(const std::string& word);
	bool	set_ip_address(uint32_t ip);
	bool	set_port(uint32_t port);
	bool	set_method(const std::string& method);
	// key = redirected file, value = redirection target
	bool	set_redirection(const std::pair<std::string,std::string>& file_pr);
	bool	set_max_client_body_size(uint32_t n);

	const char*	check_attributes() const;


public: // attributes

	StringArr							server_names;
	std::string							root;
	std::string							index;
	std::string							error_pages;
	uint32_t							ip_address;
	std::vector<uint32_t>				ports;
	StringArr							allowed_methods;
	uint32_t							max_client_body_size;
	std::map<std::string,std::string>	redirections;

	class Location;
	std::vector<Location>	locations;


private: // attributes
	std::vector<bool> m_checks;

private: // enum
	enum e_server_options
	{
		SERVER_NAME = 0,
		ROOT,
		INDEX,
		ERROR_PAGES,
		IP_ADDRESS,
		PORT,
		ALLOWED_METHODS,
		REDIRECTIONS,
		MAX_CLIENT_BODY_SIZE
	};


public: // subclass
	/* 
	In the same way as the server class resembles one server block
	the location class resembles all data of one location block.
	Setters and check_attributes() work in the same way as in the
	server class
	*/
	class Location
	{
	public: // methods
		Location();
		Location(const Location& other);
		~Location();

		Location& operator=(const Location& other);

		bool		set_prefix(const std::string& word);
		bool		set_root(const std::string& word);
		bool		set_index(const std::string& word);
		bool		set_method(const std::string& method);
		bool		set_script(const std::pair<std::string,std::string>& script);
		bool		set_redirection(const std::pair<std::string,std::string>& file_pr);
		bool		set_max_client_body_size(uint32_t n);
		const char*	set_dir_listing(const std::string& state);

		const char*	check_attributes() const;


	public: // attributes

		std::string		prefix;
		std::string		root;
		std::string		index;
		StringArr		allowed_methods;
		// key = script identifier, value = script executable
		std::map<std::string,std::string> scripts;
		// key = redirected file, value = redirection target
		std::map<std::string,std::string> redirections;
		uint32_t		max_client_body_size;
		bool			directory_listing_enabled;


	private: // attributes
		std::vector<bool> m_checks;

	private: // enum
		enum e_location_options
		{
			ROOT = 0,
			INDEX,
			ALLOWED_METHODS,
			ALLOWED_SCRIPTS,
			REDIRECTIONS,
			MAX_CLIENT_BODY_SIZE,
			DIRECTORY_LISTING
		};
	};

};

std::ostream& operator<<(std::ostream& out, const Server::Location& rhs);

std::ostream& operator<<(std::ostream& out, const Server& rhs);
