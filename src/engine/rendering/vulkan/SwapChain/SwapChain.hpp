#pragma once

#include "../../../application/Window.hpp"
#include "../VulkanInstance/VulkanInstance.hpp"

namespace AltE::Rendering {
  class SwapChain {
    public:
      explicit SwapChain(VulkanInstance *instance, Application::Window *window);
      ~SwapChain();

      vk::SwapchainKHR swapchain() { return _swapchain; }

    private:
      // the swapchain isntance
      vk::SwapchainKHR _swapchain;
      // image format expected by the windowing system
      vk::Format _swapchain_image_format;

      // array of images from the swapchain
      std::vector<vk::Image> _swapchain_images;
      std::vector<vk::ImageView> _swapchain_image_views;
  };
} // namespace AltE::Rendering