#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include <sys/event.h>
# include <utils.hpp>
# include <stdio.h>

# include <Socket.hpp>
# include <Connect.hpp>
# include <Server.hpp>

# define ENGINE_BACKLOG 10


struct	s_kevent: public kevent
{
	bool	operator==( fd_type fd );
};

std::ostream	&operator<< ( std::ofstream & out, s_kevent const & in );

enum	Type { SOCKET, CONNECTION };

struct ObjPtr
{
	Type	type;
	void	*ptr;
};

class Socket;
class Connect;

class Engine
{
	private:
		std::vector<s_kevent>			m_changes;
		std::vector<s_kevent>			m_events;
		std::map<fd_type, Socket>			m_sockets;
		std::map<fd_type, Connect>			m_connects;
		std::vector<Server>				m_servers;
		int								m_kqueue;

		typedef	std::map<fd_type, Socket>::iterator	SockIter;
		typedef	std::map<fd_type, Connect>::iterator	CnctIter;
		typedef	std::vector<s_kevent>::iterator		KeventIter;

		void		socketEvent( s_kevent & kevent  );
		void		connectEvent( s_kevent & kevent );
		void		acceptConnect( Socket & sock );
		void		closeConnects( void );
		void		debug( void );
		void		setKevent( fd_type fd, int16_t filter, uint16_t flag );
		void		closeSockets( void );
		void		listenSockets( void );

		void		assignServer( Connect &connect );

	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		//ToDo: ErrorHandling

		void		initSockets( void );
		void		initServers( void );
		void		launch( void );



};

#endif
