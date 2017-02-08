#include "lightning_server.h"
#include "request_handlers.h"
#include "mime_types.h"
#include "server_config.h"

#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
void EchoRequestHandler::handleRequest(const ServerConfig& server_config,
                                       const char* request_buffer,
                                       const size_t& request_buffer_size,
                                       char* &response_buffer,
                                       size_t& response_buffer_size) {

  // Create response, defaulting to 200 OK status code for now
  const std::string response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  const size_t header_size = response_header.size();
  response_buffer_size = header_size + request_buffer_size;

  // TODO: need to check if we need to resize response buffer, if response is huge
  // response_buffer size has a +1 for a null byte at the end
  response_buffer = new char[response_buffer_size + 1];

  // Copy in headers, the original request, and a terminating nullbyte
  response_header.copy(response_buffer, header_size);
  std::memcpy(&response_buffer[header_size], request_buffer, request_buffer_size);
  std::memcpy(&response_buffer[header_size + request_buffer_size], "\0", 1);

  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;
}

void StaticRequestHandler::handleRequest(const ServerConfig& server_config,
                                         const char* request_buffer,
                                         const size_t& request_buffer_size,
                                         char* &response_buffer,
                                         size_t& response_buffer_size) {

  // Boost has a great tokenizer
  // See: https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c#55680
  const std::string request(request_buffer);
  boost::char_separator<char> separator(" ");
  boost::tokenizer<boost::char_separator<char>> tokens(request, separator);
  std::cout << "DEBUG: Request tokens in StaticRequestHandler::handleRequest()" << std::endl;
  for (const auto& t : tokens) {
    std::cout << t << "." << std::endl;
  }

  int i = 0;
  auto curToken = tokens.begin();
  std::string resourcePath;
  for (;;) {
    if (curToken == tokens.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      break;
    }
    // We assume the 2nd token is the resource path: GET /path/to/resource.txt
    if (i == 1) {
      resourcePath = *curToken;
    }

    i++;
    curToken++;
  }

  std::string request_path = resourcePath;
  // From: Boost Library request_handler.cpp code:
  // http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/
  // example/http/server/request_handler.cpp
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos) {
    std::cout << "DEBUG: Bad Request\n" << std::endl;
    return;
  }
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::string filename = request_path.substr(last_slash_pos + 1);
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;

  std::string resourceRoot;
  std::vector<std::string> query = {"server", "location "
    + request_path.substr(0, last_slash_pos), "root"};

  server_config.propertyLookUp(query, resourceRoot);

  // Check if position of last '.' character != end of string AND
  // position of last '.' comes after position of last '/', then
  // update extention to contain the file extension
  // Example: s = "/bird.png", s[5] = '.', s[0] = '/', extension = "png"
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_path.substr(last_dot_pos + 1);
  }

  std::string content_type = "Content-Type: " + mime_types::extension_to_type(extension);
  const std::string response_header = "HTTP/1.1 200 OK\r\n" + content_type + "\r\n\r\n";
  const size_t header_size = response_header.size();
  boost::filesystem::path root_path(boost::filesystem::current_path());
  std::string full_path = root_path.string() + resourceRoot + "/" + filename;

  std::string reply = response_header;
  std::cout << full_path << std::endl;
  if (!boost::filesystem::exists(full_path)) {
    std::cout << "Dispatching 404 handler\n";
    NotFoundRequestHandler notFoundHandler;
    notFoundHandler.handleRequest(server_config,
                                  request_buffer,
                                  request_buffer_size,
                                  response_buffer,
                                  response_buffer_size);
    return;
  }

  std::ifstream file(full_path.c_str(), std::ios::in | std::ios::binary);
  char buf[512];
  size_t buf_size = file.read(buf, sizeof(buf)).gcount();

  while (buf_size > 0) {
    reply.append(buf, file.gcount());
    buf_size = file.read(buf, sizeof(buf)).gcount();
  }

  response_buffer = new char[reply.size()];
  reply.copy(response_buffer, reply.size());
  response_buffer_size = reply.size();
}

void NotFoundRequestHandler::handleRequest(const ServerConfig& server_config,
                                           const char* request_buffer,
                                           const size_t& request_buffer_size,
                                           char* &response_buffer,
                                           size_t& response_buffer_size) {

  const std::string not_found_response =
    "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
    "<html>\n<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n"
    "\n</head>\n</html>";
  response_buffer_size = not_found_response.size();

  response_buffer = new char[response_buffer_size + 1];

  not_found_response.copy(response_buffer, response_buffer_size);
  std::memcpy(&response_buffer[response_buffer_size], "\0", 1);
}
