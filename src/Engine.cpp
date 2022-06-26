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

/*
//overloading this operator to use find to search s_kevent by id
*/
bool s_kevent::operator==( fd_type fd )
{
	return (this->ident == fd);
}

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
		this->setKevent(iter->first, EVFILT_READ, EV_ADD);
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

void
Engine::setKevent( fd_type fd, int16_t filter, uint16_t flag)
{
	s_kevent	event;

	EV_SET(&event, fd, filter, flag, 0, 0, 0);
	m_changes.push_back(event);
	m_events.resize(m_changes.size());
}

void
Engine::debug( void )
{
	std::cout << "\n\nEvents:" << std::endl;
	for (KeventIter iter = m_events.begin(); iter != m_events.end(); ++iter)
		std::cout << *iter << std::endl;
	std::cout << "\n\nChanges:" << std::endl;
	for (KeventIter iter = m_changes.begin(); iter != m_changes.end(); ++iter)
		std::cout << *iter << std::endl;
}

void
Engine::acceptConnect( Socket & sock )
{
	fd_type	fd = sock.acceptConnect();

	Connect	newConnect(fd, sock.getIp(), sock.getPort());

	m_connects.insert(std::pair<fd_type, Connect>(fd, newConnect));
	this->setKevent(fd, EVFILT_READ, EV_ADD);
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
	// SockIter iter = m_sockets.find(connect.getSockFd());

	

	// //Socket not found
	// if (iter == m_sockets.end())
	// {
	// 	//internal error 500
	// 	return ;
	// }

	// connect.setServer((*iter).second.getServer(""));

	//smart server find fknt 

	Server* server = find_server(connect);
	if (server == nullptr)
		connect.set_status(404);
	else
		connect.setServer(server);
}

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

	//A connection can have multible states
	//on read we read as many bytes, as in the kevent specified
	//ToDo:
	// parse the read data( is it chunked, does is need multible read cycles)
	// we need to assign a server to the connection
	// depending on the request we 
	if (cnct.getAction() == READ)
	{
		//read the Request: ToDo Handling of chuncked requests(multible reads and concatenate)
		if (cnct.readRequest(kevent))
		{
		//if read done 
			//removing the read event from the from the change vector
			m_changes.erase(std::find(m_changes.begin(), m_changes.end(), kevent.ident));

			//assign a server, if not yet given (search for Hostname in Socket servers/ default server)
			if (cnct.getServer() == NULL)
				assignServer(cnct);

			//formulate the request (result or error), chunking of big responses
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


/*
//Main loop of the Socket.
*/
void
Engine::launch()
{
	//init kqueue
	m_kqueue = kqueue();

	//start to listen to Socket, add the Sockets to the kqueue
	listenSockets();
	
	int	n_events = 0;

	//the main Socket loop
	while (0b00101010)
	{
		n_events = kevent(m_kqueue,
			&(*m_changes.begin()), m_changes.size(),
			&(*m_events.begin()), m_events.size(),
			NULL);

		#ifdef VERBOSE
		{
			std::cout << "\nEvents: " << n_events << std::endl;
			this->debug();
		}
		#endif

		for	(int i = 0; i < n_events; ++i)
		{
			socketEvent(m_events[i]);
			connectEvent(m_events[i]);
		}
				
	}
	//closing of sockets is happening in the Engine destructor
}

