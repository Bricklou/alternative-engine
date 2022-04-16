#include "App.h"
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

    // everthing went fine
    _isInitialized = true;
  }

  void App::cleanup() {
    if (_isInitialized) {
      SDL_DestroyWindow(_window);
    }
  }

  void App::draw() {
    // nothing yet
  }

  void App::run() {
    SDL_Event e;
    bool bQuit = false;

    // main loop
    while (!bQuit) {
      // Handle events on queue
      while (SDL_PollEvent(&e) != 0) {
        // close the window when user clicks the X button or alt+F4
        if (e.type == SDL_QUIT)
          bQuit = true;
      }
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

  void App::init_vulkan() {
    vkb::InstanceBuilder builder;

    // make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("Alternative-Engine")
                        .request_validation_layers(true)
                        .require_api_version(1, 1, 0)
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
  }
} // namespace AltE