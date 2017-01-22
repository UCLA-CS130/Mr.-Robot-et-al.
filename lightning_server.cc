#include "lightning_server.h"
#include "config_parser.h"
#include <iostream>
#include <string>

LightningServer::LightningServer(const char* file_name) 
	: io_service_(), acceptor_(io_service_), socket_(io_service_)
{

    config_parser_.Parse(file_name, &config_);
    std::cout << config_.ToString() << std::endl;
}

void LightningServer::start() {
    // TODO
}