#include "../inc/Connect.hpp"

Connect::Connect( void )
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
	return (*this);
}

Connect::Connect( t_fd fd ):
	m_fd(fd)
{
	#ifdef VERBOSE
		std::cout << "Connect: Constructor called" << std::endl;
	#endif
}

t_fd
Connect::getFd( void ) const
{
	return(m_fd);
}