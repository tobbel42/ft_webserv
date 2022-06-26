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
	p_server = rhs.getServer();
	m_action = rhs.getAction();
	return (*this);
}

Connect::Connect( fd_type fd, unsigned int ip, unsigned int port):
	m_fd(fd),
	m_ip(ip),
	m_port(port),
	p_server(NULL),
	m_action(READ)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Server *
Connect::getServer( void ) const { return p_server; }

unsigned int
Connect::getIp() const { return m_ip; }

unsigned int
Connect::getPort() const { return m_port; }

fd_type
Connect::getFd( void ) const { return m_fd; }

e_action
Connect::getAction( void ) const { return m_action; }

std::string
Connect::get_hostname() const { return m_req.get_header_entry("Host"); }

void
Connect::setServer( Server * server ) { p_server = server; }

void
Connect::set_status(int status_code) { m_status_code = status_code; }

bool
Connect::readRequest( s_kevent kevent )
{
	std::vector<char> buf;
	int	len = ((READSIZE < kevent.data)?READSIZE : kevent.data);

	//Hacky, the read call writes into the internal vector array
	//we resize the vector beforhand, so its 'knows' its correct size
	buf.resize(len, '\0');
	ssize_t read_len = read(kevent.ident, &(*buf.begin()), len);

	//TODO errhandling
	if (len != read_len)
		std::cerr << "ERROR: read" << std::endl;

	return m_req.append_read(buf);
}

void
Connect::writeResponse( s_kevent kevent )
{
	m_res.generate();
	m_res.send(kevent);
	// write(kevent.ident, resp_pair.first.c_str(), resp_pair.first.size());
}

void
Connect::composeResponse( void )
{
	if (p_server == nullptr)
	{
		m_res.set_status_code(404);
		m_action = WRITE;
		return;
	}
	std::string filename = m_req.get_target();
	if (filename == "/") // und directory listing ist aus
		filename += p_server->index;

	//TOO: letztes Argument mit directory listing bool ersetzen
	MyFile f(filename, p_server->root, "http://" + m_req.get_header_entry("host") + filename, g_envp, true);
	std::string file = f.read_file();

	// the response should also have access to the file that was accessed
	// to determine the MINE type of the body
	m_res.set_server(p_server);

	//here we need a Myfile methode which returns on interanl error
	//->no error
	//->404
	//->500 on exec fail
	if (1)
	{
		m_res.set_status_code(200);
		m_res.set_body(file + "\r\n");
	}
	else
		m_res.set_status_code(404);
	m_action = WRITE;
}
