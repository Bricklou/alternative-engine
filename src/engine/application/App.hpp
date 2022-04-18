#pragma once

#include "../core/logger.hpp"
#include "Window.hpp"
#include <atomic>
#include <thread>

namespace AltE {
  class App {
    public:
      App();
      ~App();

      void run();

    private:
      std::thread _renderThread;
      std::unique_ptr<Window> _window;

      void init();
      void runRenderLoop();
      void cleanup();

      std::atomic<bool> _should_close{false};
  };
} // namespace AltE