#include <alte/rendering/ImGui/ImGui.hpp>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan.hpp>

#include <alte/rendering/Renderer.hpp>

namespace AltE::Rendering {
  ImGuiRenderer::ImGuiRenderer(Renderer *renderer) : _renderer{renderer} {

    create_descriptor_pool();
    init_imgui();
    configure_theme();
  }

  ImGuiRenderer::~ImGuiRenderer() {
    // clear font textures from cpu data
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    _renderer->get_device()->device().destroyDescriptorPool(_descriptor_pool);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
  }

  void ImGuiRenderer::init() {
    // initialize the core structure of imgui
    ImGui::CreateContext();
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
    // imgui new frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(_renderer->get_window()->get_sdl_window());

    ImGui::NewFrame();
  }

  void ImGuiRenderer::render(vk::CommandBuffer &cmd) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  }

  void ImGuiRenderer::configure_theme() {
    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameRounding = 4.f;
    style.GrabRounding = 4.f;

    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] =
        ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] =
        ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] =
        ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] =
        ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] =
        ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] =
        ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] =
        ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] =
        ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] =
        ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] =
        ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] =
        ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  }
} // namespace AltE::Rendering