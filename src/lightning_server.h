#ifndef LIGHTNING_SERVER_H
#define LIGHTNING_SERVER_H

#include "config_parser.h"
#include "server_config.h"
#include <boost/asio.hpp>

const int MAX_REQ_SIZE = 8192; // bytes

class LightningServer {
  public:
    LightningServer(const NginxConfig config_);
    ~LightningServer();
    void start();


  private:
    ServerConfig server_config_;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string port_;
    std::size_t num_threads_;
};

#endif
