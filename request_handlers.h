#ifndef REQUEST_HANDLERS_H
#define REQUEST_HANDLERS_H

#include "server_config.h"
#include "server_stats.h"

#include <boost/asio.hpp>


using boost::asio::ip::tcp;

class NginxConfig;
class ServerConfig;
class Request;
class Response;

// This is an abstract base class
class RequestHandler {
  public:
    enum Status {
      OK = 0,
      BAD_REQUEST = 400,
      NOT_FOUND = 404
    };

    // Initializes the handler. Returns true if successful
    // uri_prefix is the value in the config file that this handler will run for.
    // config is the contents of the child block for this handler ONLY.
    static RequestHandler* CreateByName(const char* type);

    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config) = 0;

    virtual Status handleRequest(const Request& request,
                                 Response* response) = 0;
};


// Notes:
// * The trick here is that you can declare an object at file scope, but you
//   can't do anything else, such as set a map key. But you can get around this
//   by creating a class that does work in its constructor.
// * request_handler_builders must be a pointer. Otherwise, it won't necessarily
//   exist when the RequestHandlerRegisterer constructor gets called.

extern std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders;
template<typename T>
class RequestHandlerRegisterer {
  public:
    RequestHandlerRegisterer(const std::string& type) {
      if (request_handler_builders == nullptr) {
        request_handler_builders = new std::map<std::string, RequestHandler* (*)(void)>;
      }
      (*request_handler_builders)[type] = RequestHandlerRegisterer::Create;
    }
    static RequestHandler* Create() {
      return new T;
    }
};
#define REGISTER_REQUEST_HANDLER(ClassName) \
  static RequestHandlerRegisterer<ClassName> ClassName##__registerer(#ClassName)


// Echo the request that was received in the body of the response
class EchoRequestHandler : public RequestHandler {
  public:
    EchoRequestHandler() {}

    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
  private:
    std::string uri_prefix_;
    ServerConfig config_;
};
REGISTER_REQUEST_HANDLER(EchoRequestHandler);

// Serve a up a file statically (without modifications)
class StaticRequestHandler : public RequestHandler {
  public:
    StaticRequestHandler() {}

    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
  private:
    std::string uri_prefix_;
    ServerConfig config_;
};
REGISTER_REQUEST_HANDLER(StaticRequestHandler);

// Return a 404 error page
class NotFoundRequestHandler : public RequestHandler {
  public:
    NotFoundRequestHandler() {}

    virtual bool init(const std::string& uri_prefix,
                        const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
  private:
    std::string uri_prefix_;
    ServerConfig config_;
};
REGISTER_REQUEST_HANDLER(NotFoundRequestHandler);

// Return server statistics, like the routes and request-type counts
class StatusHandler : public RequestHandler {
 public:
    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);

    // Make an external ServerStats instance available to the handler
    void setUpStats(ServerStats* server_stats);

 private:
    static ServerStats* server_stats_;
};

REGISTER_REQUEST_HANDLER(StatusHandler);

#endif  // REQUEST_HANDLER_H

