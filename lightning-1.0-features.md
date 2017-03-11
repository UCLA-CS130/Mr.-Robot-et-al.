# Lightning v1.0 Features

Lightning is out of beta! With our official release, we'd like
to highlight our past accomplishments and some new features. 


## New Lightning Features

### Markdown Rendering

All Markdown files (like [this one](https://github.com/UCLA-CS130/Mr.-Robot-et-al./blob/e34b0fcb693a28f48e4270a6f0e38967f05a3b65/test/test.md))
will now automatically be rendered into Markdown-style HTML before being 
served up. Previously, such files would have been served as plain text. 

This design document is served up in this way. It's a perfect example of
how Markdown improves readability and the delightfulness of a Lightning-powered
user experience.

To render the Markdown, we use the excellent [Cpp-Markdown](https://github.com/KyCodeHuynh/Cpp-Markdown) library.


#### Demo

For a running Lightning server, visit any one of these links: 

    domain:port/test.md
    domain:port/lightning-1.0-features.md

To try your own file, create one under the `test/` folder
and visit: 

    domain:port/file-name.md


### Cache-Control Server Directive

* What is the feature?
* Use-cases for it
* Notable implementation details
* Demo walkthrough instructions for each new feature
    - And some way to demonstrate that we're not just hard-coding


### HTTP Basic Authentication

* What is the feature?
* Use-cases for it
* Notable implementation details
* Demo walkthrough instructions for each new feature
    - And some way to demonstrate that we're not just hard-coding




## Core Lightning Features

Lightning is a modern webserver written in C++ that offers:

* Multithreading
* Reverse-proxy functionality
* Static file serving

We also make things easier for developers: 

* An echo handler shows the HTTP request received
* A status page shows a summary of the types of requests received 
* Low-level networking is handled by the [Boost.Asio](http://www.boost.org/doc/libs/1_63_0/doc/html/boost_asio.html) library


## The Lightning Project

The Lightning project itself is meant to be easy to 
contribute to, as we are:

* Continuously built and tested with [Travis CI](https://travis-ci.org/)

* Carefully code reviewed
    - All changes are proposed via pull request and then carefully reviewed
    - `master` branch is protected against force-pushes
    - Tests must pass on CI before a PR can be merged

* Dockerized
    - We've successfully deployed Lightning with Docker on AWS (using the
      latest AMI image), DigitalOcean (using Ubuntu 14.04.5 x64), and locally
      on Ubuntu 14.04 and macOS Sierra (10.12.3).

    - We have separate Docker images for [compilation](https://github.com/UCLA-CS130/Mr.-Robot-et-al./blob/e34b0fcb693a28f48e4270a6f0e38967f05a3b65/Dockerfile) and [deployment](https://github.com/UCLA-CS130/Mr.-Robot-et-al./blob/e34b0fcb693a28f48e4270a6f0e38967f05a3b65/Dockerfile.run), 
      letting the deployment image typically be <10 MB in size.

* Checking our coverage using [`gcov`](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [`lcov`](https://github.com/linux-test-project/lcov)

* Semantically versioned starting with `v1.0` onward



