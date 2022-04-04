#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <vector>
# include <map>
# include <algorithm>
# include <sys/event.h>
# include <utils.hpp>
# include <stdio.h>

# include <Socket.hpp>
# include <Connect.hpp>

# define ENGINE_BACKLOG 10

struct	s_kevent: public kevent
{
	bool	operator==( t_fd fd );
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
		std::map<t_fd, Socket>			m_sockets;
		std::map<t_fd, Connect>			m_connects;
		int								m_kqueue;

		typedef	std::map<t_fd, Socket>::iterator	SockIter;
		typedef	std::map<t_fd, Connect>::iterator	CnctIter;	
		typedef	std::vector<s_kevent>::iterator		KeventIter;


		void		socketEvent( t_fd fd );
		void		connectEvent( t_fd fd );
		void		acceptConnect( Socket sock );
		void		closeConnects( void );
		void		debug( void );
		void		setKevent( t_fd fd, int16_t filter, uint16_t flag );
		void		closeSockets( void );
		void		listenSockets( void );

	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		//ToDo: ErrorHandling

		void		initSockets( void );
		void		launch( void );



};

#endif
