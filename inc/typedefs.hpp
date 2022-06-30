#pragma once


#include <vector>
#include <string>
#include <limits>
#include <stdint.h>

#ifdef __APPLE__
#define KQUEUE
#endif

#ifdef KQUEUE
#include <sys/event.h>
#else
#include <poll.h>
#endif

#define HTTP_VERSION 1.1
#define DEFAULT_ERROR_PAGES "config/default_error_pages"
#define READSIZE 8192 //we reading at max 8kb of data, might change
#define CNCT_TIMEOUT 2 // in seconds
#define ENGINE_BACKLOG 10


#ifndef nullptr
#define nullptr NULL
#endif
#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif

typedef	int	fd_type;

class Server;

typedef std::vector<std::string>	StringArr;
typedef std::vector<Server>			ServerArr;
typedef std::vector<char>			ByteArr;

#ifdef KQUEUE
struct	s_kevent: public kevent
{
	inline bool operator==(fd_type fd)
	{
		return static_cast<int>(ident) == fd;
	}
/*
inherited from struct kevent:
    uintptr_t ident;	     identifier for this event
    short     filter;	     filter for event
    u_short   flags;	     action flags for kqueue
    u_int     fflags;	     filter flag value
    int64_t   data;		     filter data value 
    void      *udata;	     opaque user data identifier
    uint_64  ext[4];	     extensions
*/
};
#else
struct s_pollfd: public pollfd {
	inline bool operator==(fd_type fd1) { return fd == fd1; };
};
#endif