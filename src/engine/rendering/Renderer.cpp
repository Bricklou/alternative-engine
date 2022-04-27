#include "Renderer.hpp"

#include <spdlog/spdlog.h>

namespace AltE::Rendering {
  Renderer::Renderer(Application::Window *window) : _window{window} {
    this->init();
  }

  Renderer::~Renderer() { this->cleanup(); }

  void Renderer::init() {
    _instance = std::make_unique<VulkanInstance>(_window);
    _device = std::make_unique<Device>(_instance.get(), _window);

    recreate_swapchain();

    _command_buffers =
        std::make_unique<CommandBuffers>(_device.get(), _swapchain.get());
    _command_buffers->create();
  }

  void Renderer::cleanup() { _command_buffers->destroy(); }

  void Renderer::render() {
    if (_window->was_resized()) {
      recreate_swapchain();
      return;
    }

    auto commandBuffer = _command_buffers->begin_frame();

    if (commandBuffer.has_value()) {
      _command_buffers->begin_swapchain_renderpass(commandBuffer.value());

      // Render things

      _command_buffers->end_swapchain_renderpass(commandBuffer.value());
      bool success = _command_buffers->end_frame();
      if (!success) {
        recreate_swapchain();
      }
    }

    _device->device().waitIdle();
  }

  void Renderer::recreate_swapchain() {
    auto extent = _window->extent();

    _device->device().waitIdle();

    if (_swapchain == nullptr) {
      _swapchain =
          std::make_unique<SwapChain>(_device.get(), extent);
    } else {
      std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapchain);
      _swapchain =
          std::make_unique<SwapChain>(_device.get(), extent, oldSwapChain);

      if (!oldSwapChain->compare_swap_formats(*_swapchain)) {
        throw std::runtime_error("Swapchain image format has changed");
      }
      _command_buffers->update_swapchain(_swapchain.get());
    }


    _window->reset_resize_flag();
  }
} // namespace AltE::Rendering