#include "config_parser.h"
#include "lightning_server.h"

#include <array>
#include <iostream>
#include <string>

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

    // TODO: Avoid char-array/C-strings
    char request_buffer[8192];

    for (;;) {
      // Accept connection request
      boost::asio::ip::tcp::socket socket(io_service_);
      acceptor_.accept(socket);

      boost::system::error_code ec;
      size_t request_length = socket.read_some(boost::asio::buffer(request_buffer), ec);
      std::cout << request_buffer << std::endl;

      // Echo connection request, pasting in the request after headers,
      // then the double CRLF at the bottom
      char response_buffer[8192] = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
      const char* END_HTTP = "\r\n\r\n";
      memcpy(&response_buffer[OK_200_LENGTH], request_buffer, request_length);
      memcpy(&response_buffer[OK_200_LENGTH + request_length], END_HTTP, END_HTTP_LENGTH);
      boost::asio::write(socket,
                        boost::asio::buffer(response_buffer,
                                            OK_200_LENGTH + request_length));

    }
}
