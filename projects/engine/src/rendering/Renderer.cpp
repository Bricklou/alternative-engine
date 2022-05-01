#include <alte/rendering/Renderer.hpp>

#include <optional>
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

    _imgui_renderer = std::make_unique<ImGuiRenderer>(this);
  }

  void Renderer::cleanup() { _command_buffers->destroy(); }

  void
  Renderer::render(std::optional<RenderCallback> &render_callback,
                   std::optional<PostRenderCallback> &post_render_callback) {
    if (_window->was_resized()) {
      recreate_swapchain();
      return;
    }

    auto commandBuffer = _command_buffers->begin_frame();

    if (commandBuffer.has_value()) {
      _command_buffers->begin_swapchain_renderpass(commandBuffer.value());

      _imgui_renderer->prepare();

      // Render things
      if (render_callback) {
        render_callback.value()(this);
      }

      _imgui_renderer->render(commandBuffer.value());

      if (post_render_callback) {
        post_render_callback.value()(this);
      }

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
      _swapchain = std::make_unique<SwapChain>(_device.get(), extent);
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