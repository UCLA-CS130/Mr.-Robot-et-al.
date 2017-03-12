# Lightning in a Bottle

![](https://travis-ci.com/UCLA-CS130/Mr.-Robot-et-al..svg?token=yz9yBJgd4Sxya2e1weD1&branch=master)

:zap: Lightning is a modern, robust web server written in C++.


## Supported systems

[Ubuntu 14.04 (Trusty)](http://releases.ubuntu.com/14.04/)


## Installation

Ensure that [git](https://www.digitalocean.com/community/tutorials/how-to-install-git-on-ubuntu-14-04) and [Docker](https://store.docker.com/editions/community/docker-ce-server-ubuntu?tab=description) are both installed:

Then grab a copy of this repository:

```
$ git clone --recursive https://github.com/UCLA-CS130/Mr.-Robot-et-al..git
```

Then, to build a Lightning binary using `Dockerfile`: (prepend `sudo` to the following two commands if you're running this on Linux)

```
make build
```

And to start the Lightning server (using `Dockerfile.run`):

```
make deploy
```


### Manually Install Dependencies

If you wish to avoid the use of Docker and would prefer to manually install
everything, we have the following dependencies:

* Boost: used for socket networking as well as string manipulation.
* lcov: used to create test coverage webpages.
* HTTPie: a handy tool for checking HTTP connections that we use in our
  integration test.

To install these dependencies, simply run:

```
./configure.sh
```

If the execute permissions on the `configure.sh` script prevent it 
from running, change them to read + execute using: 

```
chmod 0755 configure.sh
```


## Usage

To following is an example of how to build and run LightningServer. It takes in one argument, the [server configuration file](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki/Config-File-Format) (the format of which can be found in the [wiki](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki)).

```
$ make
$ ./lightning [config file]
```

A basic config file that we included is `simple_config`. It contains configurations for all our curent handlers, and specifies port 8080.

To clean everything up:

```
$ make clean
```

To run only integration tests:

```
$ make integration_test
```

To run both unit and integration tests (make sure to run `make clean` first if its not the first time running the tests):

```
$ make test
```

## Contributing

### Soure Code Layout

The current layout of our source files is laid out below. The assets to be served by the static handler are stored in `assets/`. The server can be configured using the configs stored in our root directory. `deploy` is used during deployment into images by Docker. Other libraries we use can be found in their respective submodules.

```
src/
test/
assets/
deploy/
simple_config
example_config

# Other libraries
nginx-configparser/
  gtest/
cpp-markdown/
```

The following illustrates the hierarchy of `.cc` files as they are called when running the server.

```
lightning_main
  lightning_server      (listens on socket specified by config file)
    server_config       (wrapper for NginxConfig to make it easier to get properties)
    request_router      (initializes long-lived handlers)
      request
      response
      request_handlers
```


### Adding a Handler

In order to add a new handler, you will need to first create a class definition and implementation for it in `request_handlers.h` and `request_handlers.cc` files. We have a abstract base class called `RequestHandler` that contains the enums and initializes the handler. Your handler will be derived from this class. `EchoRequestHandler` is a good example of how this can be done. Each new handler will call the `init` member function and implement the `handleRequest` member function.

Once you are done with making your handler, you can use it by running the server with a config file that gives your handler a path. See `simple_config` for examples of previous handler paths.


Authors:

* Alex Fong
* Frank Chen
* Ky-Cuong Huynh
