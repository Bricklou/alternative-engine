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
  }

  void Renderer::cleanup() {}

  void Renderer::render() {}

  void Renderer::recreate_swapchain() {
    auto extent = _window->extent();

    SDL_Event event;
    while(extent.width == 0 || extent.height == 0) {
      extent = _window->extent();
      SDL_WaitEvent(&event);
    }
    _device->device().waitIdle();

    if (_swapchain == nullptr) {
      _swapchain = std::make_unique<SwapChain>(_device.get(), _window->extent());
    } else {
      std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapchain);
      _swapchain = std::make_unique<SwapChain>(_device.get(), extent, oldSwapChain);

      if (!oldSwapChain->compare_swap_formats(*_swapchain)) {
        throw std::runtime_error("Swapchain image format has changed");
      }
    }
  }
} // namespace AltE::Rendering