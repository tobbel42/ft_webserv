#include "../inc/MoteurDix.hpp"

#ifdef VERBOSE
	bool MoteurDix::m_verbose = true;
#else
	bool MoteurDix::m_verbose = false;
#endif

MoteurDix::MoteurDix( void )
{
	if (m_verbose)
		std::cout << "MoteurDix: Constructor called" << std::endl;
}

MoteurDix::~MoteurDix( void )
{
	if (m_verbose)
		std::cout << "MoteurDix: Destructor called" << std::endl;
}

MoteurDix::MoteurDix( const MoteurDix &copy )
{
	if (m_verbose)
		std::cout << "MoteurDix: Copy constructor called" << std::endl;
	*this = copy;
}

MoteurDix	
&MoteurDix::operator = ( const MoteurDix &rhs )
{
	if (m_verbose)
		std::cout << "MoteurDix: Assignation operator called" << std::endl;
	(void)rhs;
	return (*this);
}


