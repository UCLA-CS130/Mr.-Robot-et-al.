# Lightning v1.0 Features

Lightning is out of beta! With our official release, we'd like
to highlight our past accomplishments and some new features.

Core developers: 

* Alex Fong
* Frank Chen
* Ky-Cuong Huynh


## New Lightning Features

### Markdown Rendering

All Markdown files (like [this one](https://github.com/UCLA-CS130/Mr.-Robot-et-al./blob/e34b0fcb693a28f48e4270a6f0e38967f05a3b65/test/test.md))
will now automatically be rendered into Markdown-style HTML before being 
served up. Previously, such files would have been served as plain text, 
which is off-putting to many users and difficult to read.

This design document is served up in this way. It's a perfect example of
how Markdown improves readability and the delightfulness of a Lightning-powered
user experience.

To render the Markdown, we use the excellent [Cpp-Markdown](https://github.com/KyCodeHuynh/Cpp-Markdown) library.


#### Demo

For a running Lightning server, visit any one of these paths: 

    http://ec2-54-242-5-206.compute-1.amazonaws.com/static/test.md
    http://ec2-54-242-5-206.compute-1.amazonaws.com/static/lightning-1.0-features.md

To try your own file, create one under the `test/` folder
and visit: 

    http://ec2-54-242-5-206.compute-1.amazonaws.com/file-name.md


### Cache-Control Server Directive

Lightning can now direct clients to expire locally cached content 
after a specified time. This lets a Lightning administrator globally
configure how long static files will be cached by a browser. That way, 
users will usually have the latest version of files.

To configure this functionality, a new parameter `cache_max_age` has been added to the config
block for any StaticFileHandler. For example:

    path / StaticRequestHandler {
      root /public;
      # 86400 seconds = 24 hours = 1 day
      cache_max_age: 86400; # seconds, no commas or periods
    }

Then, in response to requests hitting this path, Lightning will set
the [`Cache-Control`](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control) header: 

    Cache-Control: public, max-age=86400

Browsers that support this header (all modern ones do) will then cache the content received for up to 86,400 seconds (1 day).

*Note*: `cache_max_age` is an *optional* parameter. If it is not set, no 
`Cache-Control` header will be set. 

In terms of implementation, we modified the `StaticRequestHandler`
to query its `ServerConfig` for this new parameter. In general, server-only
parameters can be added this way.


#### Demo

Modify the `simple_config` configuration file to have a short cache
expiration time, e.g., 5 seconds: 

    path / StaticRequestHandler {
      root /test;
      cache_max_age: 5; # seconds, no commas or periods
    }

Load any static file. For example, with our default Lightning instance, 
we can load an Angry Birds image with: 

    http://ec2-54-242-5-206.compute-1.amazonaws.com/static/angrybird.png

A browser's developer tools should show an `200 OK` response. Then: 

* Refresh the page, and a `304 Redirect` (to cache) or `200 OK (cached)` should
  occur, as the browser is locally caching the content.
* Wait 5 seconds, and refresh the page again. A `200 OK` response should once
  again be seen, as the content has expired.

The above can be repeated with any static file.


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



