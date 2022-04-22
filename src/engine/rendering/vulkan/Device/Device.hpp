#pragma once

#include <vulkan/vulkan.hpp>

namespace AltE {

  class Device {
    public:
      Device();
      ~Device();

    private:
      vk::Device _device;            // Vulkan device for commands
      vk::PhysicalDevice _chosenGPU; // GPU chosen as the default device
  };

} // namespace AltE
