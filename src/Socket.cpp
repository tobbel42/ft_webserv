#include "Socket.hpp"

#include "utils.hpp"

Socket::Socket(uint32_t ip, uint32_t port):
	m_ip(ip),
	m_port(port),
	m_address(),
	m_sockfd(),
	m_add_len(sizeof(m_address))
{
	#ifdef VERBOSE
		PRINT("Socket: Constructor called");
	#endif
}

Socket::Socket(const Socket& other):
	m_ip(other.m_ip),
	m_port(other.m_port),
	m_address(other.m_address),
	m_sockfd(other.m_sockfd),
	m_add_len(sizeof(m_address))
{
	#ifdef VERBOSE
		PRINT("Socket: Copy Constructor called");
	#endif
}

Socket::~Socket()
{
	#ifdef VERBOSE
		PRINT("Socket: Destructor called");
	#endif
	//Socket closing moved to Engine Destructor for convenience
}

Socket&
Socket::operator=(const Socket& other)
{
	#ifdef VERBOSE
		PRINT("Socket: Assignation operator called");
	#endif

	if (this != &other)
	{
		m_ip = other.m_ip;
		m_port = other.m_port;
		m_address = other.m_address;
		m_sockfd = other.m_sockfd;
	}
	return *this;
}

bool
Socket::operator==(fd_type fd) const { return m_sockfd == fd; }

fd_type
Socket::get_sock_fd() const { return m_sockfd; }

uint32_t
Socket::get_ip() const { return m_ip; }

uint32_t
Socket::get_port() const { return m_port; }

bool
Socket::init()
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd < 0)
	{
		EPRINT("ERROR: could not create Socket");
		return false;
	}

	#ifdef VERBOSE
	PRINT("ip:" << utils::ip_to_string(m_ip) << ", port: " << m_port);
	#endif

	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(m_ip);
	m_address.sin_port = htons(m_port);

	int on = 1;
	int tmp = setsockopt(m_sockfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));

	if (tmp == -1) 
	{
		EPRINT("ERROR: setsockopt failed");
		return false;
	}

	if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&m_address), sizeof(m_address)) == -1)
	{
		EPRINT("ERROR: bind failed");
		return false;
	}
	return true;
}

fd_type
Socket::accept_connect() 
{
	fd_type	fd = accept(
				m_sockfd,
				reinterpret_cast<sockaddr*>(&m_address),
				&m_add_len);
	return (fd);
}
