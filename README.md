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

#### Boost
Used for socket networking as well as string manipulation.
```
$ sudo apt-get install libboost-all-dev
```

#### Lcov
Needed to create test coverage webpages.
```
$ sudo apt-get install lcov
```

#### Python 3.6
Out integration test makes use of methods not supported in versions of Python earlier than 3.6
```
$ sudo apt-get install python3.6  
```


# Usage

To following is an example of how to build and run LightningServer. It takes in one argument, the [server configuration file](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki/Config-File-Format) (the format of which can be found in the [wiki](https://github.com/UCLA-CS130/Mr.-Robot-et-al./wiki)).
```
$ make
$ ./lightning [config file]
```

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

Authors: 

* Alex Fong
* Frank Chen
* Ky-Cuong Huynh
 
