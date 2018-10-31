FROM ubuntu:16.04

RUN apt-get update
RUN apt-get -y install g++
RUN apt-get -y install make
RUN apt-get -y install libtiff5-dev

RUN mkdir -p src/promethee
WORKDIR /src/promethee

COPY . .

RUN make