#pragma once

#include "../../application/Window.hpp"
#include "../DeletionQueue.hpp"
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

// number of frames to overlap when rendering
#define FRAME_OVERLAP 2

namespace AltE::Rendering {
  struct FrameData {
      VkSemaphore _present_semaphore, _render_semaphore;
      VkFence _render_fence;

      VkCommandPool _command_pool;
      VkCommandBuffer _command_pool_buffer;
  };

  struct UploadContext {
      VkFence _uploadFence;
      VkCommandPool _command_pool;
      VkCommandBuffer _command_buffer;
  };

  class Device {
    public:
#ifdef NDEBUG
      const bool enableValidationLayers = false;
#else
      const bool enableValidationLayers = true;
#endif

      int _frameNumber{0};

      Device(Application::Window *window);
      ~Device();

      // Not copyable or movable
      Device(const Device &) = delete;
      Device &operator=(const Device &) = delete;
      Device(Device &&) = delete;
      Device &operator=(Device &&) = delete;

      // getter for the frame we are rendering to right now.
      FrameData &get_current_frame();
      FrameData *get_frames() { return _frames; }
      VkPhysicalDevice &get_chosen_gpu() { return _chosen_gpu; }
      VkSurfaceKHR &get_surface() { return _surface; }
      VkDevice &get_device() { return _device; }

      UploadContext &get_upload_content() { return _upload_context; }
      VkCommandBuffer &get_command_buffers() { return _command_buffer; }

    private:
      Application::Window *_window;

      void configure_vulkan();
      void create_commandpool();

      static inline VkBool32 configure_logger(
          VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
          VkDebugUtilsMessageTypeFlagsEXT messageTypes,
          const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
          void *pUserData);

      VkInstance _instance;
      VkDebugUtilsMessengerEXT _debug_messenger;
      VkSurfaceKHR _surface;
      VkDevice _device;
      VkPhysicalDevice _chosen_gpu;

      // frame storage
      FrameData _frames[FRAME_OVERLAP];
      UploadContext _upload_context;

      // the command pool for our commands
      VkCommandPool _command_pool;
      // the buffer we will record into
      VkCommandBuffer _command_buffer;

      // queue we will submit to
      VkQueue _graphics_queue;
      // family of that queue
      uint32_t _graphics_queue_family;

      DeletionQueue _deletion_queue;
      VmaAllocator _allocator; // vma lib allocator
  };
} // namespace AltE::Rendering