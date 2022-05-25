#include "AL/al.h"
#include <alte/audio/SoundSource.hpp>

#include "AL/al.hpp"

namespace AltE {

  SoundSource::SoundSource() {
    al_check(alGenSources(1, &_source));
    al_check(alSourcei(_source, AL_BUFFER, 0));
  }

  SoundSource::SoundSource(const SoundSource &copy) {
    al_check(alGenSources(1, &_source));
    al_check(alSourcei(_source, AL_BUFFER, 0));

    set_pitch(copy.get_pitch());
    set_volume(copy.get_volume());
    set_position(copy.get_position());
    set_relative_to_listener(copy.is_relative_to_listener());
    set_min_distance(copy.get_min_distance());
    set_attenuation(copy.get_attenuation());
  }

  SoundSource::~SoundSource() {
    al_check(alSourcei(_source, AL_BUFFER, 0));
    al_check(alDeleteSources(1, &_source));
  }

  void SoundSource::set_pitch(float pitch) {
    al_check(alSourcef(_source, AL_PITCH, pitch));
  }

  void SoundSource::set_volume(float volume) {
    al_check(alSourcef(_source, AL_GAIN, volume * 0.01f));
  }

  void SoundSource::set_position(const glm::vec3 &position) {
    al_check(
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z));
  }

  void SoundSource::set_relative_to_listener(bool relative) {
    al_check(alSourcei(_source, AL_SOURCE_RELATIVE, relative));
  }

  void SoundSource::set_min_distance(float min_distance) {
    al_check(alSourcef(_source, AL_REFERENCE_DISTANCE, min_distance));
  }

  void SoundSource::set_attenuation(float attenuation) {
    al_check(alSourcef(_source, AL_ROLLOFF_FACTOR, attenuation));
  }

  float SoundSource::get_pitch() const {
    float pitch;
    al_check(alGetSourcef(_source, AL_PITCH, &pitch));
    return pitch;
  }

  float SoundSource::get_volume() const {
    float volume;
    al_check(alGetSourcef(_source, AL_GAIN, &volume));
    return volume * 100.f;
  }

  glm::vec3 SoundSource::get_position() const {
    float x, y, z;
    al_check(alGetSource3f(_source, AL_POSITION, &x, &y, &z));
    return glm::vec3(x, y, z);
  }

  bool SoundSource::is_relative_to_listener() const {
    int relative;
    al_check(alGetSourcei(_source, AL_SOURCE_RELATIVE, &relative));
    return relative != 0;
  }

  float SoundSource::get_min_distance() const {
    float min_distance;
    al_check(alGetSourcef(_source, AL_REFERENCE_DISTANCE, &min_distance));
    return min_distance;
  }

  float SoundSource::get_attenuation() const {
    float attenuation;
    al_check(alGetSourcef(_source, AL_ROLLOFF_FACTOR, &attenuation));
    return attenuation;
  }

  SoundSource &SoundSource::operator=(const SoundSource &right) {
    // Leave _source untouched -- it's not necessary to destroy and recreate the
    // OpenAL sound source, hence no copy-and-swap idiom

    // Assign the sound attributes
    set_pitch(right.get_pitch());
    set_volume(right.get_volume());
    set_position(right.get_position());
    set_relative_to_listener(right.is_relative_to_listener());
    set_min_distance(right.get_min_distance());
    set_attenuation(right.get_attenuation());

    return *this;
  }

  SoundSource::Status SoundSource::get_status() const {
    ALint status;
    al_check(alGetSourcei(_source, AL_SOURCE_STATE, &status));

    switch (status) {
      case AL_INITIAL:
      case AL_STOPPED:
        return Status::Stopped;
      case AL_PLAYING:
        return Status::Playing;
      case AL_PAUSED:
        return Status::Paused;
    }

    return Status::Stopped;
  }
} // namespace AltE