#include "config_parser.h"
#include "echo_request_handler.h"
#include "lightning_server.h"

#include <iostream>

// TODO: Refine debug output
// TODO: Namespacing
// TODO: Handle errors
// TODO: Update integration test script

LightningServer::LightningServer(const char* file_name)
  : io_service_(),
    acceptor_(io_service_)
{
  config_parser_.Parse(file_name, &config_);
  std::cout << config_.ToString() << std::endl;
  port_ = config_.statements_[0]->child_block_->statements_[0]->tokens_[1];
  std::cout << port_ << std::endl;
}

LightningServer::~LightningServer() {
  // TODO
}

void LightningServer::start() {
  // Setup server to listen for TCP connection on config file specified port
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(port_));
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  for (;;) {
    // Accept connection request
    boost::shared_ptr<boost::asio::ip::tcp::socket>
        socket(new boost::asio::ip::tcp::socket(io_service_));
    acceptor_.accept(*socket);

    // Read in request and construct echo response in EchoRequestHandler
    EchoRequestHandler* erh = new EchoRequestHandler(socket);
    char* response = erh->constructEcho();

    // Write request to socket
    boost::asio::write(*socket,
                      boost::asio::buffer(response, erh->getResponseSize()));

    delete erh;
  }
}
