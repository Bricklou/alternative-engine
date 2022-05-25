#include "AudioDevice.hpp"

#include "AL/al.h"
#include "AL/al.hpp"
#include "AL/alc.h"
#include "spdlog/spdlog.h"
#include <glm/glm.hpp>
#include <optional>

namespace {
  ALCdevice *audio_device = nullptr;
  ALCcontext *audio_context = nullptr;

  float listener_volume = 100.0f;
  glm::vec3 listener_position{0.0f, 0.0f, 0.0f};
  glm::vec3 listener_direction{0.0f, 0.0f, -1.0f};
  glm::vec3 listener_up_vector{0.0f, 1.0f, 0.0f};
} // namespace

namespace AltE {
  AudioDevice::AudioDevice() {
    // Create the device
    audio_device = alcOpenDevice(nullptr);

    if (audio_device) {
      // Create the context
      audio_context = alcCreateContext(audio_device, nullptr);

      if (audio_context) {
        // Set the context as the current one (we'll only need one)
        alcMakeContextCurrent(audio_context);

        // Apply the listener properties the user might have set
        float orientation[] = {listener_direction.x, listener_direction.y,
                               listener_direction.z, listener_up_vector.x,
                               listener_up_vector.y, listener_up_vector.z};
        al_check(alListenerf(AL_GAIN, listener_volume * 0.01f));
        al_check(alListener3f(AL_POSITION, listener_position.x,
                              listener_position.y, listener_position.z));
        al_check(alListenerfv(AL_ORIENTATION, orientation));
      } else {
        SPDLOG_ERROR("Failed to create the audio context.");
      }
    } else {
      SPDLOG_ERROR("Failed to create the audio device.");
    }
  }

  AudioDevice::~AudioDevice() {
    // Destroy the context
    alcMakeContextCurrent(nullptr);
    if (audio_context)
      alcDestroyContext(audio_context);

    // Destroy the device
    if (audio_device)
      alcCloseDevice(audio_device);
  }

  bool AudioDevice::is_extension_supported(const std::string &extension) {
    // Create a temporaty audio device in case none exists yet.
    // This device will not be used in this function and merely makes sure there
    // is a valid OpenAL device for extension queries if none has been created
    // yet.
    std::optional<AudioDevice> device;
    if (!audio_device)
      device.emplace();

    if ((extension.length() > 2) && (extension.substr(0, 3) == "ALC"))
      return alcIsExtensionPresent(audio_device, extension.c_str()) != AL_FALSE;
    else
      return alIsExtensionPresent(extension.c_str()) != AL_FALSE;
  }

  int AudioDevice::get_format_from_channel_count(uint32_t channel_count) {
    // Create a temporary audio device in case none exists yet. This device will
    // not be used in this function and merely makes sure there is a valid
    // OpenAL device form format queries if none has been created yet.
    std::optional<AudioDevice> device;
    if (!audio_device)
      device.emplace();

    // Find the good format according to the number of channels
    int format = 0;
    switch (channel_count) {
      case 1:
        format = AL_FORMAT_MONO16;
        break;
      case 2:
        format = AL_FORMAT_STEREO16;
        break;
      case 4:
        format = alGetEnumValue("AL_FORMAT_QUAD16");
        break;
      case 6:
        format = alGetEnumValue("AL_FORMAT_51CHN16");
        break;
      case 7:
        format = alGetEnumValue("AL_FORMAT_61CHN16");
        break;
      case 8:
        format = alGetEnumValue("AL_FORMAT_71CHN16");
        break;
      default:
        format = 0;
    }

    // Fixes a bug on OS X
    if (format == -1)
      format = 0;

    return format;
  }

  void AudioDevice::set_volume(float volume) {
    if (audio_context)
      al_check(alListenerf(AL_GAIN, volume * 0.01f));

    listener_volume = volume;
  }

  float AudioDevice::get_volume() { return listener_volume; }

  void AudioDevice::set_position(const glm::vec3 &position) {
    if (audio_context)
      al_check(alListener3f(AL_POSITION, position.x, position.y, position.z));

    listener_position = position;
  }

  glm::vec3 AudioDevice::get_position() { return listener_position; }

  void AudioDevice::set_direction(const glm::vec3 &direction) {
    if (audio_context) {
      float orientation[] = {direction.x,          direction.y,
                             direction.z,          listener_up_vector.x,
                             listener_up_vector.y, listener_up_vector.z};
      al_check(alListenerfv(AL_ORIENTATION, orientation));
    }

    listener_direction = direction;
  }

  glm::vec3 AudioDevice::get_direction() { return listener_direction; }

  void AudioDevice::set_up_vector(const glm::vec3 &up_vector) {
    if (audio_context) {
      float orientation[] = {listener_direction.x, listener_direction.y,
                             listener_direction.z, up_vector.x,
                             up_vector.y,          up_vector.z};
      al_check(alListenerfv(AL_ORIENTATION, orientation));
    }

    listener_up_vector = up_vector;
  }

  glm::vec3 AudioDevice::get_up_vector() { return listener_up_vector; }
} // namespace AltE