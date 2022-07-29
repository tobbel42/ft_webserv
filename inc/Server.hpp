#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <iostream>

#include "typedefs.hpp"
#include "utils.hpp"


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
	bool set_ip_address(uint32_t ip);
	bool set_port(uint32_t port);
	bool set_max_client_body_size(uint32_t n);

	const char* check_attributes() const;

	std::string	getDirectory( void ) const;
	fd_type		getSocket( void ) const;
	std::string getHostname( void ) const;


public: // attributes

	StringArr				server_names;
	std::string				root;
	std::string				index;
	std::string				error_pages;
	uint32_t				ip_address;
	std::vector<uint32_t>	ports;
	uint32_t				max_client_body_size;

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

		bool set_prefix(const std::string& word);
		bool set_root(const std::string& word);
		bool set_index(const std::string& word);
		bool set_script(const std::string& script);
		bool set_method(const std::string& method);
		const char* set_directory_listing(const std::string& state);

		const char* check_attributes() const;


	public: // attributes
		std::string		prefix;
		std::string		root;
		std::string		index;
		StringArr		allowed_scripts;
		StringArr		allowed_methods;
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