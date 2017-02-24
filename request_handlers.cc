#include "lightning_server.h"
#include "request_handlers.h"
#include "mime_types.h"
#include "response.h"
#include "request.h"
#include "server_config.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders = nullptr;

RequestHandler* RequestHandler::CreateByName(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    std::cout << "type_and_builder failed to find the type\n";
    return nullptr;
  }
  return (*type_and_builder->second)();
}


// Request Handlers instantiated by lightning_server::run()
// looks at child block of each route
// constructs ServerConfig for each to look at root /exampleuri/something

bool EchoRequestHandler::init(const std::string& uri_prefix,
                              const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  config_ = ServerConfig(config);
  // TODO: do logging/check return value of build
  config_.build();
  return true;
}

// Read in the request from the socket used to construct the handler, then
// fill the response buffer with the necessary headers followed by the
// original request.
RequestHandler::Status EchoRequestHandler::handleRequest(const Request& request,
                                                         Response* response) {
  std::cout << "EchoRequestHandler currently responding.\n";

  // Create response, defaulting to 200 OK status code for now
  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", "text/plain");
  response->SetBody(request.raw_request());
  std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response->ToString() << std::endl;

  return RequestHandler::OK;
}

bool StaticRequestHandler::init(const std::string& uri_prefix,
                                const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  config_ = ServerConfig(config);
  // TODO: do logging/check return value of build
  config_.build();
  return true;
}

RequestHandler::Status StaticRequestHandler::handleRequest(const Request& request,
                                                           Response* response) {
  std::cout << "StaticRequestHandler currently responding.\n";

  std::string request_path = request.uri();

  // Validating request
  // From: Boost Library request_handler.cpp code:
  // http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/
  // example/http/server/request_handler.cpp
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos) {
    std::cout << "DEBUG: Bad Request\n" << std::endl;
    return RequestHandler::BAD_REQUEST;
  }

  // Extracting extension for getting the correct mime type
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::string filename = request_path.substr(last_slash_pos + 1);
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;

  // Check if position of last '.' character != end of string AND
  // position of last '.' comes after position of last '/', then
  // update extention to contain the file extension
  // Example: s = "/bird.png", s[5] = '.', s[0] = '/', extension = "png"
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Since we're passed in the child block of the route block, we can directly
  // lookup the 'root' path
  std::string resourceRoot;
  std::vector<std::string> query = {"root"};
  config_.printPropertiesMap();
  config_.propertyLookUp(query, resourceRoot);

  // Construct the actual path to the requested file
  boost::filesystem::path root_path(boost::filesystem::current_path());
  boost::replace_all(request_path, uri_prefix_, resourceRoot);
  std::string full_path = root_path.string() + request_path;

  // Check to make sure that file exists, and dispatch 404 handler if it doesn't
  if (!boost::filesystem::exists(full_path)) {
    std::cout << "Dispatching 404 handler. File not found/doesn't exist at: "
              << full_path << std::endl;
    return RequestHandler::NOT_FOUND;
  }

  // Read file into buffer
  std::ifstream file(full_path.c_str(), std::ios::in | std::ios::binary);
  std::string reply = "";
  char file_buf[512];
  while (file.read(file_buf, sizeof(file_buf)).gcount() > 0) {
    reply.append(file_buf, file.gcount());
  }

  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", mime_types::extension_to_type(extension));
  response->SetBody(reply);

  return RequestHandler::OK;
}

bool NotFoundRequestHandler::init(const std::string& uri_prefix,
                                  const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  config_ = ServerConfig(config);
  // TODO: do logging/check return value of build
  config_.build();
  return true;
}

RequestHandler::Status NotFoundRequestHandler::handleRequest(const Request& request,
                                                             Response* response) {
  std::cout << "NotFoundRequestHandler currently responding.\n";

  const std::string not_found_response_html =
    "<html>\n<head>\n"
    "<title>Not Found</title>\n"
    "<h1>404 Page Not Found</h1>\n"
    "\n</head>\n</html>";

  response->SetStatus(Response::NOT_FOUND);
  response->AddHeader("Content-Type", "text/html");
  response->SetBody(not_found_response_html);

  return RequestHandler::NOT_FOUND;
}
