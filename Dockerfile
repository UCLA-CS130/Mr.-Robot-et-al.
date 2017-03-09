FROM ubuntu:14.04

# Used just for building

# Avoid confirmation dialogs by setting this non-interactive
# option, but only for the duration of this image build
ARG DEBIAN_FRONTEND=noninteractive

# Essential tooling for compilation
RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y make
RUN apt-get install -y g++

# lcov lets us check test coverage
# HTTPie generates HTTP requests for integration tests
RUN apt-get install -y lcov
RUN apt-get install -y httpie

WORKDIR /opt/lightning-webserver
COPY . /opt/lightning-webserver

RUN make clean && make

# Output tarballed lightning binary to stdout when
# a container based on this image is run
CMD ["tar", "-cf", "-", "lightning"]
