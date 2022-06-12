

#include <cstdint>

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>
#include <iostream>

typedef std::vector<std::string> StringVector;

class LocationSetup;


class ServerSetup
{
public: // methods
	ServerSetup();
	ServerSetup(const ServerSetup& other);
	~ServerSetup() {};

	ServerSetup& operator=(const ServerSetup& other);

	bool set_server_name(const std::string& name);
	bool set_root(const std::string& word);
	bool set_index(const std::string& word);
	bool set_error_pages(const std::string& word);
	bool set_ip_address(uint32_t ip);
	bool set_port(uint32_t port);
	bool set_max_client_body_size(uint32_t n);


	const char* check_attributes() const;

public: // attributes
	StringVector server_names;
	std::string root;
	std::string index;
	std::string error_pages;
	uint32_t ip_address;
	std::vector<uint32_t> ports;
	uint32_t max_client_body_size;
	std::vector<LocationSetup> locations;

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


class LocationSetup
{
public: // methods
	LocationSetup();
	LocationSetup(const LocationSetup& other);
	~LocationSetup() {}

	LocationSetup& operator=(const LocationSetup& other);


	bool set_root(const std::string& word);
	bool set_index(const std::string& word);
	bool set_method(const std::string& method);
	bool set_script(const std::string& script);
	const char* set_directory_listing(const std::string& state);

	const char* check_attributes() const;


public: // attributes
	std::string location;
	std::string root;
	std::string index;
	StringVector allowed_methods;
	StringVector allowed_scripts;
	bool directory_listing_enabled;

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


std::ostream& operator<<(std::ostream& out, const LocationSetup& rhs);

std::ostream& operator<<(std::ostream& out, const ServerSetup& rhs);