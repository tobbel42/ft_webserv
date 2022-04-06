#include "Connect.hpp"

Connect::Connect( void ):
p_server(NULL),
m_action(READ)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Connect::~Connect( void )
{
	#ifdef VERBOSE
		std::cout << "Connect: Destructor called" << std::endl;
	#endif
}

Connect::Connect( const Connect &copy )
{
	#ifdef VERBOSE
		std::cout << "Connect: Copy Constructor called" << std::endl;
	#endif
	*this = copy;
}

Connect	&Connect::operator = ( const Connect &rhs )
{
	#ifdef VERBOSE
		std::cout << "Connect: Assignation operator called" << std::endl;
	#endif
	m_fd = rhs.getFd();
	m_sockFd = rhs.getSockFd();
	p_server = rhs.getServer();
	m_action = rhs.getAction();
	return (*this);
}

Connect::Connect( t_fd fd, t_fd sockFd ):
	m_fd(fd),
	m_sockFd(sockFd),
	p_server(NULL),
	m_action(READ)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Server *
Connect::getServer( void ) const
{
	return p_server;
}

t_fd
Connect::getSockFd( void ) const
{
	return m_sockFd;
}

t_fd
Connect::getFd( void ) const
{
	return m_fd;
}

e_action
Connect::getAction( void ) const
{
	return m_action;
}

void
Connect::setServer( Server * server )
{
	p_server = server;
}

void
Connect::readRequest( s_kevent kevent )
{
	char	*buf = new char[kevent.data + 1];
	buf[kevent.data] = '\0';
	//ToDo Errorhandling
	int i = read(kevent.ident, buf, kevent.data);
	m_request.append(buf);
	delete[] buf;
}

void
Connect::writeResponse( s_kevent kevent )
{
	//ToDo Errorhandling
	write(kevent.ident, m_response.c_str(), m_response.size());
}

void
Connect::composeResponse( void )
{
	std::fstream	file;

	size_t	begin = m_request.find(' ') + 1;
	size_t	end = m_request.find(' ', begin + 1);
	std::string filename =  m_request.substr(begin, end - begin);
	if (filename == "/")
		filename = "/index.html";

	std::cout << (*p_server).getDirectory() << filename << std::endl;

	file.open((*p_server).getDirectory() + filename, std::ios::in);

	if (file.is_open())
	{

		std::string			d;
		std::string			f;
		std::stringstream	s;
		while (1)
		{
			std::getline(file, d);
			f.append(d);
			f.append("\n");
			if (file.eof())
				break;
		}
		m_response.clear();
		m_response.append("HTTP/1.1 200 OK\r\n");
		if (filename.find(".ico") == std::string::npos)
		{
			s << f.size();
			m_response.append("Content-Type: text/html\r\n");
			m_response.append("Content-Lenght:" + s.str() + "\r\n");
		}
		else
		{
			m_response.append("Content-Type: image/x-icon\r\n");
			s << (f.size() - 1);
			m_response.append("Content-Lenght:" + s.str() + "\r\n");
		}
		m_response.append("\r\n");
		m_response.append(f);
		m_response.append("\r\n");
	}
	else
	{
		m_response.clear();
		m_response.append("HTTP/1.1 404 Not Found\r\n");
		m_response.append("Content-Type: text/html\r\n");
		m_response.append("Content-Lenght: 19\r\n");
		m_response.append("\r\n");
		m_response.append("<h1>Not Found</h1>");
		m_response.append("\r\n");
	}

	m_action = WRITE;
}

