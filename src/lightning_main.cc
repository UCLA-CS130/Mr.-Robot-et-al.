#include "config_parser.h"
#include "lightning_server.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: lightning <path to config file>\n" << std::endl;
    return 1;
  }

  // Read config file
  NginxConfig config_;
  NginxConfigParser config_parser_;
  config_parser_.Parse(argv[1], &config_);
  std::cout << config_.ToString() << std::endl;

  std::cout << "~~~ Starting Lightning Server using the above config ~~~\n\n";
  LightningServer server(config_);
  server.start();

  return 0;
}
