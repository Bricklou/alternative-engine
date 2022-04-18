#include "engine/application/App.hpp"
#include <iostream>

int main() {
  AltE::App app{};

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
