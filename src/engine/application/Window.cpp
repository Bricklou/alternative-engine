#include "Window.hpp"

namespace AltE {
  Window::Window(const std::string &title, uint32_t width, uint32_t height) {
    _windowExtent = {width, height};

    // We initialize SDL and create a window with it.

    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    // create blank SDL window for our application
    _window = SDL_CreateWindow(
        title.c_str(),           // window title
        SDL_WINDOWPOS_UNDEFINED, // window position x (don't care)
        SDL_WINDOWPOS_CENTERED,  // window position y (don't care)
        width,                   // window width in pixels
        height,                  // window height in pixels
        window_flags);
  }

  Window::~Window() {
    if (_window != nullptr) {
      SDL_DestroyWindow(_window);
    }
  }

  void Window::minimize() { SDL_MinimizeWindow(_window); }

  void Window::set_fullscreen(bool state) {
    if (state) {
      SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
    } else {
      SDL_SetWindowFullscreen(_window, SDL_FALSE);
    }
  }

  void Window::set_borderless_fullscreen(bool state) {
    if (state) {
      SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      SDL_SetWindowFullscreen(_window, SDL_FALSE);
    }
  }

  Window::WindowState Window::get_state() const {
    auto window_flags = SDL_GetWindowFlags(_window);

    if (window_flags & SDL_WINDOW_MINIMIZED) {
      return WindowState::Minimized;
    } else if (window_flags & SDL_WINDOW_MAXIMIZED) {
      return WindowState::Maximized;
    } else if (window_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
      return WindowState::BorderlessFullscreen;
    } else if (window_flags & SDL_WINDOW_FULLSCREEN) {
      return WindowState::Fullscreen;
    } else {
      return WindowState::Normal;
    }
  }

} // namespace AltE