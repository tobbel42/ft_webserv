FROM debian:latest

RUN apt update
RUN apt install make && yes | apt install clang
