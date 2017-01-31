#include "request_handlers.h"

#include <string>
#include <iostream>

using namespace lightning_server;

const int MAX_REQ_SIZE = 8192;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
size_t request_handlers::echoRequestHandler(boost::shared_ptr<bai::tcp::socket> socket) {

  // Read in request
  char request_buffer[MAX_REQ_SIZE];
  boost::system::error_code ec;
  std::size_t bytes_read = socket->read_some(boost::asio::buffer(request_buffer), ec);
  std::cout << "~~~~~~~~~~Request~~~~~~~~~~\n" << request_buffer << std::endl;

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  std::size_t header_size = response_header.size();
  size_t response_size = header_size + bytes_read;

  char response_buffer[response_size];
  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, bytes_read);
  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;

  // Write echo to socket
  boost::asio::write(*socket,
                    boost::asio::buffer(response_buffer, response_size));

  return response_size;
}

