#include "Response.hpp"

Response::Response( void ) {
	#ifdef VERBOSE
		std::cout << "Response: Constructor called" << std::endl;
	#endif
}

Response::Response( const Response & cpy ) {
	#ifdef VERBOSE
		std::cout << "Response: Copy Constructor called" << std::endl;
	#endif
	*this = cpy;
}

Response::~Response() {
	#ifdef VERBOSE
		std::cout << "Response: Destructor called" << std::endl;
	#endif
}

Response & Response::operator=( const Response & rhs ) {
	#ifdef VERBOSE
		std::cout << "Response: Assignation operator called" << std::endl;
	#endif
	m_buffer = rhs.m_buffer;
	m_statusCode = rhs.m_statusCode;
	m_httpVer = rhs.m_httpVer;
	m_header = rhs.m_header;
	m_body = rhs.m_body;
	return (*this);
}