#include "../inc/Engine.hpp"

#ifdef VERBOSE
	bool Engine::s_verbose = true;
#else
	bool Engine::s_verbose = false;
#endif

Engine::Engine( void )
{
	if (s_verbose)
		std::cout << "Engine: Default Constructor called" << std::endl;
}

Engine::~Engine( void )
{
	if (s_verbose)
		std::cout << "Engine: Destructor called" << std::endl;
	this->closeSockets();
}

Engine::Engine( const Engine &copy )
{
	if (s_verbose)
		std::cout << "Engine: Copy Constructor called" << std::endl;
	*this = copy;
}

Engine	
&Engine::operator = ( const Engine &rhs )
{
	if (s_verbose)
		std::cout << "Engine: Assignation operator called" << std::endl;
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

s_kevent *
Engine::findByFd( t_fd fd, std::vector<s_kevent> vec )
{
	std::vector<s_kevent>::iterator	iter;

	iter = std::find(vec.begin(), vec.end(), fd);
	if (iter == vec.end())
		return (NULL);
	return(&(*iter));
}

Socket *
Engine::findSock( t_fd fd )
{
	SockIter	iter = std::find(m_Sockets.begin(), m_Sockets.end(), fd);
	if (iter == m_Sockets.end())
		return (NULL);
	return(&(*iter));
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
	Socket	newServ(INADDR_ANY, 8080);
	m_Sockets.push_back(newServ);
}

void
Engine::listenSockets( void )
{
	int		errFlag;
	for (SockIter iter = m_Sockets.begin(); iter != m_Sockets.end(); ++iter)
	{
		errFlag = listen((*iter).getSockFd(), ENGINE_BACKLOG);
		if (errFlag)
		{
			std::cerr << "Listen error: " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		setRead((*iter).getSockFd());
	}
}

void
Engine::closeSockets( void )
{
	for (SockIter iter = m_Sockets.begin(); iter != m_Sockets.end(); ++iter)
		close((*iter).getSockFd());
}


void
Engine::setRead( t_fd fd )
{
	s_kevent	event;

	EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
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
		if (s_verbose)
		{
			std::cout << "\nEvents: " << n_events << std::endl;
			this->debug();
		}
		if (n_events == -1)
			break ;

		for	(int i = 0; i < n_events; ++i)
		{
			Socket	*serv = findSock(m_events[i].ident);
			if (serv == NULL)
			{
				//Connection
				std::cout << "Connection Stuff" << std::endl;
			}
			else
			{
				//SocketFD
				if (!(m_events[i].flags & EV_EOF))
					serv->acceptConnect(*this);
				else
					continue;
			}
				
		}
	}

	//closing of sockets is happening in the Engine destructor
}

