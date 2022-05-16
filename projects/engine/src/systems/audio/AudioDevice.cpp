#include "AudioDevice.hpp"
#include "AL/al.h"
#include "AL/al.hpp"
#include "AL/alc.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace AltE {
  AudioDevice *AudioDevice::instance() {
    static AudioDevice *device = new AudioDevice();
    return device;
  }

  AudioDevice::AudioDevice() {}

  AudioDevice::~AudioDevice() { this->destroy_context(); }

  std::vector<std::string> AudioDevice::get_device_list() const {
    std::vector<std::string> device_list;

    ALboolean enumeration;
    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {
      throw std::runtime_error("Device enumeration extension not supported");
    }

    const ALCchar *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    if (devices == nullptr) {
      throw std::runtime_error("No devices available");
    }

    return device_list;
  }

  void AudioDevice::set_device(const std::string &device_name) {
    ALCcontext *old_ctx = this->_context;
    this->set_current_context(nullptr);

    this->destroy_device();
    this->create_device(device_name.c_str());

    ALCcontext *new_ctx = this->create_context(this->_device);
    this->set_current_context(new_ctx);

    this->destroy_context(old_ctx);
  }

  glm::vec3 AudioDevice::get_location() const {
    glm::vec3 location{};
    alGetListener3f(AL_POSITION, &location.x, &location.y, &location.z);

    AL_check_and_throw();

    return location;
  }

  void AudioDevice::set_location(const glm::vec3 &location) {
    alListener3f(AL_POSITION, location.x, location.y, location.z);
    AL_check_and_throw();
  }

  float AudioDevice::get_orientation() const {
    float ori;
    alGetListenerfv(AL_ORIENTATION, &ori);

    AL_check_and_throw();
    return ori;
  }

  void AudioDevice::set_orientation(const glm::vec3 &at, const glm::vec3 &up) {
    std::vector<float> ori;
    ori.push_back(at.x);
    ori.push_back(at.y);
    ori.push_back(at.z);
    ori.push_back(up.x);
    ori.push_back(up.y);
    ori.push_back(up.z);

    alGetListenerfv(AL_ORIENTATION, ori.data());

    AL_check_and_throw();
  }

  float AudioDevice::get_gain() const {
    float curr_gain;
    alGetListenerf(AL_GAIN, &curr_gain);
    AL_check_and_throw();
    return curr_gain;
  }

  void AudioDevice::set_gain(const float &gain) {
    // clamp between 0 and 5
    float gain_clamped = std::max(0.0f, std::min(gain, 5.0f));
    alListenerf(AL_GAIN, gain_clamped);
    AL_check_and_throw();
  }

  ALCcontext *AudioDevice::create_context(ALCdevice *device) {
    ALCcontext *ctx = alcCreateContext(device, nullptr);
    if (ctx == nullptr) {
      throw std::runtime_error("Failed to create context");
    }

    AL_check_and_throw();
    return ctx;
  }

  void AudioDevice::set_current_context(ALCcontext *context) {
    alcMakeContextCurrent(context);
    AL_check_and_throw();
    this->_context = context;
  }

  void AudioDevice::destroy_context(ALCcontext *context) {
    alcDestroyContext(_context);
    AL_check_and_throw();
  }

  void AudioDevice::destroy_context() {
    if (_context != nullptr) {
      this->set_current_context(nullptr);
      this->destroy_context(_context);
      AL_check_and_throw();
    }
  }

  void AudioDevice::create_device(const char *device) {
    _device = alcOpenDevice(device);
    if (_device == nullptr) {
      throw std::runtime_error("Failed to open device");
    }

    AL_check_and_throw();
  }

  void AudioDevice::destroy_device() {
    bool ret = alcCloseDevice(_device);
    if (ret == AL_FALSE) {
      throw std::runtime_error("Failed to close device");
    }
    AL_check_and_throw();
  }

  void AudioDevice::set_attenuation(int key) {
    if (key < 0xD001 || key > 0xD008) {
      throw std::runtime_error("bad attunation key");
    }
    alDistanceModel(key);
    AL_check_and_throw();
  }
} // namespace AltE