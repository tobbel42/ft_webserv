#include "Connect.hpp"

Connect::Connect():
p_server(NULL),
m_action(READ),
m_req(80)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Connect::~Connect()
{
	#ifdef VERBOSE
		std::cout << "Connect: Destructor called" << std::endl;
	#endif
}

Connect::Connect(const Connect &copy)
{
	#ifdef VERBOSE
		std::cout << "Connect: Copy Constructor called" << std::endl;
	#endif
	*this = copy;
}

Connect	&Connect::operator = (const Connect &rhs)
{
	#ifdef VERBOSE
		std::cout << "Connect: Assignation operator called" << std::endl;
	#endif
	m_fd = rhs.getFd();
	m_ip = rhs.getIp();
	m_port = rhs.getPort();
	p_server = rhs.getServer();
	m_action = rhs.getAction();
	m_status_code = rhs.m_status_code;
	m_req = rhs.m_req;
	m_res = rhs.m_res;
	return (*this);
}

Connect::Connect(fd_type fd, uint32_t ip, uint32_t port):
	m_fd(fd),
	m_ip(ip),
	m_port(port),
	p_server(nullptr),
	m_action(READ),
	m_req(port)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

Server *
Connect::getServer() const { return p_server; }

uint32_t
Connect::getIp() const { return m_ip; }

uint32_t
Connect::getPort() const { return m_port; }

fd_type
Connect::getFd() const { return m_fd; }

e_action
Connect::getAction() const { return m_action; }

std::string
Connect::get_hostname() const { return m_req.get_host(); }

void
Connect::setServer( Server * server ) { p_server = server; }

void
Connect::set_status(int status_code) { m_status_code = status_code; }

#ifdef KQUEUE
bool
Connect::readRequest(s_kevent & kevent)
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
#else
bool
Connect::readRequest(s_pollfd & poll)
{
	std::vector<char> buf;

	//Hacky, the read call writes into the internal vector array
	//we resize the vector beforhand, so its 'knows' its correct size
	buf.resize(READSIZE, '\0');
	ssize_t read_len = read(poll.fd, &(*buf.begin()), READSIZE);

	//TODO errhandling
	if (read_len == -1 )
		std::cerr << "ERROR: read" << std::endl;

	buf.resize(read_len);
	return m_req.append_read(buf);
}
#endif


#ifdef KQUEUE
void
Connect::writeResponse(s_kevent & kevent)
{
	m_res.generate();
	m_res.send(kevent);
}
#else
void
Connect::writeResponse(s_pollfd & poll)
{
	m_res.generate();
	m_res.send(poll);
}
#endif

void
Connect::composeResponse()
{
	if (p_server == nullptr)
	{
		m_res.set_status_code(404);
		m_action = WRITE;
		return;
	}


	#if 0

	m_req.substitute_default_target(p_server->index);
	std::string filename = m_req.get_target();


	//TOO: letztes Argument mit directory listing bool ersetzen
	//TODO we now have host, port, taregt and query in the requestthey need to be passed into the MyFile
	//maybe rename Myfile
	MyFile f(p_server->root + filename, "http://" + m_req.get_host() + ":" + utils::to_string(m_req.get_port()) + filename, g_envp, true);
	std::string file = f.read_file();

	// the response should also have access to the file that was accessed
	// to determine the MINE type of the body
	m_res.set_server(p_server);
	m_res.set_filename(p_server->root + filename);

	//here we need a Myfile methode which returns on interanl error
	//->no error
	//->404
	//->500 on exec fail
	// m_res.set_status_code(400);
	m_res.set_status_code(f.get_error_code());
	if (f.get_error_code() == 200)
		m_res.set_body(file + "\r\n");

	#else

	Executer exec(p_server, g_envp, m_req);

	exec.run();

	m_res.set_server(p_server);
	m_res.set_status_code(exec.get_status_code());
	if (exec.get_status_code() == 200)
		m_res.set_body(exec.get_content());





	#endif
	
	m_action = WRITE;
}
