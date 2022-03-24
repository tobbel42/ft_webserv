#include "../inc/Connect.hpp"

#ifdef VERBOSE
	bool Connect::m_verbose = true;
#else
	bool Connect::m_verbose = false;
#endif

Connect::Connect( void )
{
	if (m_verbose)
		std::cout << "Connect: Constructor called" << std::endl;
}

Connect::~Connect( void )
{
	if (m_verbose)
		std::cout << "Connect: Destructor called" << std::endl;
}

Connect::Connect( const Connect &copy )
{
	if (m_verbose)
		std::cout << "Connect: Copy Constructor called" << std::endl;
	*this = copy;
}

Connect	&Connect::operator = ( const Connect &rhs )
{
	if (m_verbose)
		std::cout << "Connect: Assignation operator called" << std::endl;
	m_fd = rhs.getFd();
	return (*this);
}

Connect::Connect( t_fd fd ):
	m_fd(fd)
{
	if (m_verbose)
		std::cout << "Connect: Constructor called" << std::endl;
}

t_fd
Connect::getFd( void ) const
{
	return(m_fd);
}