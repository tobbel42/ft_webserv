#include "utils.hpp"

#include <arpa/inet.h>

#include <iostream>

namespace utils {


void
str_tolower(std::string & s)
{
	for (size_t i = 0; i < s.size(); ++i)
		s[i] = tolower(s[i]);
}

std::string
get_http_time()
{
	char buffer[1000];
	time_t unix_time = time(NULL);
	tm current_time = *gmtime(&unix_time);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M%S %Z", &current_time);
	return std::string(buffer);
}

uint32_t
string_to_ip(const std::string& ip_string)
{
	return inet_addr(ip_string.c_str());
}


std::string
ip_to_string(uint32_t ip_addr)
{
	uint32_t ip_octet[4];
	std::stringstream ip_string;

	ip_octet[0] = ip_addr & 0x000000ff;
	ip_octet[1] = (ip_addr & 0x0000ff00) >> 8;
	ip_octet[2] = (ip_addr & 0x00ff0000) >> 16;
	ip_octet[3] = (ip_addr & 0xff000000) >> 24;

	ip_string << ip_octet[3] << '.' << ip_octet[2] << '.'
				<< ip_octet[1] << '.' << ip_octet[0];
	return ip_string.str();
}

} // namespace utils
