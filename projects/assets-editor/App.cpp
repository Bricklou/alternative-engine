#include "App.hpp"
#include "alte/App.hpp"
#include "spdlog/spdlog.h"
#include <functional>
#include <imgui.h>
#include <imgui_impl_sdl.h>

App::App() {
  _app = new AltE::App();

  _app->set_init_logic(
      std::bind(&App::init_logic, this, std::placeholders::_1));
  _app->set_logic_callback(
      std::bind(&App::on_logic, this, std::placeholders::_1));

  _app->set_render_callback(
      std::bind(&App::on_render, this, std::placeholders::_1));
  _app->set_post_render_callback(
      std::bind(&App::on_post_render, this, std::placeholders::_1));

  _app->init("Assets Editor");
}

App::~App() { delete _app; }

void App::run() { _app->run(); }

void App::init_logic(AltE::App *app) {
  auto &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void App::on_logic(AltE::App *app) {}

void App::on_post_render(AltE::Rendering::Renderer *renderer) {}