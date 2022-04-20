#pragma once

#include <vulkan/vulkan.h>

namespace AltE::Rendering::abstraction {
  VkCommandPoolCreateInfo
  command_pool_create_info(uint32_t queueFamilyIndex,
                           VkCommandPoolCreateFlags flags = 0);

  VkCommandBufferAllocateInfo command_buffer_allocate_info(
      VkCommandPool pool, uint32_t count = 1,
      VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

  VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass renderPass,
                                                  VkExtent2D extent);

  VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

  VkSemaphoreCreateInfo semaphore_create_info(VkSemaphoreCreateFlags flags = 0);
} // namespace AltE::Rendering::abstraction