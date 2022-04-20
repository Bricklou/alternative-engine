#pragma once

#include "Device.hpp"
#include <memory>

namespace AltE::Rendering {
  class SwapChain {
    public:
      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

      SwapChain(Device &deviceRef, VkExtent2D windowExtent);
      SwapChain(Device &deviceRef, VkExtent2D windowExtent,
                std::shared_ptr<SwapChain> previous);

      ~SwapChain();

      SwapChain(const SwapChain &) = delete;
      SwapChain &operator=(const SwapChain &) = delete;

      VkFramebuffer getFrameBuffer(int index) {
        return _swapchain_framebuffers[index];
      }

      VkRenderPass getRenderPass() { return _render_pass; }

      bool compareSwapFormats(const SwapChain &swapChain) const {
        return swapChain._swachain_image_format == _swachain_image_format;
      }

    private:
      void init();
      void createSwapchain();
      void createRenderPass();
      void createDepthResources();
      void createFramebuffers();
      void createSyncObjects();

      Device &_device;
      VkExtent2D _window_extent;
      std::vector<VkFramebuffer> _swapchain_framebuffers;

      VkRenderPass _render_pass;

      std::vector<VkFramebuffer> _framebuffers;
      std::vector<VkImage> _swapchain_images;
      std::vector<VkImageView> _swapchain_image_views;
      VkFormat _swachain_image_format;

      VkSwapchainKHR _swapchain;
      std::shared_ptr<SwapChain> _old_swapchain;

      DeletionQueue _deletion_queue;
  };
} // namespace AltE::Rendering