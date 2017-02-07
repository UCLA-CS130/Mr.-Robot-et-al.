#include "lightning_server.h"
#include "request_handlers.h"

#include <boost/tokenizer.hpp>
#include <cstddef>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
void EchoRequestHandler::handleRequest(const char* request_buffer,
                                       const size_t& request_buffer_size,
                                       char* &response_buffer,
                                       size_t& response_buffer_size) {

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  const size_t header_size = response_header.size();
  response_buffer_size = header_size + request_buffer_size;

  // TODO: need to check if we need to resize response buffer, if response is huge
  response_buffer = new char[response_buffer_size + 1];

  // Copy in headers, the original request, and a terminating nullbyte
  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, request_buffer_size);
  std::memcpy(&response_buffer[header_size + request_buffer_size], "\0", 1);

  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;
}

void StaticRequestHandler::handleRequest(const char* request_buffer,
                                         const size_t& request_buffer_size,
                                         char* &response_buffer,
                                         size_t& response_buffer_size) {

  // TODO: Parse request to get path to file
  // Boost has a great tokenizer
  // See: https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c#55680
  const std::string request(request_buffer);
  boost::char_separator<char> separator(" ");
  boost::tokenizer<boost::char_separator<char>> tokens(request, separator);
  std::cout << "DEBUG: Request tokens in StaticRequestHandler::handleRequest()" << std::endl;
  for (const auto& t : tokens) {
    std::cout << t << "." << std::endl;
  }

  // TODO: Check integrity of path
  // TODO: Determine file extension (for later, to fill out Content-Type)
  // TODO: Allocate enough memory for whole file and return pointer to file
  // TODO: Fill out response_buffer with file data
}

void NotFoundRequestHandler::handleRequest(const char* request_buffer,
                                           const size_t& request_buffer_size,
                                           char* &response_buffer,
                                           size_t& response_buffer_size) {

  const std::string not_found_response =
    "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
    "<html>\n<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n"
    "\n</head>\n</html>";
  response_buffer_size = not_found_response.size();

  response_buffer = new char[response_buffer_size + 1];

  not_found_response.copy(response_buffer, response_buffer_size);
  std::memcpy(&response_buffer[response_buffer_size], "\0", 1);
}
