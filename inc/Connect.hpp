#ifndef CONNECT_HPP
# define CONNECT_HPP

# include <iostream>
# include <utils.hpp>
class Connect
{
	private:
		static bool	s_verbose;
		t_fd		m_fd;
		
		Connect( void );
		
	public:
		Connect( t_fd fd );
		~Connect( void );
		Connect( const Connect &copy );

		Connect	&operator = ( const Connect &rhs );

		t_fd	getFd( void ) const;
};

#endif
