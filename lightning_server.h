#ifndef LIGHTNING_SERVER_H
#define LIGHTNING_SERVER_H

#include "config_parser.h"
#include <boost/asio.hpp>

class LightningServer {
	public:
	    LightningServer(const char* file_name);
	    void start();


	private:
	    NginxConfig config_;
	    NginxConfigParser config_parser_;
	    boost::asio::io_service io_service_;
	    boost::asio::ip::tcp::acceptor acceptor_;
	    boost::asio::ip::tcp::socket socket_;
};

#endif