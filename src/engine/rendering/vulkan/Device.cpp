#include "Device.hpp"
#include "vk_abstract.hpp"
#include <spdlog/spdlog.h>

#define VK_CHECK(x)                                                            \
  {                                                                            \
    VkResult err = x;                                                          \
    if (err != VK_SUCCESS) {                                                   \
      spdlog::error("Detected Vulkan error: {}", err);                         \
      abort();                                                                 \
    }                                                                          \
  }

namespace AltE::Rendering {
  Device::Device(Application::Window *window) : _window{window} {
    configure_vulkan();
    create_commandpool();
  }

  Device::~Device() {
    vkDeviceWaitIdle(_device);

    _deletion_queue.flush();

    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyDevice(_device, nullptr);
    vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
    vkDestroyInstance(_instance, nullptr);
  }

  VkBool32 Device::configure_logger(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageTypes,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData) {
    auto severity = vkb::to_string_message_severity(messageSeverity);
    auto type = vkb::to_string_message_type(messageTypes);

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

  void Device::configure_vulkan() {
    vkb::InstanceBuilder builder;

    // make the Vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("alternative-engine")
                        .request_validation_layers(enableValidationLayers)
                        .require_api_version(1, 1, 0)
                        .set_debug_callback(Device::configure_logger)
                        .build();

    vkb::Instance vkb_inst = inst_ret.value();

    // store the instance
    _instance = vkb_inst.instance;
    // store the debug messenger
    _debug_messenger = vkb_inst.debug_messenger;

    // get the surface of the window we opened with SDL
    _window->createWindowSurface(_instance, &_surface);

    // use vkboostrap to select a GPU
    // we want a GPU that can write to the SDL surface and supports Vulkan 1.1
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
    _chosen_gpu = physicalDevice.physical_device;

    // use vkbootstrap to get a Graphics queue
    _graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphics_queue_family =
        vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    // initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosen_gpu;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;

    vmaCreateAllocator(&allocatorInfo, &_allocator);

    _deletion_queue.push_function(
        [this] { vmaDestroyAllocator(this->_allocator); });

    spdlog::debug("Vulkan initialized");
  }

  void Device::create_commandpool() {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    VkCommandPoolCreateInfo commandPoolInfo =
        abstraction::command_pool_create_info(
            _graphics_queue_family,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (int i = 0; i < FRAME_OVERLAP; i++) {
      VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr,
                                   &_frames[i]._command_pool));

      // allocate the default command buffer that we will use for rendering
      VkCommandBufferAllocateInfo cmdAllocInfo =
          abstraction::command_buffer_allocate_info(_frames[i]._command_pool,
                                                    1);

      VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo,
                                        &_frames[i]._command_pool_buffer));

      _deletion_queue.push_function([this, i] {
        vkDestroyCommandPool(this->_device, this->_frames[i]._command_pool,
                             nullptr);
      });
    }

    VkCommandPoolCreateInfo uploadCommandPoolInfo =
        abstraction::command_pool_create_info(_graphics_queue_family);
    // create pool for upload context
    VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr,
                                 &_upload_context._command_pool));

    _deletion_queue.push_function([this]() {
      vkDestroyCommandPool(this->_device, this->_upload_context._command_pool,
                           nullptr);
    });

    // allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo =
        abstraction::command_buffer_allocate_info(_upload_context._command_pool,
                                                  1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo,
                                      &_upload_context._command_buffer));

    spdlog::debug("Command pool initialized");
  }

  FrameData &Device::get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
  }

} // namespace AltE::Rendering