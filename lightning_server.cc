#include "config_parser.h"
#include "lightning_server.h"
#include "request_handlers.h"
#include "request_router.h"
#include "server_config.h"
#include "request.h"
#include "response.h"

#include <iostream>
#include <cstddef>
#include <thread>
using boost::asio::ip::tcp;

LightningServer::LightningServer(const NginxConfig config_)
  : server_config_(config_),
    io_service_(),
    acceptor_(io_service_)
{
}

LightningServer::~LightningServer() {}

void processConnection(tcp::socket socket, RequestRouter* router, ServerStats* server_stats) {
  // Read in request
  char request_buffer[MAX_REQ_SIZE];
  boost::system::error_code ec;
  std::size_t request_buffer_size = socket.read_some(boost::asio::buffer(request_buffer), ec);

  switch (ec.value()) {
    case boost::system::errc::success:
      std::cout << "~~~~~~~~~~Request~~~~~~~~~~\n" << request_buffer << std::endl;
      break;
    default:
      std::cout << "Error reading from socket, error code: " << ec << std::endl;
      return;
  }

  // Preparing Request and Response objects to pass into
  // the handlers' handleRequest function
  const std::string req_string(request_buffer);
  std::unique_ptr<Request> request = Request::Parse(req_string);
  Response response;

  // Routing requests to their handlers
  RequestHandler* handler = router->routeRequest(request->uri());
  RequestHandler::Status status_code = handler->handleRequest(*request, &response);
  if (status_code == RequestHandler::NOT_FOUND) {
    router->notFoundHandler()->handleRequest(*request, &response);
  }

  server_stats->recordInteraction(*request, response);

  // Write back response
  std::string response_string = response.ToString();
  boost::asio::write(socket,
                     boost::asio::buffer(response_string.c_str(),
                                         response_string.size()));
}

void LightningServer::start() {
  // Parse the config file
  if (! server_config_.build()) {
    std::cout << "Stopping server: invalid config\n";
    return;
  }

  // All server stats are recorded into this single instance
  ServerStats server_stats;
  server_stats.init(server_config_);
  server_stats.recordConfig(server_config_);

  // Register and initialize the routers
  RequestRouter router(&server_stats);
  if (! router.buildRoutes(server_config_)) {
    std::cout << "Invalid routes in server config file\n";
    return;
  }

  // Get the port to listen on
  std::vector<std::string> query = {"port"};
  bool found_port = server_config_.propertyLookUp(query, port_);
  if (! found_port) {
    std::cout << "No port specified in config, stopping server\n";
    return;
  }

  // Set the number of threads if specified in the config
  std::vector<std::string> query_path = {"num_threads"};
  std::string num_threads;
  bool found_threads = server_config_.propertyLookUp(query_path, num_threads);
  if (!found_threads) {
    std::cout << "Number of threads not specified in config: defaulting to 0\n";
    num_threads_ = 0;
  }
  else {
    num_threads_ = std::stoi(num_threads);
  }

  // Setup server to listen for TCP connection on config file specified port
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(port_));
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  std::cout << "\nLightningServer now listening on port: " << port_ << "\n\n";

  // Lightning listening loop
  for (;;) {
    // Accept connection request and spawn a new thread for each
    tcp::socket socket(io_service_);
    acceptor_.accept(socket);
    std::thread(processConnection, std::move(socket), &router, &server_stats).detach();
  }
}

