#pragma once

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

namespace AltE {
  class App {
    public:
      // Initialize everything in the engine
      void init();

      // Shuts down the engine
      void cleanup();

      // Draw loop
      void draw();

      // Run main loop
      void run();

    private:
      bool _isInitialized = false;
      bool _frameBuffer = 0;

      VkExtent2D _windowExtent{1280, 720};
      struct SDL_Window *_window = nullptr;
      VkInstance _instance;
      VkDebugUtilsMessengerEXT _debug_messenger;
      VkPhysicalDevice _chosenGPU;
      VkDevice _device;
      VkSurfaceKHR _surface;

      void init_logger();
      void init_vulkan();
  };
} // namespace AltE