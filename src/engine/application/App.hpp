#pragma once

#include "../rendering/DeletionQueue.hpp"
#include "../rendering/vk_abstract.hpp"
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
      int _frameNumber = 0;
      int _selectedShader = 0;

      DeletionQueue _mainDeletionQueue;

      VkExtent2D _windowExtent{1280, 720};
      struct SDL_Window *_window = nullptr;
      VkInstance _instance;
      VkDebugUtilsMessengerEXT _debug_messenger;
      VkPhysicalDevice _chosenGPU;
      VkDevice _device;
      VkSurfaceKHR _surface;

      VkSwapchainKHR _swapchain;

      // image format expected by the windowing system
      VkFormat _swapchainImageFormat;

      // array of images from the swapchain
      std::vector<VkImage> _swapchainImages;

      // array of image-views from the swapchain
      std::vector<VkImageView> _swapchainImageViews;

      // queue we will submit to
      VkQueue _graphicsQueue;
      // family of that queue
      uint32_t _graphicsQueueFamily;

      // the command pool for our commands
      VkCommandPool _commandPool;
      // the buffer we will record into
      VkCommandBuffer _mainCommandBuffer;

      VkRenderPass _renderPass;
      std::vector<VkFramebuffer> _framebuffers;

      VkSemaphore _presentSemaphore, _renderSemaphore;
      VkFence _renderFence;

      VkPipelineLayout _trianglePipelineLayout;
      VkPipeline _trianglePipeline;
      VkPipeline _redTrianglePipeline;

      void init_logger();
      static inline VKAPI_ATTR VkBool32 configure_logger(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
          VkDebugUtilsMessageTypeFlagsEXT messageTypes,
          const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
          void *pUserData);
      void init_vulkan();
      void init_swapchain();
      void init_commands();
      void init_default_renderpass();
      void init_framebuffers();
      void init_sync_structures();
      void init_pipeline();
  };
} // namespace AltE