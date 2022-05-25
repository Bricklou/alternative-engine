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
} // namespace AltE