#include "config_parser.h"
#include "lightning_server.h"
#include "request_handlers.h"
#include "server_config.h"
#include "request_handlers.h"

#include <iostream>

// TODO: Handle errors

LightningServer::LightningServer(const char* file_name)
  : io_service_(),
    acceptor_(io_service_)
{
  config_parser_.Parse(file_name, &config_);
  std::cout << config_.ToString() << std::endl;
  ServerConfig ConfigWrapper(config_);
  port_ = ConfigWrapper.propertyLookUp("port");
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

    EchoRequestHandler echo_request_handler;
    echo_request_handler.handleRequest(request_buffer,
                                        request_buffer_size,
                                        response_buffer,
                                        response_buffer_size);

    // Write back response
    boost::asio::write(socket,
                       boost::asio::buffer(response_buffer, response_buffer_size));
  }
}
