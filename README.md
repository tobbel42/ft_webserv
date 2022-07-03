# ft_webserv

## run in docker

first make sure you have docker installed and running on your system

then execute
```
make docker
```
or if you need sudo on your system
```
make sdocker
```
to utilise the docker container we need special socket binding, so we need to use the docker_default.conf configfile, running make docker will do this by default

to connect to the webserv, we need to find the ip address of the container, to find out run
```
ip a
```
or 
```
ifconfig
```

the server runs on port 4242 by default