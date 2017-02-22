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
  std::vector<std::string> query = {"port"};
  server_config_.propertyLookUp(query, port_);
  std::cout << port_ << std::endl;
}

LightningServer::~LightningServer() {}

void LightningServer::start() {
  // Setup server to listen for TCP connection on config file specified port
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(port_));
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

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
        std::cout << "Error reading from socket, code: " << ec << std::endl;
        continue;
    }

    // Handle echo response in external handler
    char* response_buffer = nullptr;
    size_t response_buffer_size  = 0;

    RequestRouter router;
    if (! router.buildRoutes(server_config_)) {
      std::cout << "Invalid routes in server config file\n";
      return;
    }

    // TODO: Route requests to their handlers

    // TODO: Use-after-free vulnerability if response_buffer is used after
    // EchoRequestHandler is out of scope

    if (response_buffer_size == 0) {
      std::cout << "Failed to route due to invalid request\n";
      continue;
    }

    // Write back response
    boost::asio::write(socket,
                       boost::asio::buffer(response_buffer, response_buffer_size));
    delete response_buffer;
  }
}
