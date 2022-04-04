#include "../inc/Engine.hpp"

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

Engine	
&Engine::operator = ( const Engine &rhs )
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
bool s_kevent::operator==( t_fd fd )
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

/*
//in here all the socket binding magic should happen, for now just a dummy
*/
void
Engine::initSockets( void )
{
	Socket	newSock(INADDR_ANY, 8080);
	m_sockets.insert(std::pair<t_fd, Socket>(newSock.getSockFd(), newSock));
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
Engine::setKevent( t_fd fd, int16_t filter, uint16_t flag)
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
Engine::acceptConnect( Socket sock )
{
	t_fd	fd = sock.acceptConnect();

	Connect	newConnect(fd);

	m_connects.insert(std::pair<t_fd, Connect>(fd, newConnect));
	this->setKevent(fd, EVFILT_READ, EV_ADD);
}

void
Engine::socketEvent( t_fd fd )
{
	SockIter	iter = m_sockets.find(fd);

	if (iter == m_sockets.end())
		return ;
	std::cout << "Socket Event" << std::endl;
	this->acceptConnect(iter->second);
}

void
Engine::connectEvent( t_fd fd )
{
	CnctIter	iter = m_connects.find(fd);

	if (iter == m_connects.end())
		return ;
	std::cout << "Connect Event" << std::endl;
}


/*
//Main loop of the Socket.
*/
void
Engine::launch( void )
{
	//init kqueue
	m_kqueue = kqueue();

	//start to listen to Socket, add the Sockets to the kqueue
	listenSockets();
	
	int	n_events = 0;

	//the main Socket loop
	while (true)
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
			socketEvent(m_events[i].ident);
			connectEvent(m_events[i].ident);
		}
		sleep(2);
				
	}
	//closing of sockets is happening in the Engine destructor
}

