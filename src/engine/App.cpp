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

    SDL_Delay(200);

    _renderThread = std::thread(&App::runRenderLoop, this);

    pthread_setname_np(pthread_self(), "AltE main thread");
    pthread_setname_np(_renderThread.native_handle(), "AltE render thread");

    SDL_Delay(200);

    _window->show();
  }

  App::~App() { cleanup(); }

  void App::run() {
    using namespace Application;

    SDL_Event event;
    int frameTime;
    uint32_t frameStart;
    const int frameDelay = 1000 / 60;

    while (!this->_should_close) {
      // Stores the number of ticks at the start of the loop
      frameStart = SDL_GetTicks();

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
                this->_mtx.lock();
                _window->update_extent();
                this->_mtx.unlock();
              }
            }
        }
      }

      // This measures how long this iteration of the loop took
      frameTime = SDL_GetTicks() - frameStart;

      // This keeps us from displaying more frames than 60/Second
      if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
      }
    }

    _renderThread.join();
  }

  void App::cleanup() { spdlog::debug("Shutting down..."); }

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