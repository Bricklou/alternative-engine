#pragma once

#include "../../../application/Window.hpp"
#include <vulkan/vulkan.hpp>

namespace AltE {
  class VulkanInstance {
    public:
      explicit VulkanInstance(Application::Window *window);
      ~VulkanInstance();

    private:
      vk::Instance _instance;                      // Vulkan library instance
      vk::DebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
      vk::SurfaceKHR _surface;                     // Vulkan window surface
      Application::Window *_window;                // Our application window

      std::vector<const char *> get_requires_extensions();
      static bool check_validation_layer_support();
      void configure_debug_messenger();

      VKAPI_ATTR static VkBool32 VKAPI_CALL debugMessageFunc(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
          VkDebugUtilsMessageTypeFlagsEXT messageTypes,
          VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
          void * /*pUserData*/);
  };
} // namespace AltE
