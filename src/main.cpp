#include "engine/application/App.hpp"

int main() {
  AltE::App app{};

  app.init();
  app.run();
  app.cleanup();

  return 0;
}