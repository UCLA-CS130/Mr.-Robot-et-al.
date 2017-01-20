#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "I am only supposed to take ONE argument!" << std::endl;
    std::cerr << "But you gave me " << argc
              << " argument(s): \n" << std::endl;

    for (int i = 0; i < argc; i++) {
      std::cerr << "Argument " << i << ": "
                << argv[i] << std::endl;
    }
  }
}