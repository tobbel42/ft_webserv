#ifndef ENGINE_HPP
# define ENGINE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <ctime>

#include <Socket.hpp>
#include <Connect.hpp>
#include <Server.hpp>
#include <utils.hpp>
#include "typedefs.hpp"

#include <poll.h>

#ifdef KQUEUE
std::ostream	&operator<< ( std::ofstream & out, s_kevent const & in );
#else
std::ostream	&operator<<(std::ostream & out, s_pollfd const & in);
#endif

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

	#ifdef KQUEUE
		int								m_kqueue;
		std::vector<s_kevent>			m_changes;
		std::vector<s_kevent>			m_events;

		typedef	std::vector<s_kevent>::iterator	KeventIter;

		void		setKevent( fd_type fd, int16_t filter, uint16_t flag );
		void		socketEvent( s_kevent & kevent  );
		void		connectEvent( s_kevent & kevent );
	#else
		std::vector<s_pollfd>			m_polls;

		typedef std::vector<s_pollfd>::iterator	PollIter;

		void		setPoll(fd_type fd, short events);
		void		socketEvent(s_pollfd & poll);
		void		connectEvent(s_pollfd & poll);
	#endif

		std::map<fd_type, Socket>		m_sockets;
		std::map<fd_type, Connect>		m_connects;
		ServerArr						m_servers; // will be populated by the ConfigParser
		std::map<fd_type, std::time_t>	m_timers;

		typedef	std::map<fd_type, Socket>::iterator			SockIter;
		typedef	std::map<fd_type, Connect>::iterator		CnctIter;
		typedef std::map<fd_type, std::time_t>::iterator	TimerIter;



		void		acceptConnect( Socket & sock );
		void		closeConnects( void );
		void		debug( void );
		void		closeSockets( void );
		void		listenSockets( void );

		void		assignServer( Connect &connect );
		Server*		find_server(const Connect& cnct);

		void		check_for_timeout();

	public:
		Engine( void );
		~Engine( void );
		Engine( const Engine &copy );
		Engine	&operator=( const Engine &rhs );

		ServerArr&	getServers();

		//ToDo: ErrorHandling

		void		initSockets( void );
		void		launch( void );


};

#endif
