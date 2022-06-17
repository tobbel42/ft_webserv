#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utils.hpp>
#include "typedefs.hpp"

/*
//The server class is best understood as an ruleset how a given connection
//handels incomming requests, the rules are specified in the config files
//ToDo: pipeline data from the configfile
//		even more rules are needed
*/
class Server
{
	private:
		std::string	m_hostname;

		std::string	m_directory;

		uint32_t m_ip;
		uint32_t m_port;

		bool	m_defaultServer;

		//ERRCODE + FIlEPATH
		std::map<uint32_t, std::string> m_defaultErrorPages;

		size_t	m_clientBodySize;

		//GET POST etc.
		std::vector<std::string> m_allowedMethodes;

		std::map<std::string, std::string> m_redirections;

		bool m_directoryListing;

		//path to file
		std::string m_defaultDirectoryFile;

		//list all file extensions like .py .php
		std::vector<std::string> m_cgiFileExtensions;
		
		//maybe todo ->fileupload enable + path 


	public:
		Server( void );
		Server( std::string hostname, std::string directory);
		~Server();
		Server( const Server &cpy );

		Server	&operator=( const Server &rhs );

		std::string	getDirectory( void ) const;
		t_fd		getSocket( void ) const;
		std::string getHostname( void ) const;
		
};

#endif