#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <iostream>

#include "typedefs.hpp"


class Server
{
public: // methods

	Server();
	Server(const Server& other);
	~Server() {}

	Server& operator=(const Server& other);


	bool set_server_name(const std::string& name);
	bool set_root(const std::string& word);
	bool set_index(const std::string& word);
	bool set_error_pages(const std::string& word);
	bool set_ip_address(unsigned int ip);
	bool set_port(unsigned int port);
	bool set_method(const std::string& method);
	bool set_max_client_body_size(unsigned int n);

	const char* check_attributes() const;

	std::string	getDirectory( void ) const;
	fd_type		getSocket( void ) const;
	std::string getHostname( void ) const;


public: // attributes

	StringArr				server_names;
	std::string				root;
	std::string				index;
	std::string				error_pages;
	unsigned int				ip_address;
	std::vector<unsigned int>	ports;
	unsigned int				max_client_body_size;
	StringArr				allowed_methods;

	class Location;
	std::vector<Location>	locations;


public: // subclass

	class Location
	{
	public: // methods
		Location();
		Location(const Location& other);
		~Location() {}

		Location& operator=(const Location& other);


		bool set_root(const std::string& word);
		bool set_index(const std::string& word);
		bool set_script(const std::string& script);
		const char* set_directory_listing(const std::string& state);

		const char* check_attributes() const;


	public: // attributes
		std::string		location;
		std::string		root;
		std::string		index;
		StringArr		allowed_scripts;
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
			DIRECTORY_LISTING
		};
	};


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
		MAX_CLIENT_BODY_SIZE
	};
};


std::ostream& operator<<(std::ostream& out, const Server::Location& rhs);

std::ostream& operator<<(std::ostream& out, const Server& rhs);