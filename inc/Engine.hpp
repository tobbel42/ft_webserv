#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <vector>
# include <algorithm>
# include <sys/event.h>
# include <utils.hpp>
# include <stdio.h>

# include <Server.hpp>

# define ENGINE_BACKLOG 10

struct	s_kevent: public kevent
{
	bool	operator==( t_fd fd );
};

std::ostream	&operator<< ( std::ofstream & out, s_kevent const & in );

class Server;

class Engine
{
	private:
		static bool						m_verbose;
		std::vector<s_kevent>			m_changes;
		std::vector<s_kevent>			m_events;
		std::vector<Server>				m_servers;
		int								m_kqueue;

		typedef	std::vector<Server>::iterator	ServIter;
		typedef	std::vector<s_kevent>::iterator	KeventIter;

		s_kevent	*findByFd( t_fd, std::vector<s_kevent> vec );
		Server		*findServ( t_fd );


	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		//ToDo: ErrorHandling

		void		initServers( void );
		void		closeServers( void );
		void		listenServers( void );
		void		launch( void );


		void		debug( void );

		void		setRead( t_fd fd );


};

#endif
