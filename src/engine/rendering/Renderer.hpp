#pragma once

#include "../application/Window.hpp"
#include "./vulkan/CommandBuffers/Commandbuffers.hpp"
#include "./vulkan/Device/Device.hpp"
#include "./vulkan/SwapChain/SwapChain.hpp"
#include "./vulkan/VulkanInstance/VulkanInstance.hpp"
#include "./ImGui/ImGui.hpp"
#include <memory>

namespace AltE::Rendering {

  class Renderer {
    public:
      explicit Renderer(Application::Window *window);
      ~Renderer();

      Renderer(const Renderer &) = delete;
      Renderer &operator=(const Renderer &) = delete;

      void render();

      VulkanInstance* get_instance() { return _instance.get();}
      Device* get_device() { return _device.get(); }
      SwapChain* get_swapchain() { return _swapchain.get(); }
      Application::Window* get_window() { return _window; }
      CommandBuffers* get_command_buffers() { return _command_buffers.get(); }

    private:
      void init();
      void cleanup();

      Application::Window *_window;
      std::unique_ptr<VulkanInstance> _instance;
      std::unique_ptr<Device> _device;
      std::unique_ptr<SwapChain> _swapchain;
      std::unique_ptr<CommandBuffers> _command_buffers;
      std::unique_ptr<ImGuiRenderer> _imgui_renderer;

      void recreate_swapchain();
  };
} // namespace AltE::Rendering