#include "lightning_server.h"
#include "config_parser.h"
#include <iostream>
#include <string>
#include <array>

LightningServer::LightningServer(const char* file_name)
  : io_service_(),
    acceptor_(io_service_),
    socket_(io_service_)
{
    config_parser_.Parse(file_name, &config_);
    std::cout << config_.ToString() << std::endl;
    std::string address = "localhost";
    std::string port = config_.statements_[0]->child_block_->statements_[0]->tokens_[1];
    std::cout << port << std::endl;

    // Setup server to listen for TCP connection on config file specified port
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::stoi(port));
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    std::array<char, 8192> buffer_;

    while (true) {
      // Accept connection request
      acceptor_.accept(socket_);
      boost::system::error_code ec;
      socket_.read_some(boost::asio::buffer(buffer_), ec);

      /*
      socket_.async_read_some(boost::asio::buffer(buffer_),
        [this, buffer_] (boost::system::error_code ec, std::size_t bytes_transferred)
        {
          std::cout << buffer_.data() << std::endl;
        });
        */
      std::cout << buffer_.data() << std::endl;
      break;
    }
    socket_.close();
}

void LightningServer::start() {
    // TODO
}
