#pragma once

#include "../application/Window.hpp"
#include "./vulkan/CommandBuffers/Commandbuffers.hpp"
#include "./vulkan/Device/Device.hpp"
#include "./vulkan/SwapChain/SwapChain.hpp"
#include "./vulkan/VulkanInstance/VulkanInstance.hpp"
#include <memory>

namespace AltE::Rendering {

  class Renderer {
    public:
      explicit Renderer(Application::Window *window);
      ~Renderer();

      Renderer(const Renderer &) = delete;
      Renderer &operator=(const Renderer &) = delete;

      void render();

    private:
      void init();
      void cleanup();

      Application::Window *_window;
      std::unique_ptr<VulkanInstance> _instance;
      std::unique_ptr<Device> _device;
      std::unique_ptr<SwapChain> _swapchain;
      std::unique_ptr<CommandBuffers> _command_buffers;

      void recreate_swapchain();
  };
} // namespace AltE::Rendering