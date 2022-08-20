#include "Connect.hpp"

#include <unistd.h>
#include <string>

#include "utils.hpp"
#include "Executer.hpp"

Connect::Connect(const Connect& other)
{
	*this = other;
	#ifdef VERBOSE
		PRINT("Connect: Copy Constructor called");
	#endif
}

Connect::~Connect()
{
	#ifdef VERBOSE
		PRINT("Connect: Destructor called");
	#endif
}

Connect&
Connect::operator=(const Connect& rhs)
{
	#ifdef VERBOSE
		PRINT("Connect: Assignation operator called");
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
	return *this;
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
		PRINT("Connect: Constructor called");
	#endif
}

const Server*
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
Connect::set_server(const Server * server ) { p_server = server; }

void
Connect::set_location(const Server::Location* location) { p_location = location; }

void
Connect::set_status(int status_code) { m_status_code = status_code; }

#ifdef KQUEUE

// 1 on success
// 0 on not done yet
//-1 on error
int32_t
Connect::readRequest(s_kevent & kevent)
{
	ByteArr buf;
	int	len = ((READSIZE < kevent.data) ? READSIZE : kevent.data);

	//Hacky, the read call writes into the internal vector array
	//we resize the vector beforhand, so its 'knows' its correct size
	buf.resize(len, '\0');
	ssize_t read_len = read(kevent.ident, &(*buf.begin()), len);

	if (len != read_len)
	{
		EPRINT("ERROR: read");
		return RW_ERROR;
	}
	if (m_req.append_read(buf))
		return RW_DONE;
	else
		return RW_CONTINUE;
}

#else

int32_t
Connect::readRequest(s_pollfd & poll)
{
	ByteArr buf;

	//Hacky, the read call writes into the internal vector array
	//we resize the vector beforhand, so its 'knows' its correct size
	buf.resize(READSIZE, '\0');
	ssize_t read_len = read(poll.fd, &(*buf.begin()), READSIZE);

	if (read_len == -1 || read_len == 0)
	{
		EPRINT("ERROR: read");
		return RW_ERROR;
	}

	buf.resize(read_len);
	if (m_req.append_read(buf))
		return RW_DONE;
	else
		return RW_CONTINUE;
}

#endif // KQUEUE


#ifdef KQUEUE

int32_t
Connect::writeResponse(s_kevent & kevent)
{
	m_res.generate();
	return m_res.send(kevent);
}

#else

int32_t
Connect::writeResponse(s_pollfd & poll)
{
	m_res.generate();
	return m_res.send(poll);
}

#endif // KQUEUE

bool
Connect::check_redirect()
{
	std::string parsed_target, redirection;

	std::map<std::string, std::string>::const_iterator red_iter;

	if (p_location)
	{
		parsed_target = m_req.get_target();

		//trunctate the location prefix
		parsed_target = parsed_target.erase(0, p_location->prefix.size());

		//check if an redirection exists
		red_iter = p_location->redirections.find(parsed_target);

		if (red_iter != p_location->redirections.end())
			redirection = red_iter->second;
		else
			return false;

		//append the prefix onto the redirection
		m_res.set_content_location(
			utils::compr_slash(p_location->prefix + "/" + redirection));
	}
	else
	{
		parsed_target = m_req.get_target();

		//truncate the leading dash
		if (parsed_target[0] == '/')
			parsed_target = parsed_target.erase(0, 1);

		//check if an redirection exists
		red_iter = p_server->redirections.find(parsed_target);
		if (red_iter != p_server->redirections.end())
			redirection = red_iter->second;
		else
			return false;

		//append the prefix onto the redirection
		m_res.set_content_location(
			utils::compr_slash("/" + redirection));
	}


	m_res.set_server(p_server);
	m_res.set_location(p_location);

	//301 are saved permanently, using 307 so we can tinker with redirections
	m_res.set_status_code(307);
	m_action = WRITE;
	return true;
}

void
Connect::composeResponse()
{
	PRINT("COMPOSING_REQUEST");
	if (check_redirect())
		return;

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

	PRINT("EXEC_START");
	exec.run();
	PRINT("EXEC_DONE");

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
	if (exec.get_status_code() >= 200 && exec.get_status_code() <= 400)
		m_res.set_body(exec.get_content());

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
		return name + "/";
	else
	{
		size_t pos = name.find_last_of('/');
		if (pos == std::string::npos)
			return "/"; // is this appropriate?
		return name.substr(0, pos + 1);
	}
}
