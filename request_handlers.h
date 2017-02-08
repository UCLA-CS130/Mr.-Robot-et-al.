#ifndef REQUEST_HANDLERS_H
#define REQUEST_HANDLERS_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class ServerConfig;

// This is an abstract base class
class RequestHandler {
  public:
    RequestHandler() {};
    virtual void handleRequest(const ServerConfig& server_config,
                               const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size) = 0;
};

// Echo the request that was received in the body of the response
class EchoRequestHandler : public RequestHandler {
  public:
    EchoRequestHandler() {};
    virtual void handleRequest(const ServerConfig& server_config,
                               const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size);
};

// Serve a up a file statically (without modifications)
class StaticRequestHandler : public RequestHandler {
  public:
    StaticRequestHandler() {};
    virtual void handleRequest(const ServerConfig& server_config,
                               const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size);
};

// Return a 404 error page
class NotFoundRequestHandler : public RequestHandler {
  public:
    NotFoundRequestHandler() {};
    virtual void handleRequest(const ServerConfig& server_config,
                               const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size);
};

#endif
