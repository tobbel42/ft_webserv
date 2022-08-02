#include "Connect.hpp"

Connect::Connect():
p_server(nullptr),
m_action(READ),
m_req(80),
p_cookie_base(nullptr)
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
	p_location = rhs.get_location();
	m_action = rhs.getAction();
	m_status_code = rhs.m_status_code;
	m_req = rhs.m_req;
	m_res = rhs.m_res;
	p_cookie_base = rhs.p_cookie_base;
	return (*this);
}

Connect::Connect(fd_type fd, uint32_t ip, uint32_t port, cookies * cookie_base):
	m_fd(fd),
	m_ip(ip),
	m_port(port),
	p_server(nullptr),
	m_action(READ),
	m_req(port),
	p_cookie_base(cookie_base)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

const Server *
Connect::getServer() const { return p_server; }

const Server::Location*
Connect::get_location() const { return p_location; }

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
Connect::setServer(const Server * server ) { p_server = server; }

void
Connect::set_location(const Server::Location* location) { p_location = location; }

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
	#if 0
	if (p_server == nullptr)
	{
		m_res.set_status_code(404);
		m_action = WRITE;
		return;
	}



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

	bool is_valid_cookie = false;

	if (m_req.get_header_entry("Cookie").first) 
	{
		std::string cookie = m_req.get_header_entry("Cookie").second;
		cookie = cookie.substr(cookie.find("=") + 1);
		if (p_cookie_base->find(cookie) != p_cookie_base->end()) {
			is_valid_cookie = true;
			m_req.decrypt_cookie(p_cookie_base->find(cookie)->second);
		}
	}
	
	Executer exec(p_server, p_location, m_req);

	exec.run();

	if (exec.is_cgi()
		&& exec.get_cgi_header().find("Cookie_data") != exec.get_cgi_header().end()
		&& !is_valid_cookie)
	{
		std::string cookies_value = exec.get_cgi_header().find("Cookie_data")->second;
		std::string cookies_key = utils::to_string(p_cookie_base->size());
		p_cookie_base->insert(std::make_pair(cookies_key, cookies_value));
		m_res.set_cookie("helloCookie=" + cookies_key + "; SameSite=Lax");
	}

	m_res.set_server(p_server);
	m_res.set_location(p_location);
	m_res.set_filename(exec.get_filename());
	m_res.set_status_code(exec.get_status_code());
	if (exec.get_status_code() == 200)
		m_res.set_body(exec.get_content());




	#endif
	
	m_action = WRITE;
}

void
Connect::find_location()
{
	p_location = nullptr;

	if (p_server == nullptr)
		return;

	const std::string& filename = m_req.get_target();
	std::string directory = utils::compr_slash(find_dir(filename));

	for (size_t i = 0; i < p_server->locations.size(); ++i)
	{
		const Server::Location* loc = &p_server->locations[i];

		// look for the prefix that matches the requested dir the most
		if (directory.compare(0, loc->prefix.size(), loc->prefix) == 0)
		{
			if (p_location == nullptr ||
				loc->prefix.size() > p_location->prefix.size())
				p_location = loc;
		}
	}
}

std::string
Connect::find_dir(const std::string& name) const
{
	if (name.empty())
		return "/";
	else if (utils::is_dir(p_server->root + name))
	{
		PRINT("is a directory");
		return name + "/";
	}
	else
	{
		size_t pos = name.find_last_of('/');
		if (pos == std::string::npos)
			return "/"; // is this appropriate?
		return name.substr(0, pos + 1);
	}
}