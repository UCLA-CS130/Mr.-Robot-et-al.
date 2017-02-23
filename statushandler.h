#ifndef REQUEST_HANDLERS_H
#define REQUEST_HANDLERS_H

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
    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config) = 0;

    virtual Status handleRequest(const Request& request,
                                 Response* response) = 0;
  protected:
    std::string uri_prefix_;
    ServerConfig config_;
};

// Echo the request that was received in the body of the response
class EchoRequestHandler : public RequestHandler {
  public:
    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
};

// Serve a up a file statically (without modifications)
class StaticRequestHandler : public RequestHandler {
  public:
    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
};

// Return a 404 error page
class NotFoundRequestHandler : public RequestHandler {
  public:
    virtual bool init(const std::string& uri_prefix,
                      const NginxConfig& config);

    virtual Status handleRequest(const Request& request,
                                 Response* response);
};

// StatusHandler
class StatusHandler : public RequestHandler {
 public:
    virtual bool init(const std::string& utl_prefix,
                      const NginxConfig& config);
    virtual Status handleRequest(const Request& request, 
                                 Response* response);
    void setUpStats(const std::unique_ptr<ServerStats> server_stats);
 private:
    static std::unique_ptr<ServerStats> server_stats_;
};

#endif
