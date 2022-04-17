#include "vk_abstract.hpp"

VkCommandPoolCreateInfo
vk_abstract::command_pool_create_info(uint32_t queueFamilyIndex,
                                      VkCommandPoolCreateFlags flags) {
  VkCommandPoolCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.pNext = nullptr;

  info.queueFamilyIndex = queueFamilyIndex;
  info.flags = flags;
  return info;
}

VkCommandBufferAllocateInfo
vk_abstract::command_buffer_allocate_info(VkCommandPool pool, uint32_t count,
                                          VkCommandBufferLevel level) {
  VkCommandBufferAllocateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  info.pNext = nullptr;

  info.commandPool = pool;
  info.commandBufferCount = count;
  info.level = level;
  return info;
}