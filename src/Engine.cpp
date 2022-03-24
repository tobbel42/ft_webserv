#include "../inc/Engine.hpp"

#ifdef VERBOSE
	bool Engine::m_verbose = true;
#else
	bool Engine::m_verbose = false;
#endif

Engine::Engine( void )
{
	if (m_verbose)
		std::cout << "Engine: Constructor called" << std::endl;
}

Engine::~Engine( void )
{
	if (m_verbose)
		std::cout << "Engine: Destructor called" << std::endl;
}

Engine::Engine( const Engine &copy )
{
	if (m_verbose)
		std::cout << "Engine: Copy constructor called" << std::endl;
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

std::ostream &
operator<< ( std::ostream & out, s_kevent const & in )
{
	out << "\nident:\t" << in.ident;
	out << "\nfilter:\t" << in.filter;
	out << "\nflags:\t" << in.flags;
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

	std::cout << "init: " << newServ.getSockFd() << std::endl;
	
	m_servers.push_back(newServ);
	
	std::cout << "vec: " << m_servers.back().getSockFd() << std::endl;
	
}

void
Engine::launch( void )
{
	//init kqueue
	m_kqueue = kqueue();

	//start to listen to server, add the Sockets to the kqueue
	for (ServIter iter = m_servers.begin(); iter != m_servers.end(); ++iter)
	{
		listen((*iter).getSockFd(), ENGINE_MAX_PENDING_CONNECTIONS);
		setRead((*iter).getSockFd());
	}

	int	n_events = 0;
	timespec 	time;
	time.tv_sec = 2;
	time.tv_nsec = 0;
	//the main server loop
	while (true)
	{
		n_events = kevent(m_kqueue,
			&(*m_changes.begin()), m_changes.size(),
			&(*m_events.begin()), m_events.size(),
			&time);
		std::cout << "\nEvents: " << n_events << std::endl;
		this->debug();
	}
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


