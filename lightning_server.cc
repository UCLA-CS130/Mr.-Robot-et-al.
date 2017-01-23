#include "config_parser.h"
#include "lightning_server.h"

#include <array>
#include <iostream>
#include <string>

// TODO: Refine debug output
// TODO: Namespacing
// TODO: Handle errors
// TODO: Update integration test script
const int MAX_REQ_SIZE = 8192;

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
    boost::asio::ip::tcp::socket socket(io_service_);
    acceptor_.accept(socket);

    // Read in request
    char request_buffer[MAX_REQ_SIZE];
    boost::system::error_code ec;
    std::size_t bytes_read = socket.read_some(boost::asio::buffer(request_buffer), ec);
    std::cout << "~~~~~~~~~~Request~~~~~~~~~~\n" << request_buffer << std::endl;

    // Create response
    // Echo connection request, pasting in the request after headers,
    // then the double CRLF at the bottom
    const std::string res_header = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
    std::size_t header_size = res_header.size();
    std::size_t res_size = header_size + bytes_read;

    char response_buffer[res_size+1];
    res_header.copy(response_buffer, header_size);
    std::memcpy(&response_buffer[header_size], request_buffer, bytes_read);
    response_buffer[res_size] = '\0';

    std::cout << "~~~~~~~~~~Response~~~~~~~~~~\n" << response_buffer << std::endl;
    boost::asio::write(socket,
                      boost::asio::buffer(response_buffer, res_size));

  }
}
