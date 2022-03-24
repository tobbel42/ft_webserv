#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <vector>
# include <algorithm>
# include <sys/event.h>
# include <Server.hpp>
# include <utils.hpp>

# define ENGINE_MAX_PENDING_CONNECTIONS 10

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

		struct s_kevent
		*findByFd( t_fd, std::vector<s_kevent> vec );

	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		void		initServers( void );
		void		launch( void );

		void		debug( void );

		void		setRead( t_fd fd );


};

#endif
