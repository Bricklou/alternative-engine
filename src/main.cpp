#include "engine/application/App.h"

int main() {
  AltE::App app{};

  app.init();
  app.run();
  app.cleanup();

  return 0;
}