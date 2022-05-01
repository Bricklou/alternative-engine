#include "App.hpp"

App::App() {
  _app = new AltE::App();

  _app->init();
}

App::~App() { delete _app; }

void App::run() { _app->run(); }