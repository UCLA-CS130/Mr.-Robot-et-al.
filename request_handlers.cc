#include "lightning_server.h"
#include "request_handlers.h"

#include <iostream>
#include <string>

using boost::asio::ip::tcp;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
void EchoRequestHandler::handle_request(const char* request_buffer,
                                        const size_t& request_buffer_size,
                                        char* &response_buffer,
                                        size_t& response_buffer_size) {

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  const size_t header_size = response_header.size();
  response_buffer_size = header_size + request_buffer_size;

  // TODO: need to check if we need to resize response buffer, if response is huge
  response_buffer = (char *) malloc(sizeof(char) * response_buffer_size);
  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, request_buffer_size);

  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;
}

void request_handlers::staticServeRequestHandler(const char* request_buffer,
                                                 const size_t& request_buffer_size,
                                                 char* response_buffer,
                                                 size_t& response_buffer_size) {

  // TODO: Parse request to get path to file
  // TODO: Check integrity of path
  // TODO: Determine file extension (for later, to fill out Content-Type)
  // TODO: Allocate enough memory for whole file and return pointer to file
  // TODO: Fill out response_buffer with file data
}
