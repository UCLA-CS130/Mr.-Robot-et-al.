#ifndef LIGHTNING_SERVER_H
#define LIGHTNING_SERVER_H

#include "config_parser.h"
#include <boost/asio.hpp>

const int MAX_REQ_SIZE = 8192; // bytes

class LightningServer {
  public:
    LightningServer(const char* file_name);
    ~LightningServer();
    void start();


  private:
    NginxConfig config_;
    NginxConfigParser config_parser_;
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string port_;
};

#endif
