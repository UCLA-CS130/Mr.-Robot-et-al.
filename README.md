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

Then, to build a Lightning binary (using `Dockerfile`: 

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
* Python 3.6: Our integration test that uses methods not supported in version
  of Python earlier than 3.6
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


## Assignment Notes

### Assignment 7 (Reverse Proxy and Multithreading)

[Details about reverse proxies and our implementation can be found here.](https://github.com/UCLA-CS130/Mr.-Robot-et-al./issues/62)

To complete the reverse proxy part of this assignment, we [forked off of Team05's code](https://github.com/kfrankc/Team05) and finally made a few PRs \[[1](https://github.com/UCLA-CS130/Team05/pull/34), [2](https://github.com/UCLA-CS130/Team05/pull/36), [3](https://github.com/UCLA-CS130/Team05/pull/37)\] which merged the new handler into their project. To test this new functionality, we updated their integration test to spin up two servers, one of which is configured with the proxy handler whose port is set to the other server. We also created unit tests which verified the successful initialization of a `ReverseProxyHandler`.

The configuration format for this new handler:

```
path /reverse_proxy ReverseProxyHandler {
    # Remote host can be a full URI or an IP address
    remote_host localhost
    remote_port 4242
}
```

[Multithreading was completed on LightningServer](https://github.com/UCLA-CS130/Mr.-Robot-et-al./pull/66), which now supports synchronous multithreading by spawning a new thread for each request it recieves. Later we might transition over to asynchronous multithreading at which point we could make use of our config's new `num_threads` parameter. Most of our handlers are thread-safe except for `StatusHandler`, which requires the use of a mutex to prevent race conditions when initializing the count of recieved requests.


Authors:

* Alex Fong
* Frank Chen
* Ky-Cuong Huynh
