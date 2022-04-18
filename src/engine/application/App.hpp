#pragma once

#include "Window.hpp"
#include <thread>

namespace AltE {
  class App {
    public:
      App();
      ~App();

      void run();

    private:
      std::thread _gameLogicThread;
      std::thread _renderThread;
      std::unique_ptr<Window> _window;
  };
} // namespace AltE