#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <utils.hpp>


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