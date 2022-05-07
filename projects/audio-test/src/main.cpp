
#include "AL/al.h"
#include "AL/al.hpp"
#include "objects/MusicBuffer.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

// https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
// https://indiegamedev.net/2020/02/25/the-complete-guide-to-openal-with-c-part-2-streaming-audio/
// https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/
int main() {
  try {
    ALCdevice *openALDevice = alcOpenDevice(nullptr);
    if (!openALDevice)
      return 0;

    ALCcontext *openALContext;
    if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice,
                 nullptr) ||
        !openALContext) {
      throw std::runtime_error("Error: could not create audio context");
    }

    ALCboolean contextMadeCurrent = false;
    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice,
                 openALContext) ||
        !openALContext) {

      throw std::runtime_error("Error: could not make audio context current");
    }

    if (!alCall(alDistanceModel, AL_INVERSE_DISTANCE_CLAMPED)) {
      throw std::runtime_error(
          "ERROR: Could not set Distance Model to AL_INVERSE_DISTANCE_CLAMPED");
    }

    // The audio file NEED to be in mono format to support spatial audio
    MusicBuffer music{
        "./assets-src/audio/James Roche - Away [NCS Release]__MONO.ogg"};

    std::cout << "playing music" << std::endl;

    music.play();

    alCall(alSourcef, music.get_source(), AL_PITCH, 1);
    alCall(alSourcef, music.get_source(), AL_GAIN, 1.0f);
    alCall(alSource3f, music.get_source(), AL_POSITION, 40.f, 40.f, 40.f);
    alCall(alSource3f, music.get_source(), AL_VELOCITY, 0.f, 0.f, 0.f);
    alCall(alSourcei, music.get_source(), AL_LOOPING, AL_FALSE);
    alListener3f(AL_POSITION, 0.f, 0.f, 0.f);

    uint32_t i = 0;
    float x = 0, y = 0;

    ALint state = AL_PLAYING;
    while (state == AL_PLAYING && alGetError() == AL_NO_ERROR) {
      music.update_buffer_stream();

      alGetSourcei(music.get_source(), AL_SOURCE_STATE, &state);

      i = (i + 1) % 360;

      x = cos((360 - i) * M_PI / 180) * 20;
      y = sin((360 - i) * M_PI / 180) * 20;

      alCall(alSource3f, music.get_source(), AL_POSITION, x, y, 0.0f);

      std::cout << "x = " << x << " y = " << y << '\r' << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << std::endl;

    alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, nullptr);
    alcCall(alcDestroyContext, openALDevice, openALContext);

    ALCboolean closed;
    alcCall(alcCloseDevice, closed, openALDevice, openALDevice);

  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  return 0;
}