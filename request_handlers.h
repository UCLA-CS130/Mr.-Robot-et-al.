#ifndef REQUEST_HANDLERS_H
#define REQUEST_HANDLERS_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// This is an abstract base class
class RequestHandler {
  public:
    RequestHandler() {};
    virtual void handleRequest(const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size) = 0;
};

class EchoRequestHandler : public RequestHandler {
  public:
    EchoRequestHandler() {};
    virtual void handleRequest(const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size);
};

class StaticRequestHandler : public RequestHandler {
  public:
    StaticRequestHandler() {};
    virtual void handleRequest(const char* request_buffer,
                               const size_t& request_buffer_size,
                               char* &response_buffer,
                               size_t& response_buffer_size);
};

#endif
