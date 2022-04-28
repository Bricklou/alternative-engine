#include "ImGui.hpp"

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "../Renderer.hpp"

namespace AltE::Rendering {
  ImGuiRenderer::ImGuiRenderer(Renderer *renderer) : _renderer{renderer} {

    create_descriptor_pool();
    init_imgui();
  }

  ImGuiRenderer::~ImGuiRenderer() {
    // clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    _renderer->get_device()->device().destroyDescriptorPool(_descriptor_pool);
    ImGui_ImplVulkan_Shutdown();
  }

  void ImGuiRenderer::create_descriptor_pool() {
    // create the descriptor pool is very
    std::vector<vk::DescriptorPoolSize> pool_size = {
        {vk::DescriptorType::eSampler, 500},
        {vk::DescriptorType::eCombinedImageSampler, 500},
        {vk::DescriptorType::eSampledImage, 500},
        {vk::DescriptorType::eStorageImage, 500},
        {vk::DescriptorType::eUniformTexelBuffer, 500},
        {vk::DescriptorType::eStorageTexelBuffer, 500},
        {vk::DescriptorType::eUniformBuffer, 500},
        {vk::DescriptorType::eStorageTexelBuffer, 500},
        {vk::DescriptorType::eUniformBufferDynamic, 500},
        {vk::DescriptorType::eStorageBufferDynamic, 500},
        {vk::DescriptorType::eInputAttachment, 500},
    };

    _descriptor_pool = _renderer->get_device()->device().createDescriptorPool(
        vk::DescriptorPoolCreateInfo(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            pool_size.size(), pool_size));
  }

  void ImGuiRenderer::init_imgui() {
    // initialize the core structure of imgui
    ImGui::CreateContext();

    // initialite imgui sdl
    ImGui_ImplSDL2_InitForVulkan(_renderer->get_window()->get_sdl_window());

    // initialize imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _renderer->get_instance()->vk_instance();
    init_info.PhysicalDevice = _renderer->get_device()->physical_device();
    init_info.Device = _renderer->get_device()->device();
    init_info.Queue = _renderer->get_device()->graphics_queue();
    init_info.DescriptorPool = _descriptor_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info,
                          _renderer->get_swapchain()->render_pass());

    _renderer->get_command_buffers()->begin_frame();

    ImGui_ImplVulkan_CreateFontsTexture(
        _renderer->get_command_buffers()->get_current_command_buffer());

    _renderer->get_command_buffers()->end_frame();
  }

  void ImGuiRenderer::prepare() {
    //imgui new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(_renderer->get_window()->get_sdl_window());

    ImGui::NewFrame();

    // imgui commands
    ImGui::ShowDemoWindow();

    ImGui::Render();
  }

  void ImGuiRenderer::render(vk::CommandBuffer& cmd) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  }
} // namespace AltE::Rendering