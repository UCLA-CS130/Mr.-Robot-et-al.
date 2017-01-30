#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "lightning_server.h"
namespace bai = boost::asio::ip;

class EchoRequestHandler {
  public:
    EchoRequestHandler(boost::shared_ptr<bai::tcp::socket> socket);
    ~EchoRequestHandler();
    char* constructEcho();
    size_t getResponseSize();

  private:
    boost::shared_ptr<bai::tcp::socket> socket_;
    char* response_buffer_;
    size_t response_size_;
};

#endif
