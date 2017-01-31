#include "config_parser.h"
#include "lightning_server.h"
#include "request_handlers.h"
#include "server_config.h"

#include <iostream>

using namespace lightning_server;

// TODO: Handle errors

LightningServer::LightningServer(const char* file_name)
  : io_service_(),
    acceptor_(io_service_)
{
  config_parser_.Parse(file_name, &config_);
  std::cout << config_.ToString() << std::endl;
  ServerConfig SC(config_);
  port_ = SC.propertyLookUp("port");
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
    boost::shared_ptr<boost::asio::ip::tcp::socket>
        socket(new boost::asio::ip::tcp::socket(io_service_));
    acceptor_.accept(*socket);

    // Read in request and handle echo response in echoRequestHandler,
    // returning the number of bytes written back out to the socket.

    char response_buffer[MAX_REQ_SIZE];
    size_t response_size = 0;

    char* response = request_handlers::echoRequestHandler(socket,
                                                          response_buffer,
                                                          response_size);
    if (response == nullptr) {
      std::cout << "ERROR: echoRequestHandler failed!" << std::endl;
      return;
    }

    // Write back response
    boost::asio::write(*socket,
                       boost::asio::buffer(response_buffer, response_size));
  }
}
