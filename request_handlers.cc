#include "lightning_server.h"
#include "request_handlers.h"

#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace lightning_server;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
void request_handlers::echoRequestHandler(char* request_buffer,
                                          char* response_buffer,
                                          size_t& response_buffer_size) {

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  std::size_t header_size = response_header.size();
  size_t response_size = header_size + response_buffer_size;

  // TODO: need to check if we need to resize response buffer, if response is huge
  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, response_buffer_size);
  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;

  response_buffer_size = response_size;
}

