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

bool isCRLF(const std::string & s, size_t pos) {
	if (pos + 2 > s.size())
		return false;
	if (s[pos] == '\r' && s[pos + 1] == '\n')
		return true;
	return false;
}

bool isWS(const std::string & s, size_t pos) {
	if (s[pos] == '\t' || s[pos] == ' ')
		return true;
	return false;
}

bool isLWS(const std::string & s, size_t pos) {
	if (isCRLF(s, pos))
		pos += 2;
	return isWS(s, pos);
}

bool isRCRLF(const std::string & s, size_t pos) {
	if (pos < 1)
		return false;
	if (s[pos] == '\n' && s[pos - 1] == '\r')
		return true;
	return false;
}

bool isRLWS(const std::string & s, size_t pos) {	
	if (isRCRLF(s, pos))
		pos -= 2;
	return isWS(s, pos);
}

bool isLWS(const std::vector<char> &vec, size_t pos) {
	if (isCRLF(vec, pos))
		pos += 2;
	return isWS(vec, pos);
}

bool isWS(const std::vector<char> &vec, size_t pos) {
	if (pos < vec.size() && (vec[pos] == '\t' || vec[pos] == ' '))
		return true;
	return false;
}

bool isCRLF(const std::vector<char> &vec, size_t pos) {
	if (pos + 1 < vec.size() && vec[pos] == '\r' && vec[pos + 1] == '\n')
		return true;
	return false;
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

uint32_t
hex_str_to_i(const std::string & str) {
	uint32_t n;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> n;
	return n;
}

//ft_split, but make it cool
StringArr
str_split(const std::string &s, const std::string & del) {
	StringArr vec;
	size_t pos1 = 0, pos2;

	if (del == "")
	{
		vec.push_back(s);
		return vec;
	}

	while (pos1 <= s.size())
	{
		pos2 = pos1;
		pos1 = s.find(del, pos2);
		if (pos1 != std::string::npos) {
			vec.push_back(s.substr(pos2, pos1 - pos2));
			pos1 += del.size();
		}
		else {
			vec.push_back(s.substr(pos2));
			break;
		}
	}
	return vec;
}


std::string
read_file(std::istream& file, const char* nl)
{
	std::string contents;
	std::string line;

	do
	{
		std::getline(file, line);
		contents.append(line);
		contents.append(nl);
	} while (file);

	return contents;
}

std::string
arr_to_csv(const StringArr& arr, const char* sep)
{
	std::string result;

	StringArr::const_iterator it = arr.begin();
	while (it != arr.end())
	{
		result += *it;
		++it;
		if (it != arr.end())
			result += sep;
	}
	return result;
}

std::string
get_file_ext(const std::string& filename)
{
	size_t pos = filename.find_last_of('.');
	if (pos == std::string::npos)
		return std::string();
	return filename.substr(pos + 1);
}

} // namespace utils
