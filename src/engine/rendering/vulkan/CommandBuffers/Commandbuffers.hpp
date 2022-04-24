#pragma once

#include "../Device/Device.hpp"
#include "../SwapChain/SwapChain.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
#include <optional>

namespace AltE::Rendering {

  class CommandBuffers {
    public:
      explicit CommandBuffers(Device *device, SwapChain *swapchain);
      ~CommandBuffers();

      void create();
      void destroy();

      std::optional<vk::CommandBuffer> begin_frame();
      bool end_frame();

      vk::CommandBuffer &get_current_command_buffer() {
        return _command_buffers[_current_image_index];
      }

      [[nodiscard]] uint32_t current_frame_index() const { return _current_image_index; }

      void begin_swapchain_renderpass(vk::CommandBuffer commandBuffer);
      void end_swapchain_renderpass(vk::CommandBuffer commandBuffer);

    private:
      std::vector<vk::CommandBuffer> _command_buffers;
      Device *_device;
      SwapChain *_swapchain;

      bool _is_frame_started = false;
      uint32_t _current_image_index;
  };

} // namespace AltE::Rendering
