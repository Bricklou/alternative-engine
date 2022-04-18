#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vulkan/vulkan.h>

namespace AltE {

  class Window {
    public:
      enum class WindowState {
        Normal,
        Minimized,
        Maximized,
        Fullscreen,
        BorderlessFullscreen
      };

      Window(const std::string &title, uint32_t width, uint32_t height);
      ~Window();

      void show();
      void minimize();

      void set_fullscreen(bool state);
      void set_borderless_fullscreen(bool state);

      WindowState get_state() const;

      SDL_Window *get_sdl_window() const { return _window; }

    private:
      VkExtent2D _windowExtent;

      struct SDL_Window *_window = nullptr;
  };
} // namespace AltE
