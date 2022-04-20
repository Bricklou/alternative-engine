#include "SwapChain.hpp"

#include "vk_abstract.hpp"
#include <VkBootstrap.h>
#include <spdlog/spdlog.h>

#define VK_CHECK(x)                                                            \
  {                                                                            \
    VkResult err = x;                                                          \
    if (err != VK_SUCCESS) {                                                   \
      spdlog::error("Detected Vulkan error: {}", err);                         \
      abort();                                                                 \
    }                                                                          \
  }

namespace AltE::Rendering {
  SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent)
      : _device{deviceRef}, _window_extent{extent} {
    init();
  }

  SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent,
                       std::shared_ptr<SwapChain> previous)
      : _device{deviceRef}, _window_extent{extent}, _old_swapchain{previous} {
    init();
    _old_swapchain = nullptr;
  }

  SwapChain::~SwapChain() {
    vkDeviceWaitIdle(_device.get_device());

    _deletion_queue.flush();
  }

  void SwapChain::init() {
    createSwapchain();
    createRenderPass();
    createFramebuffers();
    createSyncObjects();

    spdlog::debug("Swapchain initialize");
  }

  void SwapChain::createSwapchain() {
    vkb::SwapchainBuilder swapchainBuilder{
        _device.get_chosen_gpu(), _device.get_device(), _device.get_surface()};

    vkb::Swapchain vkbSwapchain =
        swapchainBuilder
            .use_default_format_selection()
            // use vsync present mode
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(_window_extent.width, _window_extent.height)
            .build()
            .value();

    // store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchain_images = vkbSwapchain.get_images().value();
    _swapchain_image_views = vkbSwapchain.get_image_views().value();

    _swachain_image_format = vkbSwapchain.image_format;

    _deletion_queue.push_function([this] {
      vkDestroySwapchainKHR(this->_device.get_device(), this->_swapchain,
                            nullptr);
    });
  }

  void SwapChain::createRenderPass() {
    // we defined an attachment description for our main color image
    // the attachment is loaded as "clear" when renderpass start
    // the attachment is stored when renderpass ends
    // the attachment layout starts as "undefined", and transitions to "Present"
    // so its possible to display it we don't care about stencil, and don't use
    // multisampling

    VkAttachmentDescription color_attachment = {};
    color_attachment.format = _swachain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    // 1 dependency, which is from "outside" into the subpasss. And we can read
    // or write color
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependencies[] = {dependency};
    VkAttachmentDescription attachments[] = {color_attachment};

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // 2 attachments from attachment array
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &attachments[0];
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    // 2 dependencies from dependency array
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependencies[0];

    VK_CHECK(vkCreateRenderPass(_device.get_device(), &render_pass_info,
                                nullptr, &_render_pass));

    _deletion_queue.push_function([this] {
      vkDestroyRenderPass(this->_device.get_device(), this->_render_pass,
                          nullptr);
    });
  }

  void SwapChain::createFramebuffers() {
    // create the framebuffers for the swapchain images. This will connect the
    // render-pass to the images for rendering
    VkFramebufferCreateInfo fb_info =
        abstraction::framebuffer_create_info(_render_pass, _window_extent);

    const uint32_t swapchain_imagecount = _swapchain_images.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    for (int i = 0; i < swapchain_imagecount; i++) {
      VkImageView attachments[1];
      attachments[0] = _swapchain_image_views[i];

      fb_info.pAttachments = attachments;
      fb_info.attachmentCount = 1;

      VK_CHECK(vkCreateFramebuffer(_device.get_device(), &fb_info, nullptr,
                                   &_framebuffers[i]));

      _deletion_queue.push_function([this, i] {
        vkDestroyFramebuffer(this->_device.get_device(), this->_framebuffers[i],
                             nullptr);
        vkDestroyImageView(this->_device.get_device(),
                           this->_swapchain_image_views[i], nullptr);
      });
    }
  }

  void SwapChain::createSyncObjects() {
    // create synchronization structures
    // one fence to control when the gpu has finished rendering to the frame
    // and 2 semaphores to synchronize rendering with swapchain
    // we want the fence to start signalled so we can wait on it on the first
    // frame

    VkFenceCreateInfo fenceCreateInfo =
        abstraction::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    VkSemaphoreCreateInfo semaphoreCreateInfo =
        abstraction::semaphore_create_info();

    for (int i = 0; i < FRAME_OVERLAP; i++) {
      VK_CHECK(vkCreateFence(_device.get_device(), &fenceCreateInfo, nullptr,
                             &_device.get_frames()[i]._render_fence));

      // enqueue the destruction of the fence
      _deletion_queue.push_function([this, i] {
        vkDestroyFence(_device.get_device(),
                       _device.get_frames()[i]._render_fence, nullptr);
      });

      VK_CHECK(vkCreateSemaphore(_device.get_device(), &semaphoreCreateInfo,
                                 nullptr,
                                 &_device.get_frames()[i]._present_semaphore));
      VK_CHECK(vkCreateSemaphore(_device.get_device(), &semaphoreCreateInfo,
                                 nullptr,
                                 &_device.get_frames()[i]._render_semaphore));

      _deletion_queue.push_function([this, i] {
        vkDestroySemaphore(_device.get_device(),
                           _device.get_frames()[i]._present_semaphore, nullptr);
        vkDestroySemaphore(_device.get_device(),
                           _device.get_frames()[i]._render_semaphore, nullptr);
      });
    }
    VkFenceCreateInfo uploadFenceCreateInfo = abstraction::fence_create_info();

    VK_CHECK(vkCreateFence(_device.get_device(), &uploadFenceCreateInfo,
                           nullptr,
                           &_device.get_upload_content()._uploadFence));

    _deletion_queue.push_function([this] {
      vkDestroyFence(this->_device.get_device(),
                     this->_device.get_upload_content()._uploadFence, nullptr);
    });
  }
} // namespace AltE::Rendering