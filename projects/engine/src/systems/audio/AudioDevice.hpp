#pragma once

#include <AL/alc.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace AltE {
  class AudioDevice {
    public:
      static AudioDevice *instance();

      std::vector<std::string> get_device_list() const;
      void set_device(const std::string &device_name);

      glm::vec3 get_location() const;
      void set_location(const glm::vec3 &location);

      float get_orientation() const;
      void set_orientation(const glm::vec3 &at, const glm::vec3 &up);

      float get_gain() const;
      void set_gain(const float &gain);

      void set_attenuation(int key);

    private:
      AudioDevice();
      ~AudioDevice();

      ALCdevice *_device;
      ALCcontext *_context;

      ALCcontext *create_context(ALCdevice *device);
      void destroy_context(ALCcontext *context);
      void set_current_context(ALCcontext *context);

      void destroy_context();

      void create_device(const char *device = nullptr);
      void destroy_device();
  };
} // namespace AltE