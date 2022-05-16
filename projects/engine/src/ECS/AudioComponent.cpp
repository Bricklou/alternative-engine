#include <alte/ECS/AudioComponent.hpp>

namespace AltE {

  AudioComponent::AudioComponent(std::string filePath, bool loop)
      : filePath(filePath), loop(loop) {}

  AudioComponent::AudioComponent() : filePath(""), loop(false) {}

  bool AudioComponent::operator==(const AudioComponent &other) const {
    return filePath == other.filePath && loop == other.loop;
  }

  bool AudioComponent::operator!=(const AudioComponent &other) const {
    return !(*this == other);
  }

  void AudioComponent::play() {}
  void AudioComponent::stop() {}
  void AudioComponent::pause() {}

  bool AudioComponent::isPlaying() const { return false; }

  bool AudioComponent::isLooping() const { return loop; }
  void AudioComponent::setLooping(bool loop) { this->loop = loop; }

  void AudioComponent::setVolume(float volume) {}
  float AudioComponent::getVolume() const { return 0.0f; }

  void AudioComponent::setPitch(float pitch) {}
  float AudioComponent::getPitch() const { return 0.0f; }
} // namespace AltE