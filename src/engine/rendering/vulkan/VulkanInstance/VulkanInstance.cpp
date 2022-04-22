#include "VulkanInstance.hpp"

#include <SDL2/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace AltE {
  VulkanInstance::VulkanInstance(Application::Window *window)
      : _window{window} {
    if (enableValidationLayers && !check_validation_layer_support()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    // initialize the vk::ApplicationInfo struct
    vk::ApplicationInfo applicationInfo{"alternative-engine", 1, "No Engine", 1,
                                        VK_API_VERSION_1_1};

    auto extensions = this->get_requires_extensions();

    // initialize the vk::InstanceCreateInfo
    vk::InstanceCreateInfo instanceCreateInfo{
        {},                                       // flags
        &applicationInfo,                         // application info
        0,                                        // enabled layers count
        {},                                       // enabled layour names
        static_cast<uint32_t>(extensions.size()), // enabled extensions count
        extensions.data(),                        // enabled extensions names
    };

    // create an Instance
    _instance = vk::createInstance(instanceCreateInfo);

    this->configure_debug_messenger();
  }

  VulkanInstance::~VulkanInstance() {
    // destroy debug messenger if validation layers are enabled
    if (enableValidationLayers) {
      _instance.destroyDebugUtilsMessengerEXT(
          _debug_messenger, nullptr,
          vk::DispatchLoaderDynamic(_instance, vkGetInstanceProcAddr));
    }
    // destroy vulkan instance
    _instance.destroy();
  }

  std::vector<const char *> VulkanInstance::get_requires_extensions() {
    uint extensionsCount;

    // Check how many extensions SDL will give us
    if (!SDL_Vulkan_GetInstanceExtensions(_window->get_sdl_window(),
                                          &extensionsCount, nullptr))
      throw std::runtime_error("Unable to fetch vulkan extensions for SDL");

    // Let's declare a vector that will contain all our extensions
    std::vector<const char *> extensions = {};
    SDL_bool res = SDL_Vulkan_GetInstanceExtensions(
        _window->get_sdl_window(), &extensionsCount, extensions.data());

    // If it fail, just throw an error
    if (!res)
      throw std::runtime_error("Unable to fetch vulkan extensions for SDL");

    // Are validations layers enabled ?
    if constexpr (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  bool VulkanInstance::check_validation_layer_support() {
    std::vector<vk::ExtensionProperties> props =
        vk::enumerateInstanceExtensionProperties();

    auto propertyIterator = std::find_if(
        props.begin(), props.end(), [](vk::ExtensionProperties const &ep) {
          return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) ==
                 0;
        });

    if (propertyIterator == props.end()) {
      return false;
    }

    return true;
  }

  void VulkanInstance::configure_debug_messenger() {
    if (!enableValidationLayers)
      return;

    spdlog::debug("Validation layers enabled. Configuring debug messenger");

    vk::DebugUtilsMessengerCreateInfoEXT createInfoExt;
    createInfoExt.setMessageSeverity(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    createInfoExt.setMessageType(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
    createInfoExt.setPfnUserCallback(
        (PFN_vkDebugUtilsMessengerCallbackEXT)VulkanInstance::debugMessageFunc);

    _debug_messenger = _instance.createDebugUtilsMessengerEXT(
        createInfoExt, nullptr,
        vk::DispatchLoaderDynamic(_instance, vkGetInstanceProcAddr));
  }

  VkBool32 VulkanInstance::debugMessageFunc(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
    auto severity = vk::to_string(
        static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity));
    auto type = vk::to_string(
        static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageTypes));

    switch (messageSeverity) {
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        spdlog::debug("[{}] {}", type, pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        spdlog::error("[{}] {}", type, pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        spdlog::warn("[{}] {}", type, pCallbackData->pMessage);
        break;
      case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        spdlog::info("[{}] {}", type, pCallbackData->pMessage);
        break;
      default:
        spdlog::debug("[{}] {}", type, pCallbackData->pMessage);
        break;
    }

    return VK_FALSE;
  }
} // namespace AltE