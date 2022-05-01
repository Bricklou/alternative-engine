#include "App.hpp"

void App::on_render(AltE::Rendering::Renderer *renderer) {
  init_window();

  init_dockspace();

  // End the parent window that contains the Dockspace:
  ImGui::End();
};

void App::init_window() {
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  // Set the parent window's position, size, and viewport to match that of the
  // main viewport. This is so the parent window completely covers the main
  // viewport, giving it a "full-screen" feel.
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  // Set the parent window's styles to match that of the main viewport:
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,
                      0.0f); // No corner rounding on the window
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,
                      0.0f); // No border around the window

  ImGui::Begin("DockSpace Demo", nullptr, window_flags);
  ImGui::PopStyleVar(2);
}

void App::init_dockspace() {
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  ImGui::ShowDemoWindow();
}