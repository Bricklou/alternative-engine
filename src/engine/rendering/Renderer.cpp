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
  }

  void Renderer::cleanup() {}

  void Renderer::render() {}
} // namespace AltE::Rendering