#pragma once

#include "../../../application/Window.hpp"
#include <vulkan/vulkan.hpp>

#if !defined(NDEBUG)
#define ENABLE_VALIDATION_LAYERS true
#endif

namespace AltE::Rendering {
  class VulkanInstance {
    public:
      explicit VulkanInstance(Application::Window *window);
      ~VulkanInstance();

      vk::Instance &vk_instance() { return _instance; }

    private:
      vk::Instance _instance;       // Vulkan library instance
      Application::Window *_window; // Our application window

      std::vector<const char *> get_requires_extensions();

#if ENABLE_VALIDATION_LAYERS
      vk::DebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle

      static bool check_validation_layer_support();
      void configure_debug_messenger();
#endif
  };
} // namespace AltE::Rendering
