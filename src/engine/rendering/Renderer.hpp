#pragma once

namespace AltE::Rendering {

  class Renderer {
    public:
      Renderer() = default;
      ~Renderer() = default;

      void render();

    private:
      void init();
      void cleanup();
  };
} // namespace AltE::Rendering