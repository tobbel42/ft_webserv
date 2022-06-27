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
		std::map<fd_type, Socket>		m_sockets;
		std::map<fd_type, Connect>		m_connects;
		ServerArr						m_servers; // will be populated by the ConfigParser
		std::map<fd_type, std::time_t>	m_timers;
		int								m_kqueue;

		typedef	std::map<fd_type, Socket>::iterator			SockIter;
		typedef	std::map<fd_type, Connect>::iterator		CnctIter;
		typedef	std::vector<s_kevent>::iterator				KeventIter;
		typedef std::map<fd_type, std::time_t>::iterator	TimerIter;



		void		socketEvent( s_kevent & kevent  );
		void		connectEvent( s_kevent & kevent );
		void		acceptConnect( Socket & sock );
		void		closeConnects( void );
		void		debug( void );
		void		setKevent( fd_type fd, int16_t filter, uint16_t flag );
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
