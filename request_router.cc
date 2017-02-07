#include "request_router.h"
#include "request_handlers.h"

#include <boost/tokenizer.hpp>

// TODO: Split out HTTP parsing into its own class
bool RequestRouter::routeRequest(const ServerConfig& server_config,
                                 const char* request_buffer,
                                 const size_t& request_buffer_size,
                                 char* &response_buffer,
                                 size_t& response_buffer_size) {

  // Tokenize the request so it's easy to extract the resource path
  const std::string request(request_buffer);
  boost::char_separator<char> separator(" ");
  boost::tokenizer<boost::char_separator<char>> tokens(request, separator);

  // We assume that files are stored at the top level and won't recursively
  // search for the requested file. Also, that routing in the config is
  // specified within the server block.
  std::vector<std::string> query = {"server"};
  // We assume the 2nd token is the resource path: GET /path/to/resource.txt

  int i = 0;
  auto curToken = tokens.begin();
  std::string resourcePath;
  for (;;) {
    if (curToken == tokens.end()) {
      // TODO: tokens is too short
      std::cout << "DEBUG: tokens in RequestRouter is too short" << std::endl;
      return false;
    }

    if (i == 1) {
      resourcePath = *curToken;
    }

    i++;
  }

  size_t indexLastSlash = resourcePath.find_last_of("/");

  // Checking for a file extension to make sure the path given includes a
  // resource (/echo vs /static/file.jpg vs /static/otherroute)
  if (resourcePath.substr(indexLastSlash).find(".")) {
    std::string resourceRoot = resourcePath.substr(0, indexLastSlash);
    query.push_back("location " + resourceRoot);
  }
  else {
    query.push_back("location " + resourcePath);
  }

  std::string action;
  server_config.propertyLookUp(query, action);

  if (action == "echo") {
    EchoRequestHandler echoHandler = EchoRequestHandler();
    echoHandler.handleRequest(request_buffer,
                              request_buffer_size,
                              response_buffer,
                              response_buffer_size);
    return true;
  }
  else if (action == "static_serve") {
    StaticRequestHandler staticHandler = StaticRequestHandler();
    staticHandler.handleRequest(request_buffer,
                                request_buffer_size,
                                response_buffer,
                                response_buffer_size);
    return true;
  }
  else {
    NotFoundRequestHandler notFoundHandler = NotFoundRequestHandler();
    notFoundHandler.handleRequest(request_buffer,
                                  request_buffer_size,
                                  response_buffer,
                                  response_buffer_size);
    return false;
  }
}

