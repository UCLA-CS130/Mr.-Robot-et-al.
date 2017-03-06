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

# This gives us the apt-get-repository command,
# needed for Python 3.6, which has new language features
# used in our integration tests
RUN apt-get install -y python-software-properties
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:jonathonf/python-3.6
RUN apt-get update
RUN apt-get install -y python3.6

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
