#pragma once

#include <vulkan/vulkan.hpp>
#include "../vulkan/CommandBuffers/Commandbuffers.hpp"

namespace AltE::Rendering {

  class Renderer;

  class ImGuiRenderer {
    public:
      explicit ImGuiRenderer(Renderer* renderer);
      ~ImGuiRenderer();

      static void init();
      void prepare();
      void render(vk::CommandBuffer& cmd);

    private:
      Renderer* _renderer;
      vk::DescriptorPool _descriptor_pool;

      void create_descriptor_pool();
      void init_imgui();
      void configure_theme();
  };

} // namespace AltE::Rendering