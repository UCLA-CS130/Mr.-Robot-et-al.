#include "echo_request_handler.h"
#include <string>
#include <iostream>

const int MAX_REQ_SIZE = 8192;

EchoRequestHandler::EchoRequestHandler(boost::shared_ptr<bai::tcp::socket> socket)
  : socket_(socket)
{
}

EchoRequestHandler::~EchoRequestHandler() {
}

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
char* EchoRequestHandler::constructEcho() {
  // Read in request
  char request_buffer[MAX_REQ_SIZE];
  boost::system::error_code ec;
  std::size_t bytes_read = socket_->read_some(boost::asio::buffer(request_buffer), ec);
  std::cout << "~~~~~~~~~~Request~~~~~~~~~~\n" << request_buffer << std::endl;

  // Create response, defaulting to 200 OK status code
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  std::size_t header_size = response_header.size();
  response_size_ = header_size + bytes_read;

  response_buffer_ = new char[response_size_]();
  response_header.copy(response_buffer_, header_size);
  std::memcpy(&response_buffer_[header_size], request_buffer, bytes_read);
  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer_ << std::endl;

  return response_buffer_;
}

// Getter for stored echo response size.
size_t EchoRequestHandler::getResponseSize() {
  return response_size_;
}
