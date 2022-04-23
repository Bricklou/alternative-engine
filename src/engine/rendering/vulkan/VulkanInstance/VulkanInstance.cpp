#include "VulkanInstance.hpp"

#include <SDL2/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#if ENABLE_VALIDATION_LAYERS
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR static VkBool32 VKAPI_CALL
debugMessageFunc(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                 VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                 VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
                 void * /*pUserData*/) {
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

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                           pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const *pAllocator) {
  return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
#endif

namespace AltE::Rendering {
  VulkanInstance::VulkanInstance(Application::Window *window)
      : _window{window} {
#if ENABLE_VALIDATION_LAYERS
    if (!check_validation_layer_support()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }
#endif

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

#if ENABLE_VALIDATION_LAYERS
    this->configure_debug_messenger();
#endif
  }

  VulkanInstance::~VulkanInstance() {
#if ENABLE_VALIDATION_LAYERS
    // destroy debug messenger if validation layers are enabled
    _instance.destroyDebugUtilsMessengerEXT(_debug_messenger);
#endif
    // destroy vulkan instance
    _instance.destroy();
  }

  std::vector<const char *> VulkanInstance::get_requires_extensions() {
    uint extensionsCount;

    // Check how many extensions SDL will give us
    if (!SDL_Vulkan_GetInstanceExtensions(_window->get_sdl_window(),
                                          &extensionsCount, nullptr))
      throw std::runtime_error("Unable to fetch vulkan extensions for SDL");

    spdlog::debug("Found {} vulkan instance extensions for SDL window",
                  extensionsCount);

    // Let's declare a vector that will contain all our extensions
    std::vector<const char *> extensions(extensionsCount);

    SDL_bool res = SDL_Vulkan_GetInstanceExtensions(
        _window->get_sdl_window(), &extensionsCount, extensions.data());

    // If it fail, just throw an error
    if (!res)
      throw std::runtime_error("Unable to fetch vulkan extensions for SDL");

#if ENABLE_VALIDATION_LAYERS
    // Are validations layers enabled ?
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
  }

#if ENABLE_VALIDATION_LAYERS
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
        (PFN_vkDebugUtilsMessengerCallbackEXT)debugMessageFunc);

    pfnVkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            _instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (!pfnVkCreateDebugUtilsMessengerEXT) {
      throw std::runtime_error("GetInstanceProcAddr: Unable to find "
                               "pfnVkCreateDebugUtilsMessengerEXT function.");
    }

    pfnVkDestroyDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            _instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (!pfnVkDestroyDebugUtilsMessengerEXT) {
      throw std::runtime_error("GetInstanceProcAddr: Unable to find "
                               "pfnVkDestroyDebugUtilsMessengerEXT function.");
    }

    _debug_messenger = _instance.createDebugUtilsMessengerEXT(createInfoExt);
  }
#endif
} // namespace AltE::Rendering