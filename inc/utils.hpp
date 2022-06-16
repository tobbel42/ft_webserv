#pragma once

#include <ctime>
#include <cstdint>
#include <cctype>

#include <string>
#include <sstream>




namespace utils {

/*
@brief Converts the supplied string to uppercase
*/
void
str_tolower(std::string & s);

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


/*
@brief Converts the supplied string to the templated value.
The value must have an overload for the std::ostream::operator >>
*/
template<typename T>
inline T
from_string(const std::string& s)
{
	T value;
	std::stringstream ss(s);
	ss >> value;
	return value;
}

/*
@return The current time in the format:
Weekday-name, Day Month-name Year Hour:Minute:Second Timezone
*/
std::string
get_http_time();

/*
@brief Converts the supplied string to an ip address
*/
uint32_t
string_to_ip(const std::string& ip_string);

/*
@brief Converts the supplied ip address to std::string
*/
std::string
ip_to_string(uint32_t ip_addr);



} // namespace utils
