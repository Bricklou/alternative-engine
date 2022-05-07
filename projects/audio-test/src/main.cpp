
#include "AL/al.h"
#include "AL/al.hpp"
#include "objects/MusicBuffer.hpp"
#include "objects/SoundDevice.hpp"
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
    SoundDevice::init();
    SoundDevice *device = SoundDevice::get();

    // The audio file NEED to be in mono format to support spatial audio
    MusicBuffer music{
        "./assets-src/audio/James Roche - Away [NCS Release]__MONO.ogg"};

    std::cout << "playing music" << std::endl;

    music.play();
    music.set_position(40, 40, 40);

    device->set_location(0, 0, 0);

    uint32_t i = 0;
    float x = 0, y = 0;

    PlayingState state = music.get_state();
    while (state == PlayingState::Playing) {
      music.update_buffer_stream();

      state = music.get_state();

      i = (i + 1) % 360;

      x = cos((360 - i) * M_PI / 180) * 20;
      y = sin((360 - i) * M_PI / 180) * 20;

      music.set_position(x, y, 0);

      std::cout << "x = " << x << " y = " << y << '\r' << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << std::endl;

  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }

  return 0;
}