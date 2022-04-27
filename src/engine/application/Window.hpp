#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <vulkan/vulkan.hpp>

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

      Window(const std::string &title, int width, int height);
      ~Window();

      Window(const Window &) = delete;
      Window &operator=(const Window &) = delete;

      void show();
      void hide();
      void minimize();

      void set_fullscreen(bool state);
      void set_borderless_fullscreen(bool state);

      [[nodiscard]] WindowState get_state() const;

      [[nodiscard]] SDL_Window *get_sdl_window() const { return _window; }

      void createWindowSurface(vk::Instance instance, VkSurfaceKHR *surface);

      [[nodiscard]] vk::Extent2D extent() const { return _windowExtent; }

      [[nodiscard]] bool was_resized() const {
        return _window_resized_flags;
      }

      void reset_resize_flag() {
        _window_resized_flags = false;
      }

      void update_extent() {
        int w,h;
        SDL_GetWindowSize(_window, &w, &h);
        _windowExtent.width = static_cast<uint32_t>(w);
        _windowExtent.height = static_cast<uint32_t>(h);
      }

    private:
      vk::Extent2D _windowExtent;
      std::string _title;

      struct SDL_Window *_window = nullptr;
      bool _window_resized_flags = false;
  };
} // namespace AltE::Application
