FROM debian:latest

RUN apt update && apt install -y make clang python2 php

ADD . .

RUN make re

CMD [ "./webserv", "./config/docker_default.conf" ]