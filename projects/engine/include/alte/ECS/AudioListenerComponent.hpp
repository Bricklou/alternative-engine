#pragma once

#include <alte/export.hpp>

#include <memory>

namespace AltE {
  struct ALTE_API AudioDevice;

  struct ALTE_API AudioListenerComponent {
    public:
      AudioListenerComponent();
      ~AudioListenerComponent();

      AudioListenerComponent(AudioListenerComponent &&);
      AudioListenerComponent &operator=(AudioListenerComponent &&);

    private:
      static AudioListenerComponent *_instance;

      std::unique_ptr<AudioDevice> _device;
  };
} // namespace AltE