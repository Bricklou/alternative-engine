#pragma once

#include "../application/Window.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/SwapChain.hpp"
#include <memory>

namespace AltE::Rendering {

  class Renderer {
    public:
      Renderer(Application::Window *window);
      ~Renderer();

      void render();

    private:
      void init();
      void cleanup();
      void recreate_swapchain();

      Application::Window *_window;
      std::unique_ptr<Device> _device;
      std::unique_ptr<SwapChain> _swapchain;
  };
} // namespace AltE::Rendering