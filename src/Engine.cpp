#include "../inc/Engine.hpp"

// ToDo: initialisation of all the member attributes

Engine::Engine( void )
{
	#ifdef VERBOSE
		std::cout << "Engine: Default Constructor called" << std::endl;
	#endif
}

Engine::~Engine( void )
{
	#ifdef VERBOSE
		std::cout << "Engine: Destructor called" << std::endl;
	#endif
	this->closeConnects();
	this->closeSockets();
}

Engine::Engine( const Engine &copy )
{
	#ifdef VERBOSE
		std::cout << "Engine: Copy Constructor called" << std::endl;
	#endif
	*this = copy;
}

Engine &	
Engine::operator = ( const Engine &rhs )
{
	#ifdef VERBOSE
		std::cout << "Engine: Assignation operator called" << std::endl;
	#endif
	(void)rhs;
	return (*this);
}

#ifdef KQUEUE
std::ostream &
operator<< ( std::ostream & out, s_kevent const & in )
{
	out << "\nident:\t" << in.ident;
	out << "\nfilter:\t" << in.filter;
	out << "\nflags:\t" << std::bitset<16>(in.flags);
	out << "\nfflags:\t" << in.fflags;
	out << "\ndata:\t" << in.data;
	out << "\nudata:\t" << in.udata;
	return( out );
}
#else
std::ostream &
operator<<(std::ostream & out, s_pollfd const & in) {
	out << "\nfd:\t\t" << in.fd;
	out << "\nevents:\t" << in.events;
	out << "\nrevents:\t" << in.revents;
	return out;
}
#endif


ServerArr&
Engine::getServers()
{
	return m_servers;
}

/*
@brief initializes all the unique host:port combinations of the servers
		and saves them in a map
*/
void
Engine::initSockets( void )
{
	for (size_t i = 0; i < m_servers.size(); ++i)
	{
		const Server& server = m_servers[i];
		for (size_t j = 0; j < server.ports.size(); ++j)
		{
			Socket	newSock(server.ip_address, server.ports[j]);
			m_sockets.insert(std::make_pair(newSock.getSockFd(), newSock));
		}
	}
}

void
Engine::listenSockets( void )
{
	int		errFlag;
	for (SockIter iter = m_sockets.begin(); iter != m_sockets.end(); ++iter)
	{
		errFlag = listen(iter->first, ENGINE_BACKLOG);
		if (errFlag)
		{
			std::cerr << "Listen error: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		#ifdef KQUEUE
			setKevent(iter->first, EVFILT_READ, EV_ADD);
		#else
			setPoll(iter->first, POLLIN);
		#endif
	}
}

void
Engine::closeSockets( void )
{
	for (SockIter iter = m_sockets.begin(); iter != m_sockets.end(); ++iter)
		close(iter->first);
}

void
Engine::closeConnects( void )
{
	for (CnctIter iter = m_connects.begin(); iter != m_connects.end(); ++iter)
		close(iter->first);
}

#ifdef KQUEUE
void
Engine::setKevent( fd_type fd, int16_t filter, uint16_t flag)
{
	s_kevent	event;

	EV_SET(&event, fd, filter, flag, 0, 0, 0);
	m_changes.push_back(event);
	m_events.resize(m_changes.size());
}
#else
void
Engine::setPoll(fd_type fd, short events) {
	s_pollfd	poll;

	poll.fd = fd;
	poll.events = events;
	m_polls.push_back(poll);
}
#endif

void
Engine::debug( void )
{
	#ifdef KQUEUE
	std::cout << "\n\nEvents:" << std::endl;
	for (KeventIter iter = m_events.begin(); iter != m_events.end(); ++iter)
		std::cout << *iter << std::endl;
	std::cout << "\n\nChanges:" << std::endl;
	for (KeventIter iter = m_changes.begin(); iter != m_changes.end(); ++iter)
		std::cout << *iter << std::endl;
	#else
	PRINT("\n\nPOLLS:");
	for (PollIter iter = m_polls.begin(); iter != m_polls.end(); ++iter)
		PRINT(*iter);
	#endif
}

void
Engine::acceptConnect( Socket & sock )
{
	fd_type	fd = sock.acceptConnect();

	if (fd == -1)
	{
		EPRINT("error: failed to accept connection");
		return ;
	}

	Connect	newConnect(fd, sock.getIp(), sock.getPort());

	m_connects.insert(std::pair<fd_type, Connect>(fd, newConnect));

	m_timers.insert(std::make_pair(fd, std::time(nullptr)));

	#ifdef KQUEUE
	setKevent(fd, EVFILT_READ, EV_ADD);
	#else
	setPoll(fd, POLLIN);
	#endif
}

Server*
Engine::find_server(const Connect& cnct)
{
	Server* default_server = nullptr;
	for (size_t i = 0; i < m_servers.size(); ++i)
	{
		// look for a matching ip:port combination
		Server& server = m_servers[i];

		if (cnct.getIp() == server.ip_address)
		{
			for (size_t j = 0; j < server.ports.size(); ++j)
			{
				uint32_t port = server.ports[j];

				if (port == cnct.getPort())
				{
					// the first one is the default server
					if (default_server == nullptr)
						default_server = &server;

					// look for a matching server_name
					for (size_t k = 0; k < server.server_names.size(); ++k)
					{
						if (cnct.get_hostname() == server.server_names[k])
							return &server;
					}
				}
			}
		}
	}
	return default_server;
}


void
Engine::assignServer( Connect &connect )
{
	Server* server = find_server(connect);
	if (server == nullptr)
		connect.set_status(404);
	else
		connect.setServer(server);
}

#ifdef KQUEUE
void
Engine::socketEvent( s_kevent & kevent )
{
	SockIter	iter = m_sockets.find(kevent.ident);

	if (iter == m_sockets.end())
		return ;
	#ifdef VERBOSE
		std::cout << "Socket Event" << std::endl;
	#endif
	this->acceptConnect(iter->second);
}
#else
void
Engine::socketEvent(s_pollfd & poll)
{
	SockIter	iter = m_sockets.find(poll.fd);

	if (iter == m_sockets.end())
		return ;
	#ifdef VERBOSE
		std::cout << "Socket Event" << std::endl;
	#endif
	this->acceptConnect(iter->second);
}
#endif

#ifdef KQUEUE
void
Engine::connectEvent( s_kevent & kevent )
{
	//check if the FD is linked to a connection
	CnctIter	iter = m_connects.find(kevent.ident);

	if (iter == m_connects.end())
		return ;

	#ifdef VERBOSE
		std::cout << "ConnectEvent" << std::endl;
	#endif

	//eyeCandy

	Connect	& cnct = iter->second;

	//connection can have multible states

	//on read we read as many bytes, as in the kevent specified
	if (cnct.getAction() == READ)
	{
		m_timers[cnct.getFd()] = std::time(nullptr);

		//read the Request: ToDo Handling of chuncked requests(multible reads and concatenate)
		if (cnct.readRequest(kevent))
		{
			//removing the read event from the from the change vector
			m_changes.erase(std::find(m_changes.begin(), m_changes.end(), kevent.ident));
			//remove the fd from the timerlist
			m_timers.erase(cnct.getFd());

			//assign a server, if not yet given
			//(search for Hostname in Socket servers/ default server)
			if (cnct.getServer() == NULL)
				assignServer(cnct);

			//formulate the request (result or error)
			cnct.composeResponse();

			//adding the write event to the change vector
			setKevent(kevent.ident, EVFILT_WRITE, EV_ADD);
		}
	}
	//On write 
	else if (cnct.getAction() == WRITE)
	{
		//write the response
		cnct.writeResponse(kevent);

		//if write done
			//remove the event form the change vector
			m_changes.erase(std::find(m_changes.begin(), m_changes.end(), kevent.ident));

			//close the connection
			close(cnct.getFd());

			//remove connection from FD pool
			m_connects.erase(iter);
	}
}
#else
void
Engine::connectEvent(s_pollfd & poll)
{
	//check if the FD is linked to a connection
	CnctIter	iter = m_connects.find(poll.fd);

	if (iter == m_connects.end())
		return ;

	#ifdef VERBOSE
		std::cout << "ConnectEvent" << std::endl;
	#endif

	//eyeCandy

	Connect	& cnct = iter->second;

	//connection can have multible states

	//on read we read as many bytes, as in the kevent specified
	if (cnct.getAction() == READ)
	{
		m_timers[cnct.getFd()] = std::time(nullptr);

		//read the Request: ToDo Handling of chuncked requests(multible reads and concatenate)
		if (cnct.readRequest(poll))
		{
			poll.events = POLLOUT;

			//remove the fd from the timerlist
			m_timers.erase(cnct.getFd());

			//assign a server, if not yet given
			//(search for Hostname in Socket servers/ default server)
			if (cnct.getServer() == NULL)
				assignServer(cnct);

			//formulate the request (result or error)
			cnct.composeResponse();

		}
	}
	//On write 
	else if (cnct.getAction() == WRITE)
	{
		//write the response
		cnct.writeResponse(poll);

		//if write done
			//remove the event form the change vector
			m_polls.erase(std::find(m_polls.begin(), m_polls.end(), poll.fd));

			//close the connection
			close(cnct.getFd());

			//remove connection from FD pool
			m_connects.erase(iter);
	}
}
#endif

void
Engine::check_for_timeout() {
	std::time_t	time = std::time(nullptr);
	TimerIter 	i = m_timers.begin();
	fd_type 	fd;

	while (i != m_timers.end()) {
		if (i->second < time - CNCT_TIMEOUT)
		{
			fd = i->first;
			++i;
			close(fd);
			#ifdef KQUEUE
			m_changes.erase(std::find(m_changes.begin(), m_changes.end(), fd));
			#else
			m_polls.erase(std::find(m_polls.begin(), m_polls.end(), fd));
			#endif
			m_connects.erase(fd);
			m_timers.erase(fd);
		}
		else
			++i;
	}
}


/*
//Main loop of the Socket.
*/
void
Engine::launch()
{
	#ifdef KQUEUE
	//init kqueue
	m_kqueue = kqueue();
	#endif

	//start to listen to Socket, add the Sockets to the kqueue
	listenSockets();
	
	int	n_events = 0;
	#ifdef KQUEUE
	struct timespec timeout = {1, 0};
	#else

	#endif

	//the main Socket loop
	while (0b00101010)
	{
		#ifdef KQUEUE
		n_events = kevent(m_kqueue,
			&(*m_changes.begin()), m_changes.size(),
			&(*m_events.begin()), m_events.size(),
			&timeout);
		#else
		n_events = poll(&(*m_polls.begin()), m_polls.size(), 1000);
		#endif

		#ifdef VERBOSE
		{
			std::cout << "\nEvents: " << n_events << std::endl;
			this->debug();
		}
		#endif

		#ifdef KQUEUE
		for	(int i = 0; i < n_events; ++i)
		{
			socketEvent(m_events[i]);
			connectEvent(m_events[i]);
		}
		#else
		if (n_events > 0) {
			for (size_t i = 0; i < m_polls.size(); ++i) {
				if (m_polls[i].revents == m_polls[i].events) {
					socketEvent(m_polls[i]);
					connectEvent(m_polls[i]);
				}
			}
		}
		#endif
		check_for_timeout();
	}
	//closing of sockets is happening in the Engine destructor
}

