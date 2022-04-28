#include "App.hpp"
#include "rendering/Renderer.hpp"
#include <chrono>
#include <pthread.h>

using namespace std::chrono_literals;

namespace AltE {
  App::App() { init(); }

  void App::init() {
    Core::Logger::createLogger("Alternative-Engine");

    spdlog::debug("Logger initialized");

    _window =
        std::make_unique<Application::Window>("Alternative-Engine", 800, 600);

    std::this_thread::sleep_for(200ms);

    _renderThread = std::thread(&App::runRenderLoop, this);

    pthread_setname_np(pthread_self(), "AltE-main");
    pthread_setname_np(_renderThread.native_handle(), "AltE-render");

    std::this_thread::sleep_for(200ms);

    _window->show();
  }

  App::~App() { cleanup(); }

  void App::run() {
    using namespace Application;

    SDL_Event event;
    uint64_t startTime;
    int64_t frameTime;

    while (!this->_should_close) {
      startTime = SDL_GetTicks64();

      std::chrono::system_clock::now();
      bool window_resized = false;
      while (SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_QUIT:
            {
              _should_close = true;
            }
          case SDL_KEYDOWN:
            {
              if (event.key.keysym.sym == SDLK_F11) {
                bool is_fullscreen = _window->get_state() ==
                                     Window::WindowState::BorderlessFullscreen;

                _window->set_borderless_fullscreen(!is_fullscreen);
              }
            }
          case SDL_WINDOWEVENT:
            {
              if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                window_resized = true;
              }
            }
          default:
            break;
        }
      }

      if (window_resized) {
        on_window_resize();
      }

      frameTime = SDL_GetTicks64() - startTime;

      if (50 > frameTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50-frameTime));
      }
    }

    _renderThread.join();
  }

  void App::cleanup() { spdlog::debug("Shutting down..."); }

  void App::on_window_resize() {
    this->_mtx.lock();
    _window->update_extent();
    this->_mtx.unlock();
  }

  void App::runRenderLoop() {

    std::unique_ptr<Rendering::Renderer> renderer =
        std::make_unique<Rendering::Renderer>(_window.get());

    std::this_thread::sleep_for(5ms);

    while (!this->_should_close) {
      this->_mtx.lock();

      renderer->render();

      this->_mtx.unlock();
    }
  }
} // namespace AltE