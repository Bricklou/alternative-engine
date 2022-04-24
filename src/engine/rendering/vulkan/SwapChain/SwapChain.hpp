#pragma once

#include "../Device/Device.hpp"
#include "../../../application/Window.hpp"
#include <vulkan/vulkan.hpp>

namespace AltE::Rendering {
  class SwapChain {
    public:
      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

      explicit SwapChain(Device *device, vk::Extent2D window_extent);
      explicit SwapChain(Device *device, vk::Extent2D window_extent,
                         std::shared_ptr<SwapChain> oldSwapChain);
      ~SwapChain();

      SwapChain(const SwapChain &) = delete;
      SwapChain &operator=(const SwapChain &) = delete;

      vk::SwapchainKHR swapchain() { return _swapchain; }
      size_t imageCount() { return _swapchain_images.size(); }

      vk::Framebuffer getFrameBuffer(int index) {
        return _swapchain_framebuffers[index];
      }
      vk::RenderPass getRenderPass() { return _render_pass; }
      vk::ImageView getImageView(int index) {
        return _swapchain_image_views[index];
      }

      vk::Format image_format() { return _swapchain_image_format; }
      vk::Extent2D extent() { return _swapchain_extent; }
      [[nodiscard]] uint32_t width() const { return _swapchain_extent.width; }
      [[nodiscard]] uint32_t height() const { return _swapchain_extent.height; }

      [[nodiscard]] float extent_aspect_ratio() const {
        return static_cast<float>(_swapchain_extent.width) /
               static_cast<float>(_swapchain_extent.height);
      }

      vk::Result acquire_next_image(uint32_t *imageIndex);
      vk::Result submit_command_buffers(const vk::CommandBuffer *buffers,
                                        uint32_t *imageIndex);

      [[nodiscard]] bool compare_swap_formats(const SwapChain &swapChain) const {
        return swapChain._swapchain_image_format == _swapchain_image_format;
      }

    private:
      // the swapchain isntance
      vk::SwapchainKHR _swapchain;
      // image format expected by the windowing system
      vk::Format _swapchain_image_format;
      // swapchain extent
      vk::Extent2D _swapchain_extent;
      // window extent
      vk::Extent2D _window_extent;

      // render pass
      vk::RenderPass _render_pass;

      // the old swapchain
      std::shared_ptr<SwapChain> _old_swapchain;

      // array of images from the swapchain
      std::vector<vk::Image> _swapchain_images;
      std::vector<vk::ImageView> _swapchain_image_views;

      // framebuffers
      std::vector<vk::Framebuffer> _swapchain_framebuffers;

      Device *_device;

      // sync objects
      std::vector<vk::Semaphore> _image_available_semaphores;
      std::vector<vk::Semaphore> _render_finished_semaphores;
      std::vector<vk::Fence> _in_flight_fences;
      std::vector<vk::Fence> _images_in_flight;

      void init();
      void create_swapchain();
      void create_image_views();
      void create_renderpass();
      void create_framebuffers();
      void create_sync_objects();

      // Helper functions
      vk::SurfaceFormatKHR choose_swap_surface_format(
          const std::vector<vk::SurfaceFormatKHR> &availableFormats);
      vk::PresentModeKHR choose_swap_present_mode(
          const std::vector<vk::PresentModeKHR> &availablePresentModes);
      vk::Extent2D
      choose_swap_extent(const vk::SurfaceCapabilitiesKHR &capabilities);

      size_t _current_frame = 0;
  };
} // namespace AltE::Rendering