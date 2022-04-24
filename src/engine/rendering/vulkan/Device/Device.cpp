#include "Device.hpp"
#include <set>

namespace AltE::Rendering {
  Device::Device(VulkanInstance *instance, Application::Window *window)
      : _instance{instance}, _window{window} {

    _window->createWindowSurface(_instance->vk_instance(),
                                 reinterpret_cast<VkSurfaceKHR *>(&_surface));

    this->pick_physical_device();
    this->create_logical_device();
    this->create_command_pool();
  }

  Device::~Device() {
    _device.destroyCommandPool(_command_pool, nullptr);

    _device.destroy();
    _instance->vk_instance().destroySurfaceKHR(_surface);
  }

  void Device::pick_physical_device() {
    uint32_t deviceCount = 0;
    auto result = _instance->vk_instance().enumeratePhysicalDevices(
        &deviceCount, nullptr);
    if (deviceCount == 0 || result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to find GPUs with Vulkan support !");
    }

    std::vector<vk::PhysicalDevice> devices(deviceCount);
    result = _instance->vk_instance().enumeratePhysicalDevices(&deviceCount,
                                                               devices.data());
    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to find GPUs with Vulkan support !");
    }

    for (const auto &device : devices) {
      if (is_device_suitable(device)) {
        _chosen_gpu = device;
        break;
      }
    }

    if (!_chosen_gpu) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }

    _chosen_gpu.getProperties(&_gpu_properties);
  }

  void Device::create_logical_device() {
    QueueFamilyIndices indices = find_queue_families(_chosen_gpu);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily,
                                              indices.presentFamily};

    float queuePriority = 1.f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      vk::DeviceQueueCreateInfo queueCreateInfo{
          {}, queueFamily, 1, &queuePriority};
      queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(true);

    vk::DeviceCreateInfo createInfo{};

    createInfo.setQueueCreateInfoCount(
        static_cast<uint32_t>(queueCreateInfos.size()));
    createInfo.setPQueueCreateInfos(queueCreateInfos.data());

    createInfo.setPEnabledFeatures(&deviceFeatures);
    createInfo.setEnabledExtensionCount(
        static_cast<uint32_t>(_device_extensions.size()));
    createInfo.setPpEnabledExtensionNames(_device_extensions.data());

    // might not really be necessary anymore because device specific validation
    // layers have been deprecated
#if ENABLE_VALIDATION_LAYERS
    createInfo.setEnabledLayerCount(
        static_cast<uint32_t>(_validation_layers.size()));
    createInfo.setPpEnabledLayerNames(_validation_layers.data());
#else
    createInfo.setEnabledLayerCount(0);
#endif

    if (_chosen_gpu.createDevice(&createInfo, nullptr, &_device) !=
        vk::Result::eSuccess) {
      throw std::runtime_error("failed to create logical device!");
    }

    _device.getQueue(indices.graphicsFamily, 0, &_graphics_queue);
    _device.getQueue(indices.presentFamily, 0, &_present_queue);
  }

  bool Device::is_device_suitable(const vk::PhysicalDevice &device) {
    QueueFamilyIndices indices = find_queue_families(device);

    bool extensionsSupported = check_device_extension_support(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport =
          query_swapchain_support(device);
      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures;
    device.getFeatures(&supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy;
  }

  QueueFamilyIndices Device::find_queue_families(vk::PhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

    std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
    device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
      if (queueFamily.queueCount > 0 &&
          queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
        indices.graphicsFamily = i;
        indices.graphicsFamilyHasValue = true;
      }
      vk::Bool32 presentSupport = false;
      auto result = device.getSurfaceSupportKHR(i, _surface, &presentSupport);

      if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to get surface support");
      }

      if (queueFamily.queueCount > 0 && presentSupport) {
        indices.presentFamily = i;
        indices.presentFamilyHasValue = true;
      }
      if (indices.isComplete()) {
        break;
      }

      i++;
    }

    return indices;
  }

  bool Device::check_device_extension_support(vk::PhysicalDevice device) {
    uint32_t extensionCount;
    auto result = device.enumerateDeviceExtensionProperties(
        nullptr, &extensionCount, nullptr);

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to get device extension properties");
    }

    std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
    result = device.enumerateDeviceExtensionProperties(
        nullptr, &extensionCount, availableExtensions.data());

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to get device extension properties");
    }

    std::set<std::string> requiredExtensions(_device_extensions.begin(),
                                             _device_extensions.end());

    for (const auto &extension : availableExtensions) {
      requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
  }

  SwapChainSupportDetails
  Device::query_swapchain_support(vk::PhysicalDevice device) {
    SwapChainSupportDetails details;
    auto result =
        device.getSurfaceCapabilitiesKHR(_surface, &details.capabilities);

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to get surface capabilities");
    }

    uint32_t formatCount;
    result = device.getSurfaceFormatsKHR(_surface, &formatCount, nullptr);

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to get surface format");
    }

    if (formatCount != 0) {
      details.formats.resize(formatCount);
      result = device.getSurfaceFormatsKHR(_surface, &formatCount,
                                           details.formats.data());

      if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to get surface format");
      }
    }

    uint32_t presentModeCount;
    result =
        device.getSurfacePresentModesKHR(_surface, &presentModeCount, nullptr);

    if (result != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to get surface present mode");
    }

    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      result = device.getSurfacePresentModesKHR(_surface, &presentModeCount,
                                                details.presentModes.data());

      if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to get surface present mode");
      }
    }
    return details;
  }

  SwapChainSupportDetails Device::get_swapchain_support() {
    return query_swapchain_support(_chosen_gpu);
  }

  QueueFamilyIndices Device::find_physical_queue_families() {
    return find_queue_families(_chosen_gpu);
  }

  void Device::create_command_pool() {
    QueueFamilyIndices queueFamilyIndices = find_physical_queue_families();

    vk::CommandPoolCreateInfo poolInfo{
        {vk::CommandPoolCreateFlagBits::eTransient |
         vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
        queueFamilyIndices.graphicsFamily};

    if (_device.createCommandPool(&poolInfo, nullptr, &_command_pool) !=
        vk::Result::eSuccess) {
      throw std::runtime_error("failed to create command pool!");
    }
  }
} // namespace AltE::Rendering