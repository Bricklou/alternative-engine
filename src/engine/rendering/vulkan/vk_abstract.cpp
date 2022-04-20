#include "vk_abstract.hpp"

namespace AltE::Rendering::abstraction {
  VkCommandPoolCreateInfo
  command_pool_create_info(uint32_t queueFamilyIndex,
                           VkCommandPoolCreateFlags flags /*= 0*/) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;
    return info;
  }

  VkCommandBufferAllocateInfo command_buffer_allocate_info(
      VkCommandPool pool, uint32_t count /*= 1*/,
      VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = pool;
    info.commandBufferCount = count;
    info.level = level;
    return info;
  }

  VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderPass,
                                                  VkExtent2D extent) {
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;

    info.renderPass = renderPass;
    info.attachmentCount = 1;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    return info;
  }

  VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags /*= 0*/) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;

    info.flags = flags;

    return info;
  }

  VkSemaphoreCreateInfo
  semaphore_create_info(VkSemaphoreCreateFlags flags /*= 0*/) {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
  }

} // namespace AltE::Rendering::abstraction