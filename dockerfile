FROM debian:latest

RUN apt update && apt install -y make clang

ADD . .

RUN make re

CMD [ "./webserv", "./config/default_docker.conf" ]