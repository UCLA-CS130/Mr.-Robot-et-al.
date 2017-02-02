#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace lightning_server {
  // All request handlers return a pointer to the response buffer
  // to be written back to the client.
  namespace request_handlers {
    // Echo the request that was received
    void echoRequestHandler(char* request_buffer,
                            char* response_buffer,
                            size_t& response_buffer_size);
  }
}

#endif
