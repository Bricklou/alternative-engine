#pragma once

#include <alte/application/Window.hpp>
#include <alte/core/logger.hpp>
#include <alte/rendering/Renderer.hpp>
#include <atomic>
#include <functional>
#include <optional>
#include <thread>

namespace AltE {
  class App;
  typedef std::function<void(App *)> InitLogicCallback;
  typedef std::function<void(App *)> LogicCallback;

  class App {
    public:
      App() = default;
      ~App();

      void init(const std::string &app_name = "Alternative-Engine");
      void run();

      void set_init_logic(InitLogicCallback init_logic) {
        _init_logic = init_logic;
      }
      void set_logic_callback(LogicCallback logic_callback) {
        _logic_callback = logic_callback;
      }
      void set_render_callback(Rendering::RenderCallback render_callback) {
        _render_callback = render_callback;
      }
      void set_post_render_callback(
          Rendering::PostRenderCallback post_render_callback) {
        _post_render_callback = post_render_callback;
      }

    private:
      std::thread _renderThread;
      std::unique_ptr<Application::Window> _window;

      void runRenderLoop();
      void cleanup();

      void on_window_resize();

      std::atomic<bool> _should_close{false};
      std::mutex _mtx;
      SDL_Event _last_event = {};

      std::optional<InitLogicCallback> _init_logic;
      std::optional<LogicCallback> _logic_callback;
      std::optional<Rendering::RenderCallback> _render_callback;
      std::optional<Rendering::PostRenderCallback> _post_render_callback;
  };
} // namespace AltE