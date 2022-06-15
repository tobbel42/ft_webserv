#include "../inc/utils.hpp"

void str_tolower(std::string & s) {
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