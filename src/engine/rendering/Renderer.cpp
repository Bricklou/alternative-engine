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
    this->init_descriptors();
    this->init_descriptors();
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

  void Renderer::init_descriptors() {

    _global_pool =
        DescriptorPool::Builder(*_device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10)
            .build();

    auto globalSetLayout =
        DescriptorSetLayout::Builder(*_device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        VK_SHADER_STAGE_VERTEX_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                        VK_SHADER_STAGE_VERTEX_BIT |
                            VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();
  }

  void Renderer::cleanup() {}

  void Renderer::render() {}
} // namespace AltE::Rendering