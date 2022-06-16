#pragma once


#include <vector>
#include <string>
#include <limits>

#include <cstdint>

#define HTTP_VERSION 1.1

typedef	unsigned long	t_fd;

class Server;

typedef std::vector<std::string>	StringArr;
typedef std::vector<Server>			ServerArr;