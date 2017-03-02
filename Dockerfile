FROM ubuntu:14.04

# Avoid confirmation dialogs by setting this non-interactive
# option, but only for the duration of this image build
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y make
RUN apt-get install -y g++

# This gives us the apt-get-repository command
RUN apt-get install -y python-software-properties
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:jonathonf/python-3.6
RUN apt-get update
RUN apt-get install -y python3.6

RUN apt-get install -y lcov
RUN apt-get install -y httpie

WORKDIR /opt/lightning-webserver
# Copies from current dif into image's working dir
COPY . /opt/lightning-webserver

RUN make clean && make

# Port in container, host (check order)
EXPOSE 2020:2020

CMD ["./lightning", "simple_config"]
