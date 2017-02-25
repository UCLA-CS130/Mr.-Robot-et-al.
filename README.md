# Lightning in a Bottle

![](https://travis-ci.com/UCLA-CS130/Mr.-Robot-et-al..svg?token=yz9yBJgd4Sxya2e1weD1&branch=master)

:zap: Lightning is a modern, robust web server written in C++. 

# Setup

### Supported systems

[Ubuntu 14.04](http://releases.ubuntu.com/14.04/)


### Installation

```
$ git clone --recursive git@github.com:UCLA-CS130/Mr.-Robot-et-al..git
```

### Dependencies

We have the following dependencies:

* Boost: used for socket networking as well as string manipulation.
* Lcov: used to create test coverage webpages.
* Python 3.6: Our integration test that uses methods not supported in version of Python earlier than 3.6
* httpie: a handy tool for checking http connections that we use in our integration test.

To install these dependencies, simply run `./configure.sh`

# Usage

To following is an example of how to build and run LightningServer. It takes in one argument, the [server configuration file](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki/Config-File-Format) (the format of which can be found in the [wiki](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki)).
```
$ make
$ ./lightning [config file]
```

A basic config file that we included is `simple_config`. It contains configurations for all our curent handlers, and specifies port 2020.

To clean everything up:
```
$ make clean
```

To run only integration tests
```
$ make integration_test
```

To run both unit and integration tests (make sure to run `make clean` first if its not the first time running the tests):
```
$ make test
```

# Contributing

### Soure Code Layout

Currently most of our source files are stored in our root directory. The provided `nginx-configparser` source files are in the nginx-configparser submodule which also contains the GTest submodule. 

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
 
