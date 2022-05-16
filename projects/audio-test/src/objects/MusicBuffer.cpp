#include "MusicBuffer.hpp"
#include "../AL/al.hpp"
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>

void MusicBuffer::play() {
  ALsizei i;

  // clear any al errors
  alGetError();

  // Rewing the source position and clear the buffer queue
  alSourceRewind(_source);
  alSourcei(_source, AL_BUFFER, 0);

  // Fill the buffer queue
  for (i = 0; i < NUM_BUFFERS; i++) {
    // Get some data to give it to the buffer
    _membuf.clear();
    _membuf.resize(OGG_BUFFER_SIZE);
    uint32_t slen = ogg_read(_audio_data, _membuf.data(), BUFFER_SAMPLES);
    if (slen < 1)
      break;

    // slen *= _audio_data.channels * sizeof(short);
    alBufferData(_buffers[i], _format, _membuf.data(), (ALsizei)slen,
                 _audio_data.sample_rate);
  }

  if (alGetError() != AL_NO_ERROR) {
    throw std::runtime_error("Error buffering for playback");
  }

  // Now queue and start playback !
  alSourceQueueBuffers(_source, NUM_BUFFERS, _buffers);
  alSourcePlay(_source);
  if (alGetError() != AL_NO_ERROR) {
    throw std::runtime_error("Error starting playback");
  }
}

void MusicBuffer::update_buffer_stream() {
  ALint processed, state;

  // clear error
  alGetError();
  // Get relevant source info
  alGetSourcei(_source, AL_SOURCE_STATE, &state);
  alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
  if (alGetError() != AL_NO_ERROR) {
    throw std::runtime_error("Error checking music source state");
  }

  while (processed > 0) {
    ALuint bufid;
    uint32_t slen;

    alSourceUnqueueBuffers(_source, 1, &bufid);
    processed--;

    // Read the next chunk of data, refill the buffer, and queue it back on the
    // source
    _membuf.clear();
    _membuf.resize(OGG_BUFFER_SIZE);
    slen = ogg_read(_audio_data, _membuf.data(), BUFFER_SAMPLES);

    if (slen > 0) {
      alBufferData(bufid, _format, _membuf.data(), (ALsizei)slen,
                   _audio_data.sample_rate);
      alSourceQueueBuffers(_source, 1, &bufid);
    }

    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Error buffering music data");
    }
  }

  // Make sure the source hasn't underrun
  if (state != AL_PLAYING && state != AL_PAUSED) {
    ALint queued;

    // If no buffers are queued, playback is finished
    alGetSourcei(_source, AL_BUFFERS_QUEUED, &queued);
    if (queued == 0) {
      return;
    }

    alSourcePlay(_source);
    if (alGetError() != AL_NO_ERROR) {
      throw std::runtime_error("Error restarting music source");
    }
  }
}

ALint MusicBuffer::get_source() { return _source; }

MusicBuffer::MusicBuffer(const char *file_path) {
  alGenSources(1, &_source);
  alGenBuffers(NUM_BUFFERS, _buffers);

  std::size_t frame_size;

  ogg_open(file_path, _audio_data);

  // Get the sound format, and figure out the OpenAL format
  if (_audio_data.channels == 1 && _audio_data.bits_per_sample == 8) {
    _format = AL_FORMAT_MONO8;
  } else if (_audio_data.channels == 1 && _audio_data.bits_per_sample == 16) {
    _format = AL_FORMAT_MONO16;
  } else if (_audio_data.channels == 2 && _audio_data.bits_per_sample == 8) {
    _format = AL_FORMAT_STEREO8;
  } else if (_audio_data.channels == 2 && _audio_data.bits_per_sample == 16) {
    _format = AL_FORMAT_STEREO16;
  } else {
    std::string errormsg = "Unsupported sound format: ";
    errormsg += _audio_data.channels;
    errormsg += " channels, ";
    errormsg += _audio_data.bits_per_sample;
    errormsg += " bits per sample";
    throw std::runtime_error(errormsg);
  }

  ALenum err = alGetError();
  printf("%s\n", alGetString(err));

  if (!_format) {
    ogg_close(_audio_data);
    throw std::runtime_error("Unsupported sound format");
  }
}

MusicBuffer::~MusicBuffer() {
  alDeleteSources(1, &_source);

  ogg_close(_audio_data);

  alDeleteBuffers(NUM_BUFFERS, _buffers);
}

void MusicBuffer::set_looping(bool looping) {
  alSourcei(_source, AL_LOOPING, looping);
}

void MusicBuffer::set_position(const float &x, const float &y, const float &z) {
  alSource3f(_source, AL_POSITION, x, y, z);
}

void MusicBuffer::set_velocity(const float &x, const float &y, const float &z) {
  alSource3f(_source, AL_VELOCITY, x, y, z);
}

PlayingState MusicBuffer::get_state() {
  ALint state;
  alGetSourcei(_source, AL_SOURCE_STATE, &state);

  AL_CheckAndThrow();

  switch (state) {
  case AL_INITIAL:
    return PlayingState::Initial;
  case AL_PLAYING:
    return PlayingState::Playing;
  case AL_PAUSED:
    return PlayingState::Paused;
  case AL_STOPPED:
    return PlayingState::Stopped;
  default:
    return PlayingState::Unknown;
  }
}
