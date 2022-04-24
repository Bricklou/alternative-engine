#include "SwapChain.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

#include <utility>

namespace AltE::Rendering {
  int SwapChain::MAX_FRAMES_IN_FLIGHT = 2;

  SwapChain::SwapChain(Device *device, vk::Extent2D window_extent)
      : _device{device}, _window_extent{window_extent} {
    init();
  }

  SwapChain::SwapChain(Device *device, vk::Extent2D window_extent,
                       std::shared_ptr<SwapChain> oldSwapChain)
      : _device{device}, _window_extent{window_extent},
        _old_swapchain{std::move(oldSwapChain)} {
    init();
    _old_swapchain = nullptr;
  }

  SwapChain::~SwapChain() {
    for (auto imageView : _swapchain_image_views) {
      _device->device().destroyImageView(imageView, nullptr);
    }
    _swapchain_image_views.clear();

    if (_swapchain) {
      _device->device().destroySwapchainKHR(_swapchain, nullptr);
      _swapchain = nullptr;
    }

    for (auto framebuffer : _swapchain_framebuffers) {
      _device->device().destroyFramebuffer(framebuffer, nullptr);
    }
    _device->device().destroyRenderPass(_render_pass, nullptr);

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      _device->device().destroySemaphore(_render_finished_semaphores[i],
                                         nullptr);
      _device->device().destroySemaphore(_image_available_semaphores[i],
                                         nullptr);
      _device->device().destroyFence(_in_flight_fences[i], nullptr);
    }
  }

  void SwapChain::init() {
    create_swapchain();
    create_image_views();
    create_renderpass();
    create_framebuffers();
    create_sync_objects();
  }

  void SwapChain::create_swapchain() {
    SwapChainSupportDetails swapChainSupport = _device->get_swapchain_support();

    vk::SurfaceFormatKHR surfaceFormat =
        choose_swap_surface_format(swapChainSupport.formats);
    vk::PresentModeKHR presentMode =
        choose_swap_present_mode(swapChainSupport.presentModes);
    vk::Extent2D extent = choose_swap_extent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    SwapChain::MAX_FRAMES_IN_FLIGHT = std::max(2, static_cast<int>(imageCount));

    vk::SwapchainCreateInfoKHR createInfo{
        {},
        _device->surface(),
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment};

    QueueFamilyIndices indices = _device->find_physical_queue_families();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily,
                                     indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
      createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
      createInfo.setQueueFamilyIndexCount(2);
      createInfo.setPQueueFamilyIndices(queueFamilyIndices);
    } else {
      createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
      createInfo.setQueueFamilyIndexCount(0);
      createInfo.setPQueueFamilyIndices(nullptr);
    }

    createInfo.setPreTransform(swapChainSupport.capabilities.currentTransform);
    createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

    createInfo.setPresentMode(presentMode);
    createInfo.setClipped(true);

    createInfo.setOldSwapchain(_old_swapchain == nullptr
                                   ? VK_NULL_HANDLE
                                   : _old_swapchain->_swapchain);

    if (_device->device().createSwapchainKHR(
            &createInfo, nullptr, &_swapchain) != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to create swapchain !");
    }

    vk::Result res;
    // we only specified a minimum number of images in the swap chain, so the
    // implementation is allowed to create a swap chain with more. That's why
    // we'll first query the final number of images with
    // vkGetSwapchainImagesKHR, then resize the container and finally call it
    // again to retrieve the handles.
    res = _device->device().getSwapchainImagesKHR(_swapchain, &imageCount,
                                                  nullptr);
    if (res != vk::Result::eSuccess) {
      throw std::runtime_error("failed to get swapchain images count");
    }
    _swapchain_images.resize(imageCount);
    res = _device->device().getSwapchainImagesKHR(_swapchain, &imageCount,
                                                  _swapchain_images.data());
    if (res != vk::Result::eSuccess) {
      throw std::runtime_error("failed to get swapchain images");
    }

    _swapchain_image_format = surfaceFormat.format;
    _swapchain_extent = extent;
  }

  void SwapChain::create_image_views() {
    _swapchain_image_views.resize(_swapchain_images.size());
    for (size_t i = 0; i < _swapchain_images.size(); i++) {
      vk::ImageViewCreateInfo viewInfo{
          {},
          _swapchain_images[i],
          vk::ImageViewType::e2D,
          _swapchain_image_format,
          {},
          {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};

      if (_device->device().createImageView(&viewInfo, nullptr,
                                            &_swapchain_image_views[i]) !=
          vk::Result::eSuccess) {
        throw std::runtime_error("failed to create texture image view!");
      }
    }
  }

  void SwapChain::create_renderpass() {
    vk::AttachmentDescription colorAttachment{{},
                                              _swapchain_image_format,
                                              vk::SampleCountFlagBits::e1,
                                              vk::AttachmentLoadOp::eClear,
                                              vk::AttachmentStoreOp::eStore,
                                              vk::AttachmentLoadOp::eDontCare,
                                              vk::AttachmentStoreOp::eDontCare,
                                              vk::ImageLayout::eUndefined,
                                              vk::ImageLayout::ePresentSrcKHR};

    vk::AttachmentReference colorAttachmentRef{
        0, vk::ImageLayout::eColorAttachmentOptimal};

    vk::SubpassDescription subpass{{}, vk::PipelineBindPoint::eGraphics,
                                   0,  nullptr,
                                   1,  &colorAttachmentRef};

    vk::SubpassDependency dependency{
        VK_SUBPASS_EXTERNAL,
        0,
        {vk::PipelineStageFlagBits::eColorAttachmentOutput |
         vk::PipelineStageFlagBits::eEarlyFragmentTests},
        {vk::PipelineStageFlagBits::eColorAttachmentOutput |
         vk::PipelineStageFlagBits::eEarlyFragmentTests},
        {},
        {vk::AccessFlagBits::eColorAttachmentWrite}};

    std::vector<vk::AttachmentDescription> attachments = {colorAttachment};
    vk::RenderPassCreateInfo renderPassInfo{
        {},
        static_cast<uint32_t>(attachments.size()),
        attachments.data(),
        1,
        &subpass,
        1,
        &dependency};

    if (_device->device().createRenderPass(
            &renderPassInfo, nullptr, &_render_pass) != vk::Result::eSuccess) {
      throw std::runtime_error("failed to create render pass!");
    }
  }

  void SwapChain::create_framebuffers() {
    _swapchain_framebuffers.resize(imageCount());
    for (size_t i = 0; i < imageCount(); i++) {
      std::vector<vk::ImageView> attachments = {_swapchain_image_views[i]};

      vk::Extent2D swapChainExtent = _swapchain_extent;
      vk::FramebufferCreateInfo framebufferInfo{
          {},
          _render_pass,
          static_cast<uint32_t>(attachments.size()),
          attachments.data(),
          swapChainExtent.width,
          swapChainExtent.height,
          1};

      if (_device->device().createFramebuffer(&framebufferInfo, nullptr,
                                              &_swapchain_framebuffers[i]) !=
          vk::Result::eSuccess) {
        throw std::runtime_error("failed to create framebuffer!");
      }
    }
  }

  void SwapChain::create_sync_objects() {
    _image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    _images_in_flight.resize(imageCount(), nullptr);

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{{vk::FenceCreateFlagBits::eSignaled}};

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (_device->device().createSemaphore(&semaphoreInfo, nullptr,
                                            &_image_available_semaphores[i]) !=
              vk::Result::eSuccess ||
          _device->device().createSemaphore(&semaphoreInfo, nullptr,
                                            &_render_finished_semaphores[i]) !=
              vk::Result::eSuccess ||
          _device->device().createFence(&fenceInfo, nullptr,
                                        &_in_flight_fences[i]) !=
              vk::Result::eSuccess) {
        throw std::runtime_error(
            "failed to create synchronization objects for a frame!");
      }
    }
  }

  vk::Result SwapChain::acquire_next_image(uint32_t *imageIndex) {
    vk::Result result;
    result = _device->device().waitForFences(
        1, &_in_flight_fences[_current_frame], true,
        std::numeric_limits<uint64_t>::max());

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("failed to wait for in flight fence");
    }

    result = _device->device().acquireNextImageKHR(
        _swapchain, std::numeric_limits<uint64_t>::max(),
        _image_available_semaphores[_current_frame], nullptr, imageIndex);

    return result;
  }

  vk::Result SwapChain::submit_command_buffers(const vk::CommandBuffer *buffers,
                                               uint32_t *imageIndex) {
    vk::Result res;
    if (_images_in_flight[*imageIndex]) {
      res = _device->device().waitForFences(1, &_images_in_flight[*imageIndex],
                                            true, UINT64_MAX);

      if (res != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fence");
      }
    }
    _images_in_flight[*imageIndex] = _in_flight_fences[_current_frame];

    vk::SubmitInfo submitInfo{};

    vk::Semaphore waitSemaphores[] = {
        _image_available_semaphores[_current_frame]};
    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);

    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(buffers);

    vk::Semaphore signalSemaphores[] = {
        _render_finished_semaphores[_current_frame]};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    res = _device->device().resetFences(1, &_in_flight_fences[_current_frame]);
    if (res != vk::Result::eSuccess) {
      throw std::runtime_error("failed to reset fence");
    }
    if (_device->graphics_queue().submit(1, &submitInfo,
                                         _in_flight_fences[_current_frame]) !=
        vk::Result::eSuccess) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo{};

    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(signalSemaphores);

    vk::SwapchainKHR swapChains[] = {_swapchain};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(swapChains);

    presentInfo.setPImageIndices(imageIndex);

    auto result = _device->present_queue().presentKHR(&presentInfo);

    _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
  }

  vk::SurfaceFormatKHR SwapChain::choose_swap_surface_format(
      const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
      if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
          availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
        return availableFormat;
      }
    }

    return availableFormats[0];
  }

  vk::PresentModeKHR SwapChain::choose_swap_present_mode(
      const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
      if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        spdlog::debug("Present mode: Mailbox");
        return availablePresentMode;
      }
    }

    spdlog::debug("Present mode: V-Sync");
    return vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D SwapChain::choose_swap_extent(
      const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()) {
      return capabilities.currentExtent;
    } else {
      vk::Extent2D actualExtent = _window_extent;
      actualExtent.width = std::max(
          capabilities.minImageExtent.width,
          std::min(capabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = std::max(
          capabilities.minImageExtent.height,
          std::min(capabilities.maxImageExtent.height, actualExtent.height));

      return actualExtent;
    }
  }
} // namespace AltE::Rendering