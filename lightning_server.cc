#include "config_parser.h"
#include "lightning_server.h"
#include "request_handlers.h"
#include "request_router.h"
#include "server_config.h"
#include "request.h"
#include "response.h"

#include <iostream>
#include <cstddef>

LightningServer::LightningServer(const NginxConfig config_)
  : server_config_(config_),
    io_service_(),
    acceptor_(io_service_)
{
}

LightningServer::~LightningServer() {}

void LightningServer::start() {
  // Parse the config file
  if (! server_config_.build()) {
    std::cout << "Stopping server: invalid config\n";
    return;
  }

  // All server stats are recorded into this single instance
  ServerStats server_stats;
  server_stats.recordConfig(server_config_);

  // Register and initialize the routers
  RequestRouter router(&server_stats);
  if (! router.buildRoutes(server_config_)) {
    std::cout << "Invalid routes in server config file\n";
    return;
  }

  // Get the port to listen on
  std::vector<std::string> query = {"port"};
  server_config_.propertyLookUp(query, port_);

  // Setup server to listen for TCP connection on config file specified port
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(port_));
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  std::cout << "\nLightningServer now listening on port: " << port_ << "\n\n";

  // Lightning listening loop
  for (;;) {
    // Accept connection request
    boost::asio::ip::tcp::socket socket(io_service_);
    acceptor_.accept(socket);

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
        continue;
    }

    // Preparing Request and Response objects to pass into
    // the handlers' handleRequest function
    const std::string req_string(request_buffer);
    std::unique_ptr<Request> request = Request::Parse(req_string);
    Response response;

    // Routing requests to their handlers
    RequestHandler* handler = router.routeRequest(request->uri());
    RequestHandler::Status status_code = handler->handleRequest(*request, &response);

    server_stats.recordInteraction(*request, response);

    // Write back response
    std::string response_string = response.ToString();
    boost::asio::write(socket,
                       boost::asio::buffer(response_string.c_str(),
                                           response_string.size()));
  }
}
