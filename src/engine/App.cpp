#include "App.hpp"
#include "rendering/Renderer.hpp"

namespace AltE {
  App::App() { init(); }

  void App::init() {
    Core::Logger::createLogger("Alternative-Engine");

    spdlog::debug("Logger initialized");

    _window =
        std::make_unique<Application::Window>("Alternative-Engine", 800, 600);

    _renderThread = std::thread(&App::runRenderLoop, this);
  }

  App::~App() { cleanup(); }

  void App::run() {
    using namespace Application;

    SDL_Event event;

    while (!this->_should_close) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          this->_should_close = true;
        } else if (event.type == SDL_KEYDOWN) {
          if (event.key.keysym.sym == SDLK_F11) {
            bool is_fullscreen = _window->get_state() ==
                                 Window::WindowState::BorderlessFullscreen;

            _window->set_borderless_fullscreen(!is_fullscreen);
          }
        }
      }
    }

    _renderThread.join();
  }

  void App::cleanup() { spdlog::debug("Shutting down..."); }

  void App::runRenderLoop() {
    std::unique_ptr<Rendering::Renderer> renderer =
        std::make_unique<Rendering::Renderer>(_window.get());

    while (!this->_should_close) {
      renderer->render();
    }
  }
} // namespace AltE