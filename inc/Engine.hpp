#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <vector>
# include <algorithm>
# include <sys/event.h>
# include <utils.hpp>
# include <stdio.h>

# include <Socket.hpp>

# define ENGINE_BACKLOG 10

struct	s_kevent: public kevent
{
	bool	operator==( t_fd fd );
};

std::ostream	&operator<< ( std::ofstream & out, s_kevent const & in );

class Socket;

class Engine
{
	private:
		static bool						s_verbose;
		std::vector<s_kevent>			m_changes;
		std::vector<s_kevent>			m_events;
		std::vector<Socket>				m_sockets;
		int								m_kqueue;

		typedef	std::vector<Socket>::iterator	SockIter;
		typedef	std::vector<s_kevent>::iterator	KeventIter;

		s_kevent	*findByFd( t_fd, std::vector<s_kevent> vec );
		Socket		*findSock( t_fd );


	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		//ToDo: ErrorHandling

		void		initSockets( void );
		void		closeSockets( void );
		void		listenSockets( void );
		void		launch( void );


		void		debug( void );

		void		setRead( t_fd fd );


};

#endif
