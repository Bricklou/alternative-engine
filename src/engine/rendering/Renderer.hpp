#pragma once

#include "../application/Window.hpp"
#include "./vulkan/VulkanInstance/VulkanInstance.hpp"
#include <memory>

namespace AltE::Rendering {

  class Renderer {
    public:
      explicit Renderer(Application::Window *window);
      ~Renderer();

      void render();

    private:
      void init();
      void cleanup();

      Application::Window *_window;
      std::unique_ptr<VulkanInstance> _instance;
  };
} // namespace AltE::Rendering