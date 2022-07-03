# ft_webserv

## run in docker

first make sure you have docker installed and running on your system

then execute
	make docker
or if you need sudo on your system
	make sdocker

to utilise the docker container we need special socket binding, so we need to use the docker_default.conf config file

to connect to the webserv, you need to find the ip address of the container
run
	if a
or 
	ifconfig
and you will find the ipadress

the server runs on port 4242 by default