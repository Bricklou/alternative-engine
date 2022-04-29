#include "engine/App.hpp"
#include <iostream>

int main() {
  try {
    AltE::App app{};
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
