#include "App.hpp"

namespace AltE {
  App::App() {
    _window = std::make_unique<Window>("Alternative-Engine", 800, 600);
  }

  App::~App() {}

  void App::run() {
    bool should_close = false;
    SDL_Event event;

    while (!should_close) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          should_close = true;
        } else if (event.type == SDL_KEYDOWN) {
          if (event.key.keysym.sym == SDLK_F11) {
            bool is_fullscreen = _window->get_state() ==
                                 Window::WindowState::BorderlessFullscreen;

            _window->set_borderless_fullscreen(!is_fullscreen);
          }
        }
      }
    }
  }
} // namespace AltE