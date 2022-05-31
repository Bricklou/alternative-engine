#include <alte/ECS/AudioListenerComponent.hpp>
#include <alte/export.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "../systems/audio/AudioDevice.hpp"
#include <alte/ECS/AudioListenerComponent.hpp>

namespace AltE {
  AudioListenerComponent *AudioListenerComponent::_instance = nullptr;

  AudioListenerComponent::AudioListenerComponent() {
    if (this->_instance != nullptr) {
      throw std::runtime_error("AudioListenerComponent has already been "
                               "created. Can't created a new one.");
    }
    _instance = this;

    _device = std::make_unique<AudioDevice>();
  }

  AudioListenerComponent::AudioListenerComponent(
      AudioListenerComponent &&other) {
    if (this->_instance != nullptr) {
      throw std::runtime_error("AudioListenerComponent has already been "
                               "created. Can't created a new one.");
    }

    _device = std::move(other._device);
  }

  AudioListenerComponent &
  AudioListenerComponent::operator=(AudioListenerComponent &&other) {
    if (this->_instance != nullptr) {
      throw std::runtime_error("AudioListenerComponent has already been "
                               "created. Can't created a new one.");
    }

    _device = std::move(other._device);
    return *this;
  }

  AudioListenerComponent::~AudioListenerComponent() {}

  void AudioListenerComponent::set_volume(float volume) {
    _device->set_volume(volume);
  }

  float AudioListenerComponent::get_volume() const {
    return _device->get_volume();
  }

  void AudioListenerComponent::set_position(const glm::vec3 &position) {
    _device->set_position(position);
  }

  glm::vec3 AudioListenerComponent::get_position() const {
    return _device->get_position();
  }

  void AudioListenerComponent::set_direction(const glm::vec3 &direction) {
    _device->set_direction(direction);
  }

  glm::vec3 AudioListenerComponent::get_direction() const {
    return _device->get_direction();
  }
} // namespace AltE