#pragma once

#include <alte/App.hpp>

class App {
  public:
    App();
    ~App();

    void run();

  private:
    AltE::App *_app;
};