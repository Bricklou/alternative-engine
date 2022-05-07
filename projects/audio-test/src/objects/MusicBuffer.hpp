#pragma once

#include "../ogg/ogg.hpp"
#include <AL/al.h>
#include <vector>

enum class PlayingState { Initial, Playing, Paused, Stopped, Unknown };
class MusicBuffer {
public:
  void play();
  void pause();
  void stop();

  void update_buffer_stream();

  MusicBuffer(const char *file_path);
  ~MusicBuffer();

  ALint get_source();
  void set_looping(bool looping);
  void set_position(const float &x, const float &y, const float &z);
  void set_velocity(const float &x, const float &y, const float &z);

  PlayingState get_state();

private:
  ALuint _source;
  static const int BUFFER_SAMPLES = 1024;
  static const int NUM_BUFFERS = 4;
  ALuint _buffers[NUM_BUFFERS];
  std::vector<char> _membuf;
  ALenum _format;

  AudioData _audio_data;

  MusicBuffer() = delete;
};