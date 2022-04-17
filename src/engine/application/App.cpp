#include "App.hpp"
#include "../rendering/shader_utils.hpp"
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <SDL2/SDL_vulkan.h>
#include <VkBootstrap.h>

#define VK_CHECK(x)                                                            \
  {                                                                            \
    VkResult err = x;                                                          \
    if (err != VK_SUCCESS) {                                                   \
      spdlog::default_logger()->error("Detected Vulkan error: {}", err);       \
      abort();                                                                 \
    }                                                                          \
  }

namespace AltE {
  void App::init() {
    // We initialize SDL and create a window with it
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    // create blank SDL window for our application
    _window = SDL_CreateWindow(
        "Alternative-Engine",    // window title
        SDL_WINDOWPOS_UNDEFINED, // window position x (don't care)
        SDL_WINDOWPOS_UNDEFINED, // window position y (don't care)
        _windowExtent.width,     // window width in pixels
        _windowExtent.height,    // window height in pixels
        window_flags);

    // configure the logger
    init_logger();
    // load the core Vulkan structure
    init_vulkan();
    // create the swapchain
    init_swapchain();
    // create the command pool
    init_commands();
    // create the render pass
    init_default_renderpass();
    // create the framebuffers
    init_framebuffers();
    // create the render semaphores
    init_sync_structures();
    // create pipeline
    init_pipeline();

    // everthing went fine
    _isInitialized = true;
    spdlog::default_logger()->info("App initialized");
  }

  void App::cleanup() {
    if (_isInitialized) {
      // make sure the gpu has stopped doing its things
      vkDeviceWaitIdle(_device);

      vkDestroyCommandPool(_device, _commandPool, nullptr);

      // destroy sync objects
      vkDestroyFence(_device, _renderFence, nullptr);
      vkDestroySemaphore(_device, _renderSemaphore, nullptr);
      vkDestroySemaphore(_device, _presentSemaphore, nullptr);

      vkDestroySwapchainKHR(_device, _swapchain, nullptr);

      // destroy the main render pass
      vkDestroyRenderPass(_device, _renderPass, nullptr);

      // destroy swapchain resources
      for (int i = 0; i < _swapchainImageViews.size(); i++) {
        vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);

        vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
      }

      vkDestroySurfaceKHR(_instance, _surface, nullptr);

      vkDestroyDevice(_device, nullptr);
      vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
      vkDestroyInstance(_instance, nullptr);

      SDL_DestroyWindow(_window);
    }
    spdlog::default_logger()->debug("Engine closed");
  }

  void App::draw() {
    // wait until the GPU has finished rendering the last frame. Timeout of 1
    // second
    VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000));
    VK_CHECK(vkResetFences(_device, 1, &_renderFence));

    // request image from the swapchain, one second timeout
    uint32_t swapchainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000,
                                   _presentSemaphore, nullptr,
                                   &swapchainImageIndex));

    // now that we are sure that the commands finished executing, we can safely
    // reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));

    // naming it cmd for shorter writing
    VkCommandBuffer cmd = _mainCommandBuffer;

    // begin the command buffer recording. We will use this command buffer
    // exactly once, so we want to let Vulkan know that
    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;

    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    // make a clear-color from frame number. This will flash wih a 120*pi frame
    // period
    VkClearValue clearValue;
    float flash = abs(sin(_frameNumber / 120.f));
    clearValue.color = {{0.f, 0.f, flash, 1.f}};

    // start the main renderpass
    // we will use the clear color from above, and the framebuffer of the index
    // the swapchain gave us
    VkRenderPassBeginInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.pNext = nullptr;

    rpInfo.renderPass = _renderPass;
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = _windowExtent;
    rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

    // connect clear values
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    // finalize the render pass
    vkCmdEndRenderPass(cmd);
    // finalize the command buffer (we can no longer add commands, but it can
    // now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    // prepare the submission to the queue
    // we want to wait on the _presentSemaphore, as that semaphore is signaled
    // when the swapchain is ready we will signal the _renderSemaphore, to
    // signal that rendering has finished

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags waitStage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &_presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &_renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    // submit command buffer to the queue and execute it
    // _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));

    // this will put the image we jsut rendered into the visible window
    // we want to wait on the _renderSemaphore for that,
    // as it's necessary that drawing commands have finished before the image is
    // displayed to the user
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.pSwapchains = &_swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &_renderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

    // increase the number of frames drawn
    _frameNumber++;
  }

  void App::run() {
    spdlog::default_logger()->debug("Running engine");

    SDL_Event e;
    bool bQuit = false;
    bool isFullScreen = false;

    // main loop
    while (!bQuit) {
      // Handle events on queue
      while (SDL_PollEvent(&e) != 0) {
        // close the window when user clicks the X button or alt+F4
        if (e.type == SDL_QUIT) {
          spdlog::default_logger()->debug("Received close event");
          bQuit = true;
        } else if (e.type == SDL_KEYDOWN) {
          if (e.key.keysym.sym == SDLK_F11) {
            if (!isFullScreen) {
              // if F11+SHIFT => borderless fullscreen
              if (e.key.keysym.mod & KMOD_SHIFT) {
                SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                spdlog::default_logger()->debug(
                    "Changed window to borderless fullscreen mode");

              } else {
                // fullscreen
                SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
                spdlog::default_logger()->debug(
                    "Changed window to fullscreen mode");
              }
              isFullScreen = true;
            } else {
              // windowed
              SDL_SetWindowFullscreen(_window, SDL_FALSE);

              spdlog::default_logger()->debug(
                  "Changed window to windowed mode");
              isFullScreen = false;
            }
          }
        }
      }

      draw();
    }
  }

  void App::init_logger() {
    try {
      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

      spdlog::set_default_logger(std::make_shared<spdlog::logger>(
          "AltE", spdlog::sinks_init_list{console_sink}));
      spdlog::default_logger()->set_level(spdlog::level::trace);
      spdlog::default_logger()->set_pattern(
          "\033[90m[%Y-%m-%d %T.%e]\033[m [\033[33m%n\033[m] [%^%=9l%$] %v");
    } catch (const spdlog::spdlog_ex &ex) {
      std::cerr << "Log initialization failed: " << ex.what() << std::endl;
      abort();
    }
  }

  VkBool32 App::configure_logger(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData) {
    auto severity = vkb::to_string_message_severity(messageSeverity);
    auto type = vkb::to_string_message_type(messageTypes);

    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        spdlog::default_logger()->debug("[{}] {}", type,
                                        pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        spdlog::default_logger()->error("[{}] {}", type,
                                        pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        spdlog::default_logger()->warn("[{}] {}", type,
                                       pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        spdlog::default_logger()->info("[{}] {}", type,
                                       pCallbackData->pMessage);
        break;
      default:
        spdlog::default_logger()->debug("[{}] {}", type,
                                        pCallbackData->pMessage);
        break;
    }

    return VK_FALSE;
  }

  void App::init_vulkan() {
    vkb::InstanceBuilder builder;

    // make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("Alternative-Engine")
                        .request_validation_layers(true)
                        .require_api_version(1, 1, 0)
                        .set_debug_callback(App::configure_logger)
                        .build();

    vkb::Instance vkb_inst = inst_ret.value();
    // store the instance
    _instance = vkb_inst.instance;
    // store the debug messenger
    _debug_messenger = vkb_inst.debug_messenger;

    // get the surface of the window we opened with SDL
    SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

    // use vkboostratp to select a GPU
    // We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{vkb_inst};

    vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 1)
                                             .set_surface(_surface)
                                             .select()
                                             .value();

    // create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    // get the VkDevice handle used in the rest of a Vulkan application
    _device = vkbDevice.device;
    _chosenGPU = physicalDevice.physical_device;

    // use vkboostrap to get a Graphics Queue
    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily =
        vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    spdlog::default_logger()->debug("Vulkan initialized");
  }

  void App::init_swapchain() {
    vkb::SwapchainBuilder swapchainBuilder{_chosenGPU, _device, _surface};

    vkb::Swapchain vkbSwapchain =
        swapchainBuilder
            .use_default_format_selection()
            // use vsync present mode
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(_windowExtent.width, _windowExtent.height)
            .build()
            .value();

    // store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();

    _swapchainImageFormat = vkbSwapchain.image_format;

    spdlog::default_logger()->debug("Swapchain initialized");
  }

  void App::init_commands() {
    // create a command pool for commands submtted to the graphics queue
    // we also want the pool to allow for resetting of in,dividual command
    // buffers
    VkCommandPoolCreateInfo commandPoolInfo =
        vk_abstract::command_pool_create_info(
            _graphicsQueueFamily,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(
        vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

    // allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo =
        vk_abstract::command_buffer_allocate_info(_commandPool, 1);

    VK_CHECK(
        vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));
  }

  void App::init_default_renderpass() {
    // the renderpass will use this color attachement
    VkAttachmentDescription color_attachment = {};
    // the attachment will have the format needed by the swapchain
    color_attachment.format = _swapchainImageFormat;
    // 1 sample, we won't be doing MSAA
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // we clear when this attacment is loaded
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // we keep the attachment sotre when the renderpass ends
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // we don't care about stencil
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // we don't know or care about the starting layout of the attachment
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    // after the renderpass ends, the image has to be on a layout ready for
    // display
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    // attachment number will index into the pAttachments array in the parent
    // renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    // connect the color attachment to the info
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    // connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VK_CHECK(
        vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));

    spdlog::default_logger()->debug("Renderpass initialized");
  }

  void App::init_framebuffers() {
    // create the framebuffers for the swapchain images. This will connect the
    // render-pass to the images for rendering.
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = _renderPass;
    fb_info.attachmentCount = 1;
    fb_info.width = _windowExtent.width;
    fb_info.height = _windowExtent.height;
    fb_info.layers = 1;

    // grab how many images we have in the swapchain
    const uint32_t swapchain_imagecount = _swapchainImageViews.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    // create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++) {
      fb_info.pAttachments = &_swapchainImageViews[i];
      VK_CHECK(
          vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));
    }

    spdlog::default_logger()->debug("Framebuffers initialized");
  }

  void App::init_sync_structures() {
    // create synchronization structures
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;

    // we want to create the fence with the Create Signaled flag, so we cant
    // wait on it before using it on a GPU command (for the first frame)
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

    // for the semapgores, we don't need any flags
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                               &_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr,
                               &_renderSemaphore));

    spdlog::default_logger()->debug("Sync structures initialized");
  }

  void App::init_pipeline() {
    VkShaderModule triangleFragShader;
    VkShaderModule triangleVertexShader;

    if (!shader_utils::load_shader_module(_device,
                                          "../assets/shaders/triangle.frag.spv",
                                          &triangleFragShader)) {
      spdlog::default_logger()->error(
          "Error when building the triangle fragment shader module");
    } else {
      spdlog::default_logger()->debug(
          "Triangle fragment shader module successfully loaded");
    }

    if (!shader_utils::load_shader_module(_device,
                                          "../assets/shaders/triangle.vert.spv",
                                          &triangleVertexShader)) {
      spdlog::default_logger()->error(
          "Error when building the triangle vertex shader module");
    } else {
      spdlog::default_logger()->debug(
          "Triangle vertex shader module successfully loaded");
    }
  }
} // namespace AltE