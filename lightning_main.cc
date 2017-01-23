#include "config_parser.h"
#include "lightning_server.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: lightning <path to config file>\n" << std::endl;
    return 1;
  }

  LightningServer server(argv[1]);
  server.start();

  return 0;
}