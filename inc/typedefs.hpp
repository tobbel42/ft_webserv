#pragma once


#include <vector>
#include <string>
#include <limits>

#include <cstdint>

#define HTTP_VERSION 1.1
#define READSIZE 8192 //we reading at max 8kb of data, might change

typedef	unsigned long	t_fd;
typedef t_fd			fd_type; // what is the fd_type??

class Server;

typedef std::vector<std::string>	StringArr;
typedef std::vector<Server>			ServerArr;