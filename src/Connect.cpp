#include "../inc/Connect.hpp"

#ifdef VERBOSE
	bool Connect::s_verbose = true;
#else
	bool Connect::s_verbose = false;
#endif

Connect::Connect( void )
{
	if (s_verbose)
		std::cout << "Connect: Constructor called" << std::endl;
}

Connect::~Connect( void )
{
	if (s_verbose)
		std::cout << "Connect: Destructor called" << std::endl;
}

Connect::Connect( const Connect &copy )
{
	if (s_verbose)
		std::cout << "Connect: Copy Constructor called" << std::endl;
	*this = copy;
}

Connect	&Connect::operator = ( const Connect &rhs )
{
	if (s_verbose)
		std::cout << "Connect: Assignation operator called" << std::endl;
	m_fd = rhs.getFd();
	return (*this);
}

Connect::Connect( t_fd fd ):
	m_fd(fd)
{
	if (s_verbose)
		std::cout << "Connect: Constructor called" << std::endl;
}

t_fd
Connect::getFd( void ) const
{
	return(m_fd);
}