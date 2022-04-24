#pragma once

#include "../../../application/Window.hpp"
#include "../VulkanInstance/VulkanInstance.hpp"
#include <vulkan/vulkan.hpp>

namespace AltE::Rendering {

  struct QueueFamilyIndices {
      uint32_t graphicsFamily;
      uint32_t presentFamily;
      bool graphicsFamilyHasValue = false;
      bool presentFamilyHasValue = false;
      [[nodiscard]] bool isComplete() const {
        return graphicsFamilyHasValue && presentFamilyHasValue;
      }
  };

  struct SwapChainSupportDetails {
      vk::SurfaceCapabilitiesKHR capabilities;
      std::vector<vk::SurfaceFormatKHR> formats;
      std::vector<vk::PresentModeKHR> presentModes;
  };

  class Device {
    public:
      explicit Device(VulkanInstance *instance, Application::Window *window);
      ~Device();

      vk::Device device() { return _device; }
      SwapChainSupportDetails get_swapchain_support();
      vk::SurfaceKHR &surface() { return _surface; }
      QueueFamilyIndices findPhysicalQueueFamilies();
      vk::Queue graphics_queue() { return _graphics_queue; }
      vk::Queue present_queue() { return _present_queue; }

    private:
      vk::Device _device;             // Vulkan device for commands
      vk::PhysicalDevice _chosen_gpu; // GPU chosen as the default device
      vk::PhysicalDeviceProperties _gpu_properties;
      vk::SurfaceKHR _surface; // Surface used to render image into the window
      VulkanInstance *_instance;
      Application::Window *_window;

      vk::Queue _graphics_queue;
      vk::Queue _present_queue;

      void pick_physical_device();
      void create_logical_device();

      // Helpers methods
      bool isDeviceSuitable(const vk::PhysicalDevice &physicalDevice);
      QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);
      bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
      SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

      const std::vector<const char *> _device_extensions = {
          VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#if ENABLE_VALIDATION_LAYERS
      const std::vector<const char *> _validation_layers = {
          "VK_LAYER_KHRONOS_validation"};
#endif
  };
} // namespace AltE::Rendering
