#include "App.hpp"
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

    // everthing went fine
    _isInitialized = true;
  }

  void App::cleanup() {
    if (_isInitialized) {
      vkDestroyCommandPool(_device, _commandPool, nullptr);
      vkDestroySwapchainKHR(_device, _swapchain, nullptr);

      // destroy swapchain resources
      for (int i = 0; i < _swapchainImageViews.size(); i++) {
        vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
      }
      vkDestroyDevice(_device, nullptr);
      vkDestroySurfaceKHR(_instance, _surface, nullptr);
      vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
      vkDestroyInstance(_instance, nullptr);
      SDL_DestroyWindow(_window);
    }
    spdlog::default_logger()->debug("Engine closed");
  }

  void App::draw() {
    // nothing yet
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
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        spdlog::default_logger()->error("[{}] {}", type,
                                        pCallbackData->pMessage);
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        spdlog::default_logger()->warn("[{}] {}", type,
                                       pCallbackData->pMessage);
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        spdlog::default_logger()->info("[{}] {}", type,
                                       pCallbackData->pMessage);
      default:
        spdlog::default_logger()->debug("[{}] {}", type,
                                        pCallbackData->pMessage);
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
                        .use_default_debug_messenger()
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
} // namespace AltE