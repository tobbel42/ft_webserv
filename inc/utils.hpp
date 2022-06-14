#pragma once

#include <string>
#include <sstream>

typedef	unsigned long	t_fd;



namespace utils {



/*
@brief Converts the supplied value to a std::string. 
The value must have an overload for the std::ostream::operator <<
*/
template<typename T>
inline std::string
to_string(const T& value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}




} // namespace utils