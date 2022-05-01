#pragma once

#include <alte/App.hpp>
#include <alte/rendering/Renderer.hpp>
#include <imgui.h>

class App {
  public:
    App();
    ~App();

    void run();

  private:
    void init_logic(AltE::App *app);
    void on_render(AltE::Rendering::Renderer *renderer);
    void on_logic(AltE::App *app);
    void on_post_render(AltE::Rendering::Renderer *renderer);

    // ImGui
    void init_window();
    void init_dockspace();

  private:
    AltE::App *_app;
};