#include "glm/fwd.hpp"
#include <alte/ECS/AudioComponent.hpp>

namespace AltE {
  AudioComponent::AudioComponent() {}

  AudioComponent &AudioComponent::operator=(AudioComponent &&other) {
    if (this != &other) {
      _audio_info = std::move(other._audio_info);
      _stream = std::move(other._stream);
    }
    return *this;
  }

  AudioComponent::AudioComponent(AudioComponent &&other)
      : _audio_info(std::move(other._audio_info)),
        _stream(std::move(other._stream)) {}

  AudioComponent::~AudioComponent() {
    if (_stream) {
      _stream->stop();
      _stream.reset();
    }
  }

  SoundStream::Status AudioComponent::get_status() const {
    return _stream->get_status();
  }

  void AudioComponent::play() { _stream->play(); }

  void AudioComponent::pause() { _stream->pause(); }

  void AudioComponent::stop() { _stream->stop(); }

  void AudioComponent::set_volume(float volume) { _stream->set_volume(volume); }

  float AudioComponent::get_volume() const { return _stream->get_volume(); }

  void AudioComponent::set_position(const glm::vec3 &position) {
    _stream->set_position(position);
  }

  glm::vec3 AudioComponent::get_position() const {
    return _stream->get_position();
  }

  void AudioComponent::set_loop(bool loop) { _stream->set_loop(loop); }

  bool AudioComponent::get_loop() const { return _stream->get_loop(); }

  void AudioComponent::set_source(SoundStream &source) {
    _stream.reset(std::move(&source));
  }

  const SoundStream &AudioComponent::get_source() const { return *_stream; }
} // namespace AltE