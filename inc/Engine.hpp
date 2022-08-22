#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <ctime>

#include "Socket.hpp"
#include "Connect.hpp"
#include "Server.hpp"
#include "typedefs.hpp"


#ifdef KQUEUE
std::ostream	&operator<<(std::ofstream & out, s_kevent const & in);
#else
std::ostream	&operator<<(std::ostream & out, s_pollfd const & in);
#endif

enum	Type { SOCKET, CONNECTION };

/*
The engine class is the heart of our project.
It manages all sockets, the connections of all fds kqueue/poll
made available and maps a server with matching host:port combination
or server name to the connections
*/
class Engine
{
public:
	/*Constructors------------------------------------------------------------*/
	Engine();
	~Engine();

public:
	/*UserInterface-----------------------------------------------------------*/

	ServerArr&	get_servers();

	bool		init_sockets();
	bool		launch();


private:
	/*MemberVariabels---------------------------------------------------------*/

	#ifdef KQUEUE
	fd_type							m_kqueue;
	std::vector<s_kevent>			m_changes;
	std::vector<s_kevent>			m_events;
	#else
	std::vector<s_pollfd>			m_polls;
	#endif

	std::map<fd_type, Socket>		m_sockets;
	std::map<fd_type, Connect>		m_connects;
	// will be populated by the ConfigParser
	ServerArr						m_servers;
	std::map<fd_type, std::time_t>	m_timers;

	StringMap						m_cookie_base;


private:
	/*Typedefs----------------------------------------------------------------*/

	#ifdef KQUEUE
	typedef	std::vector<s_kevent>::iterator				KeventIter;
	#else
	typedef std::vector<s_pollfd>::iterator				PollIter;
	#endif

	typedef	std::map<fd_type, Socket>::iterator			SockIter;
	typedef	std::map<fd_type, Connect>::iterator		CnctIter;
	typedef std::map<fd_type, std::time_t>::iterator	TimerIter;


private:
	/*InternalMemberFunctions-------------------------------------------------*/
	#ifdef KQUEUE
	void		set_kevent(fd_type fd, int16_t filter, uint16_t flag );
	void		socket_event(s_kevent & kevent);
	void		connect_event(s_kevent & kevent);
	void		connect_read(s_kevent & kevent, Connect & cnct);
	void		connect_write(s_kevent & kevent, Connect & cnct);
	#else
	void		set_poll(fd_type fd, short events);
	void		socket_event(s_pollfd & poll);
	void		connect_event(s_pollfd & poll);
	void		connect_read(s_pollfd & poll, Connect & cnct);
	void		connect_write(s_pollfd & poll, Connect & cnct);
	#endif

	void		drop_cnct(fd_type ident);
	void		accept_connect(Socket & sock);
	void		close_connects();
	void		close_sockets();
	bool		listen_sockets();
	void		assign_server(Connect & cnct);
	Server*		find_server(const Connect & cnct);
	void		check_for_timeout();


private:
	/*Debug-------------------------------------------------------------------*/

	void		debug();

	void		print_start_msg();

	/*ControllStuff-----------------------------------------------------------*/

	bool		user_event();


private:
	Engine(const Engine &copy);
	Engine	&operator=(const Engine &rhs);
};
