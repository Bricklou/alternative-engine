#include "Commandbuffers.hpp"
#include <spdlog/spdlog.h>

namespace AltE::Rendering {
  CommandBuffers::CommandBuffers(Device *device, SwapChain *swapchain)
      : _device{device}, _swapchain{swapchain} {}

  CommandBuffers::~CommandBuffers() {}

  void CommandBuffers::create() {
    _command_buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocInfo{
        _device->command_pool(), vk::CommandBufferLevel::ePrimary,
        static_cast<uint32_t>(_command_buffers.size())};
    if (_device->device().allocateCommandBuffers(
            &allocInfo, _command_buffers.data()) != vk::Result::eSuccess) {
      throw std::runtime_error("failed to allocate command buffers !");
    }
  }

  void CommandBuffers::destroy() {
    _device->device().freeCommandBuffers(
        _device->command_pool(), static_cast<uint32_t>(_command_buffers.size()),
        _command_buffers.data());
    _command_buffers.clear();
  }

  std::optional<vk::CommandBuffer> CommandBuffers::begin_frame() {
    assert(!_is_frame_started &&
           "Can't call begin_frame while already in progress");

    auto result = _swapchain->acquire_next_image(&_current_image_index);

    if (result == vk::Result::eErrorOutOfDateKHR) {
      return {};
    }

    if (result != vk::Result::eSuccess &&
        result != vk::Result::eSuboptimalKHR) {
      throw std::runtime_error("failed to acquire swapchain image");
    }

    _is_frame_started = true;

    auto commandBuffer = get_current_command_buffer();

    vk::CommandBufferBeginInfo beginInfo{};

    if (commandBuffer.begin(&beginInfo) != vk::Result::eSuccess) {
      throw std::runtime_error("failed to begin recording command buffer");
    }
    return commandBuffer;
  }

  bool CommandBuffers::end_frame() {
    assert(_is_frame_started &&
           "Can't call endFrame while frame is not in progress");

    auto commandBuffer = get_current_command_buffer();
    commandBuffer.end();

    auto result = _swapchain->submit_command_buffers(&commandBuffer,
                                                     &_current_image_index);
    if (result == vk::Result::eErrorOutOfDateKHR ||
        result == vk::Result::eSuboptimalKHR) {
      _is_frame_started = false;
      return false;
    } else if (result != vk::Result::eSuccess) {
      _is_frame_started = false;
      throw std::runtime_error("failed to prevent swapchain image");
    }

    _is_frame_started = false;
    _current_image_index =
        (_current_image_index + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;

    return true;
  }

  void
  CommandBuffers::begin_swapchain_renderpass(vk::CommandBuffer commandBuffer) {
    assert(_is_frame_started &&
           "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == get_current_command_buffer() &&
           "Can't begin render pass on command buffer from a different frame");

    vk::RenderPassBeginInfo renderPassInfo{
        _swapchain->render_pass(),
        _swapchain->framebuffer(_current_image_index),
        {{0, 0}, _swapchain->extent()}};

    std::vector<vk::ClearValue> clearValues(1);
    clearValues[0].setColor(std::array<float, 4>{0.01f, 0.01f, 0.01f, 1.f});
    renderPassInfo.setClearValueCount(
        static_cast<uint32_t>(clearValues.size()));
    renderPassInfo.setPClearValues(clearValues.data());

    commandBuffer.beginRenderPass(&renderPassInfo,
                                  vk::SubpassContents::eInline);

    vk::Viewport viewport{0.f,
                          0.f,
                          static_cast<float>(_swapchain->extent().width),
                          static_cast<float>(_swapchain->extent().height),
                          0.f,
                          1.f};
    vk::Rect2D scissor{{0, 0}, _swapchain->extent()};
    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
  }

  void
  CommandBuffers::end_swapchain_renderpass(vk::CommandBuffer commandBuffer) {
    assert(_is_frame_started &&
           "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == get_current_command_buffer() &&
           "Can't end render pass on command buffer from a different frame");
    commandBuffer.endRenderPass();
  }

} // namespace AltE::Rendering