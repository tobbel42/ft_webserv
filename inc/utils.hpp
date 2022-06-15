#ifndef UTILS_HPP
#define UTILS_HPP

typedef	unsigned long	fd_type;

#define READSIZE 8192 //we reading at max 8kb of data, might change


#include <string>
#include <ctype.h>
void str_tolower(std::string & s);
bool isLWS(const std::string & s, size_t pos);
bool isWS(const std::string & s, size_t pos);
bool isCRLF(const std::string & s, size_t pos);
bool isRLWS(const std::string & s, size_t pos);
bool isRCRLF(const std::string & s, size_t pos);
#endif
