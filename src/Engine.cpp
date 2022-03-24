#include "../inc/Engine.hpp"

#ifdef VERBOSE
	bool Engine::m_verbose = true;
#else
	bool Engine::m_verbose = false;
#endif

Engine::Engine( void )
{
	if (m_verbose)
		std::cout << "Engine: Default Constructor called" << std::endl;
}

Engine::~Engine( void )
{
	if (m_verbose)
		std::cout << "Engine: Destructor called" << std::endl;
	this->closeServers();
}

Engine::Engine( const Engine &copy )
{
	if (m_verbose)
		std::cout << "Engine: Copy Constructor called" << std::endl;
	*this = copy;
}

Engine	
&Engine::operator = ( const Engine &rhs )
{
	if (m_verbose)
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

Server *
Engine::findServ( t_fd fd )
{
	ServIter	iter = std::find(m_servers.begin(), m_servers.end(), fd);
	if (iter == m_servers.end())
		return (NULL);
	return(&(*iter));
}

std::ostream &
operator<< ( std::ostream & out, s_kevent const & in )
{
	out << "\nident:\t" << in.ident;
	out << "\nfilter:\t" << in.filter;
	out << "\nflags:\t" << std::bitset<2>(in.flags);
	out << "\nfflags:\t" << in.fflags;
	out << "\ndata:\t" << in.data;
	out << "\nudata:\t" << in.udata;
	return( out );
}

/*
//in here all the socket binding magic should happen, for now just a dummy
*/
void
Engine::initServers( void )
{
	Server	newServ(INADDR_ANY, 8080);
	m_servers.push_back(newServ);
}

void
Engine::listenServers( void )
{
	int		errFlag;
	for (ServIter iter = m_servers.begin(); iter != m_servers.end(); ++iter)
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
Engine::closeServers( void )
{
	for (ServIter iter = m_servers.begin(); iter != m_servers.end(); ++iter)
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
//Main loop of the server.
*/
void
Engine::launch( void )
{
	//init kqueue
	m_kqueue = kqueue();

	//start to listen to server, add the Sockets to the kqueue
	listenServers();
	
	int	n_events = 0;

	//the main server loop
	while (true)
	{
		n_events = kevent(m_kqueue,
			&(*m_changes.begin()), m_changes.size(),
			&(*m_events.begin()), m_events.size(),
			NULL);
		if (m_verbose)
		{
			std::cout << "\nEvents: " << n_events << std::endl;
			this->debug();
		}
		if (n_events == -1)
			break ;

		for	(int i = 0; i < n_events; ++i)
		{
			Server	*serv = findServ(m_events[i].ident);
			if (serv == NULL)
			{
				//Connection
				std::cout << "Connection Stuff" << std::endl;
			}
			else
			{
				//ServerFD
				if (!(m_events[i].flags & EV_EOF))
					serv->acceptConnect(*this);
				else
					continue;
			}
				
		}
	}

	//closing of sockets is happening in the Engine destructor
}

