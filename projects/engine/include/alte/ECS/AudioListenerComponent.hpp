#pragma once

#include "alte/ECS/TransformComponent.hpp"
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

      void set_volume(float volume);
      float get_volume() const;

      void set_position(const glm::vec3 &position);
      glm::vec3 get_position() const;

      void set_direction(const glm::vec3 &direction);
      glm::vec3 get_direction() const;

      void set_up(const glm::vec3 &up);
      glm::vec3 get_up() const;

    private:
      static AudioListenerComponent *_instance;

      std::unique_ptr<AudioDevice> _device;
  };
} // namespace AltE