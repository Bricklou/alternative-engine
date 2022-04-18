#include "App.hpp"

namespace AltE {
  App::App() { init(); }

  void App::init() {
    Logger::createLogger("Alternative-Engine");

    spdlog::debug("Logger initialized");

    _window = std::make_unique<Window>("Alternative-Engine", 800, 600);

    _renderThread = std::thread(&App::runRenderLoop, this);
  }

  App::~App() { cleanup(); }

  void App::run() {
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

  void App::cleanup() {}

  void App::runRenderLoop() {
    while (!this->_should_close) {
    }
  }

} // namespace AltE