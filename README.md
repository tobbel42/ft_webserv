# ft_webserv

The goal of ft_webserv is to write a simple webserver from scratch by only using tools of the C++98 standart libary and system calls. \
It follows the guidelines of the [HTML/1.1 RFC](https://www.rfc-editor.org/rfc/rfc9112.html), but is not fully compliant to it.

Please note: The project is only tested and intended to be used on UNIX-based operating systems.

1. [Usage](#usage)
2. [Overview](#overview)
3. [Docker](#docker)
4. [Resources](#resources)


## Usage:

clone the repository:
```bash
git clone https://github.com/tobbel42/ft_webserv.git &&
cd ft_webserv
```

build the project:
```bash
make
```

run the webserver with a default configuration:
```bash
./webserv
```
or use a custom configuration file:
```bash
./webserv [configFileName]
```


## Overview:

The webserver listens to all its specified ip:port addresses at the same
time by using [kqueue] on MACOS and [poll] on other operating systems for its I/O Multiplexing. \
For the default configuration the server listens to ``127.0.0.1:8080`` and ``127.0.0.1:8081`` and is rooted at testServerDir (a guide on how to write custom configurations can be found [here](https://github.com/tobbel42/ft_webserv/blob/main/config/default.conf)). \
The webserver supports: \
    - The GET, PUT, POST and DELETE methods \
    - Displaying static websites \
    - Defining proxies in subdirectories of the server \
    - Executing CGI scripts based on certain extensions (e.g. .php for php-CGI) \
    - Listing the contents of directories


## Docker

first make sure you have docker installed and running on your system

execute
```bash
make docker
```

the server runs on port 8080 by default

## Resources:

[NGINX](https://nginx.org/en/docs/dirindex.html) \
[request and response](https://www.rfc-editor.org/rfc/rfc2616) \
[CGI](https://www.rfc-editor.org/rfc/rfc3875)



[kqueue]: https://man.openbsd.org/kqueue.2
[poll]: https://www.man7.org/linux/man-pages/man2/poll.2.html