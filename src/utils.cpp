#include "utils.hpp"

void str_tolower(std::string & s) {
	for (size_t i = 0; i < s.size(); ++i)
		s[i] = tolower(s[i]);
}