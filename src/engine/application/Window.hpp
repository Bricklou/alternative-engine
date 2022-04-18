#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vulkan/vulkan.h>

namespace AltE::Application {

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

      Window(const Window &) = delete;
      Window &operator=(const Window &) = delete;

      void show();
      void minimize();

      void set_fullscreen(bool state);
      void set_borderless_fullscreen(bool state);

      WindowState get_state() const;

      SDL_Window *get_sdl_window() const { return _window; }

      void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

      VkExtent2D getExtent() const { return _windowExtent; }

    private:
      VkExtent2D _windowExtent;
      std::string _title;

      struct SDL_Window *_window = nullptr;
  };
} // namespace AltE::Application
