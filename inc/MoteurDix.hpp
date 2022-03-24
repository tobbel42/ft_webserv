#ifndef MOTEURDIX_HPP
# define MOTEURDIX_HPP

# include <iostream>

class MoteurDix
{
	private:
		static bool	m_verbose;

	public:
		MoteurDix( void );
		~MoteurDix( void );
		MoteurDix( const MoteurDix &copy );

		MoteurDix	&operator = ( const MoteurDix &rhs );
};

#endif
