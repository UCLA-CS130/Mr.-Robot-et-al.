#include "lightning_server.h"
#include "request_handlers.h"
#include "mime_types.h"
#include "response.h"
#include "request.h"
#include "server_config.h"
#include "./cpp-markdown/markdown.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
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
  std::string extension = "";

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
  if (!boost::filesystem::exists(full_path) || extension == "") {
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

  if (extension == "md") {
    markdown::Document doc;
    doc.read(reply);

    // markdown::write write out to an ostream, so we need to convert it to a string
    std::ostringstream stream;
    doc.write(stream);
    std::string md_html =  stream.str();

    response->SetBody(md_html);
  }
  else {
    response->SetBody(reply);
  }

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

bool ReverseProxyRequestHandler::init(const std::string& uri_prefix,
                                      const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  config_ = ServerConfig(config);
  // TODO: do logging/check return value of build
  config_.build();
  return true;
}

RequestHandler::Status ReverseProxyRequestHandler::handleRequest(const Request& request,
                                                                 Response* response) {
  std::cout << "ReverseProxyRequestHandler currently responding.\n";

  std::string reverse_proxy_host;
  std::string reverse_proxy_port;
  std::vector<std::string> query_host = {"reverse_proxy_host"};
  std::vector<std::string> query_port = {"reverse_proxy_port"};

  config_.printPropertiesMap();
  if (!config_.propertyLookUp(query_host, reverse_proxy_host)) {
    std::cout << "Failed to specify proxy host.\n";
    return RequestHandler::NOT_FOUND; 
  }

  if (!config_.propertyLookUp(query_port, reverse_proxy_port)) {
    std::cout << "Note: Reverse proxy port not specified. Querying host only: " 
              << reverse_proxy_host << std::endl;
  }

  // New connection setup
  boost::asio::io_service io_service;
  tcp::resolver resolver(io_service);
  boost::system::error_code ec;

  // Attempt to query the host using the HTTP protocol
  std::string port = "http"; 
  if (reverse_proxy_port != "") {
    port = reverse_proxy_port;
  }
  std::string modified_proxy_host = 
  ReverseProxyRequestHandler::sanitizeProxyHost(reverse_proxy_host); 
  
  tcp::resolver::query query(modified_proxy_host, port);
  tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  
  // Try each endpoint until we succesfully establish a connection
  tcp::socket socket(io_service);
  boost::asio::connect(socket, endpoint_iterator, ec);
  if (ec) {
    // TODO: Return 5xx error instead of 404 
    std::cout << "Error establishing connection with reverse proxy host: " 
              << reverse_proxy_host << std::endl;
    return RequestHandler::NOT_FOUND;
  }
  
  // Modify request: 
  // Need to handle cases such as: /proxy/static1/file.txt 
  std::string proxy_path = ReverseProxyRequestHandler::obtainPath(request.uri());

  Request r; 
  if (proxy_path == "") {
    r.AddRequestLine("GET", "/", "HTTP/1.1");
  }
  else {
    r.AddRequestLine("GET", proxy_path, "HTTP/1.1");
  } 
  r.AddHeader("Host", reverse_proxy_host);
  r.AddHeader("Accept", "*/*");
  r.AddHeader("Connection", "close") ; 

  std::string reverse_proxy_request = r.ToString(); 
  std::cout << "=== MODIFIED REQUEST ===\n" << reverse_proxy_request << std::endl;
  
  // Send the request: 
  boost::asio::write(socket,
                     boost::asio::buffer(reverse_proxy_request.c_str(),
                                         reverse_proxy_request.size()));

  // Receive response: 
  std::string raw_response; 
  if (!ReverseProxyRequestHandler::obtainResponse(socket, raw_response)) {
    return RequestHandler::NOT_FOUND;
  }

  // Parse raw response 
  auto parsed_response = Response::Parse(raw_response);
  if (parsed_response == nullptr) {
    std::cout << "An error occured parsing the response.\n";
    return RequestHandler::NOT_FOUND;
  }

  std::string parsed_resp_str = parsed_response->ToString();
  std::cout << "=== RESPONSE FROM REMOTE ===\n" << parsed_resp_str << std::endl;

  // Handle redirect - handles at most one redirect
  if (parsed_response->statusCode() == "302" || parsed_response->statusCode() == "301") {
    std::cout << "Handling redirect...\n"; 
    std::string location_val; 
    if (!parsed_response->GetHeader("Location", location_val)) {
      std::cout << "Redirect response does not specify location\n"; 
      return RequestHandler::NOT_FOUND;
    }

    std::string redirect_host = ReverseProxyRequestHandler::sanitizeProxyHost(location_val);
    std::cout << "REDIRECT LOCATION: " << redirect_host << std::endl;

    tcp::resolver::query redirect_query(redirect_host, port);
    boost::asio::connect(socket, resolver.resolve(redirect_query), ec);
    if (ec) {
      // TODO: Return 5xx error instead of 404 
      std::cout << "Error establishing connection with reverse proxy redirect host: "
                 << redirect_host << std::endl;
      return RequestHandler::NOT_FOUND;
    }

    Request redirect;
    if (proxy_path == "") {
      redirect.AddRequestLine("GET", "/", "HTTP/1.1");
    }
    else {
      redirect.AddRequestLine("GET", proxy_path, "HTTP/1.1");
    } 
    redirect.AddHeader("Host", redirect_host);
    redirect.AddHeader("Accept", "*/*");
    redirect.AddHeader("Connection", "close") ; 

    std::string redirect_request = redirect.ToString(); 
    std::cout << "=== REDIRECT REQUEST ===\n" << redirect_request << std::endl;
  
    // Send the redirect request: 
    boost::asio::write(socket,
                       boost::asio::buffer(redirect_request.c_str(),
                                           redirect_request.size()));
    // Receive redirect response
    std::string raw_redirect_response; 
    if (!ReverseProxyRequestHandler::obtainResponse(socket, raw_redirect_response)) {
      std::cout << "Response from redirect failed to be read.\n";
      return RequestHandler::NOT_FOUND;
    }
    
    // Parse raw redirect response 
    auto parsed_redirect_response = Response::Parse(raw_redirect_response);
    if (parsed_redirect_response == nullptr) {
      std::cout << "An error occured parsing the response.\n";
      return RequestHandler::NOT_FOUND;
    }

    if (parsed_redirect_response->statusCode() == "302" || 
        parsed_redirect_response->statusCode() == "301") {
      std::cout << "Attempting to redirect request more than once.\n";
      return RequestHandler::NOT_FOUND;
    }

    // Request successful, write response to server
    std::string parsed_redirect_resp_str = parsed_redirect_response->ToString();
    std::cout << "=== REDIRECT RESPONSE FROM REMOTE ===\n" 
              << parsed_redirect_resp_str << std::endl;

    // Below we are casting the unique_ptr get() call into a raw pointer
    // response is also casted to prevent the compiler from complaining
    *(response) = *(parsed_redirect_response.get());

    return RequestHandler::OK;
  
  }

  // Request successful, write response to server
  /// Below we are casting the unique_ptr get() call into a raw pointer
  // response is also casted to prevent the compiler from complaining
  *(response) = *(parsed_response.get());

  return RequestHandler::OK;
}

// Takes in an uri of the following format: /proxy_path/foo/bar
// and obtains the path following /proxy_path => /foo/bar
// 
// If nothing follows the uri like so: /proxy_path
// The function will return an empty string

std::string ReverseProxyRequestHandler::obtainPath(std::string uri) {
  // Assumption: non-root paths for ReverseProxyRequestHandler will be
  // prefixed with the word "proxy" in its path
  // IE /reverse_proxy_path, /proxy, ect
  
  // Handle case where files are served from "/"
  // IE /static/index.html, return the uri 
  boost::iterator_range<string::iterator> find_proxy = boost::find_nth(uri, "proxy", 0); 
  if (!find_proxy) {
    return uri; 
  }
  std::string default_path = "";
  boost::iterator_range<string::iterator> r = boost::find_nth(uri, "/", 1);
  
  if (r) {
    std::string path(r.begin(), uri.end());
    return path;
  }
  return default_path;
}

// If a remote proxy host is in the form: http:://www.foo.com/ return
// www.foo.com instead. 
std::string ReverseProxyRequestHandler::sanitizeProxyHost(std::string& remote_proxy_host) {

  // Remove trailing forward slash 
  if (remote_proxy_host.back() == '/') {
    remote_proxy_host.pop_back(); 
  }

  boost::iterator_range<string::iterator> r = boost::find_nth(remote_proxy_host, "www.", 0); 
  if (r) {
    std::string sanitized_host(r.begin(), remote_proxy_host.end()); 
    return sanitized_host; 
  }

  return remote_proxy_host;
}

// Obtain the response being written to socket 
bool ReverseProxyRequestHandler::obtainResponse(tcp::socket& socket, std::string& raw_response) {
  boost::asio::streambuf remote_response_buffer;
  boost::system::error_code ec;
  std::size_t bytes_read;
  while ((bytes_read = boost::asio::read(socket, remote_response_buffer, 
                       boost::asio::transfer_at_least(1), ec))) {

    // Read the data from buffer into string
    // Taken from: http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/overview/core/buffers.html
    std::string read_data = std::string(boost::asio::buffers_begin(remote_response_buffer.data()),
                                        boost::asio::buffers_begin(remote_response_buffer.data()) + bytes_read);
    
    raw_response += read_data;
    // Remove the bytes read from the buffer
    remote_response_buffer.consume(bytes_read);
  }
  if (ec != boost::asio::error::eof) {
    // Error reading.
    std::cout << "Error reading response.\n";
    return false;
  }

  return true;
}

bool StatusHandler::init(const std::string& uri_prefix,
                        const NginxConfig& config) {
  server_stats_ = nullptr;
  return true;
}

RequestHandler::Status StatusHandler::handleRequest(const Request& request,
                                                    Response* response) {
  std::cout << "StatusHandler currently responding.\n";

  // Get the prefix-to-handler map
  std::unordered_map<std::string, std::string> prefix_to_handlers
    = server_stats_->allRoutes();

  // Get the (url, status_code)-to-count map
  std::unordered_map<std::vector<string>,
                     int,
                     tuple_hash<std::vector<string>>>
  tuple_to_count
    = server_stats_->handlerCallDistribution();

  // Print both out nicely in response
  std::string reply = "Available Handlers\n";
  for (auto it : prefix_to_handlers) {
    std::string line = it.first + " <--- " + it.second + "\n";
    reply += line;
  }
  reply += "\n";

  std::string table = "Count | URL Requested                                   | Status Code\n"
                      "---------------------------------------------------------------------\n";
  const char separator = ' ';
  const int count_width = 8;
  const int url_width = 50;
  const int status_width = 11;

  reply += table;
  for (auto it : tuple_to_count) {
    std::stringstream line;
    line << std::to_string(it.second)
         << std::setw(count_width-(std::to_string(it.second)).length()) << "| "
         << it.first[0] << std::setw(url_width-it.first[0].length()) << "| "
         << it.first[1] << std::setw(status_width-it.first[1].length()) << "\n";
    reply += line.str();
  }

  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", "text/plain");
  response->SetBody(reply);

  return RequestHandler::OK;
}

void StatusHandler::setUpStats(ServerStats* server_stats) {
  server_stats_ = server_stats;
}
