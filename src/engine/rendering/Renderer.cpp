#include "Renderer.hpp"

#include <spdlog/spdlog.h>

namespace AltE::Rendering {
  Renderer::Renderer(Application::Window *window) : _window{window} {
    this->init();
  }

  Renderer::~Renderer() { this->cleanup(); }

  void Renderer::init() {
    _device = std::make_unique<Device>(_window);
    this->recreate_swapchain();
  }

  void Renderer::recreate_swapchain() {
    auto extent = _window->getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = _window->getExtent();
    }

    vkDeviceWaitIdle(_device->get_device());

    if (_swapchain == nullptr) {
      _swapchain = std::make_unique<SwapChain>(*_device, extent);
    } else {
      std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapchain);
      _swapchain = std::make_unique<SwapChain>(*_device, extent, oldSwapChain);

      if (!oldSwapChain->compareSwapFormats(*_swapchain.get())) {
        throw std::runtime_error("Swap chain image format has changed!");
      }
    }
  }

  void Renderer::cleanup() {}

  void Renderer::render() {}
} // namespace AltE::Rendering