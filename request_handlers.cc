#include "lightning_server.h"
#include "request_handlers.h"

#include <iostream>
#include <string>

using boost::asio::ip::tcp;
using namespace lightning_server;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
char* request_handlers::echoRequestHandler(boost::shared_ptr<tcp::socket> socket,
                                           char* response_buffer,
                                           size_t& response_buffer_size) {
  // Read in request
  char request_buffer[MAX_REQ_SIZE];
  boost::system::error_code ec;
  std::size_t bytes_read = socket->read_some(boost::asio::buffer(request_buffer), ec);
  switch (ec.value()) {
    case boost::system::errc::success:
      std::cout << "~~~~~~~~~~Request~~~~~~~~~~\n" << request_buffer << std::endl;
      break;
    default:
      std::cout << "Error reading from socket, code: " << ec << std::endl;
      return nullptr;
  }

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  std::size_t header_size = response_header.size();
  size_t response_size = header_size + bytes_read;

  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, bytes_read);
  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;

  response_buffer_size = response_size;
  return response_buffer;
}

