FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get -y install build-essential
RUN apt-get install -y iputils-ping
RUN apt-get install -y ufw
RUN ufw allow from any to any port 54329 proto tcp


WORKDIR /shengji-backend

COPY src/ ./src/
COPY include-3p/ ./include-3p/


WORKDIR /shengji-backend/build

RUN apt install g++
RUN apt-get update && apt-get install cmake -y
RUN cmake -S ../src/ -B .
RUN make

EXPOSE 54329

CMD ["./shengji-backend"]