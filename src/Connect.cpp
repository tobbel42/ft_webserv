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
	m_ip = rhs.getIp();
	m_port = rhs.getPort();
	//m_sockFd = rhs.getSockFd();
	p_server = rhs.getServer();
	m_action = rhs.getAction();
	return (*this);
}

Connect::Connect( fd_type fd, unsigned int ip, unsigned int port):
	m_fd(fd),
	m_ip(ip),
	m_port(port),
	//m_sockFd(sockFd),
	p_server(NULL),
	m_action(READ)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Server *
Connect::getServer( void ) const { return p_server; }

// fd_type
// Connect::getSockFd( void ) const
// {
// 	return m_sockFd;
// }

unsigned int
Connect::getIp() const { return m_ip; }

unsigned int
Connect::getPort() const { return m_port; }

fd_type
Connect::getFd( void ) const { return m_fd; }

e_action
Connect::getAction( void ) const { return m_action; }

void
Connect::setServer( Server * server ) { p_server = server; }

bool
Connect::readRequest( s_kevent kevent )
{
	char	buf[READSIZE + 1];
	int		len = ((READSIZE < kevent.data)?READSIZE : kevent.data);
	memset(buf, '\0', READSIZE + 1);
	long	i = read(kevent.ident, buf, len);
	return m_req.appendRead(buf);
	//return true;
}

void
Connect::writeResponse( s_kevent kevent )
{
	//ToDo Errorhandling
	// std::cout << "RESPONSE" << std::endl;
	// std::cout << m_response << std::endl;
	write(kevent.ident, m_response.c_str(), m_response.size());
}

void
Connect::composeResponse( void )
{
	std::ifstream	fs;

	// std::cout << "###\n" << m_req.m_buffer << "###\n" << std::endl;

	// size_t	begin = m_req.m_buffer.find(' ') + 1;
	// size_t	end = m_req.m_buffer.find(' ', begin + 1);
	// std::cout << begin << " " << end << std::endl;
	// std::string filename =  m_req.m_buffer.substr(begin, end - begin);
	// std::cout << "hello" << "$" << m_req.m_target << "$" << std::endl;
	std::string filename = m_req.getTarget();
	std::cout << filename << std::endl;
	if (filename == "/")
		filename += p_server->index;

	//just a fix
	fs.open(p_server->root + filename);


	// the response should also have access to the file that was accessed
	// to determine the MINE type of the body
	Response response;
	response.set_server(p_server);


	if (fs.is_open())
	{
		response.set_body(utils::read_file(fs));
		response.set_status_code(200);
		// std::string			line;
		// std::string			file;
		// std::stringstream	ss;

		// while (1)
		// {
		// 	std::getline(fs, line);
		// 	file.append(line);
		// 	file.append("\r\n");
		// 	if (fs.eof())
		// 		break;
		// }
		// m_response.clear();
		// m_response.append("HTTP/1.1 200 OK\r\n");
		// if (filename.find(".ico") == std::string::npos)
		// {
		// 	ss << file.size();
		// 	m_response.append("Content-Type: text/html\r\n");
		// 	m_response.append("Content-Lenght: " + ss.str() + "\r\n");
		// }
		// else
		// {
		// 	m_response.append("Content-Type: image/x-icon\r\n");
		// 	ss << (file.size() - 1);
		// 	m_response.append("Content-Lenght: " + ss.str() + "\r\n");
		// }
		// m_response.append("\r\n");
		// m_response.append(file);
		// m_response.append("\r\n");
	}
	else
	{
		response.set_status_code(404);
	}
	std::pair<std::string, size_t> resp_pair =  response.generate_response();
	m_response = resp_pair.first;
	m_action = WRITE;
}
