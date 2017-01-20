#include "config_parser.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: ./config_parser <path to config file>\n" << std::endl;
    return 1;
  }

  return 0;
}