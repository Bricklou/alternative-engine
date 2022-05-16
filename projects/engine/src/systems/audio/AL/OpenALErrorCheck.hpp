#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <stdexcept>
#include <string>

static void ALC_check_and_throw(ALCdevice *device) {
  if (alcGetError(device) != AL_NO_ERROR) {
    std::string error = "error with alcdevice: ";
    error += alcGetString(device, alcGetError(device));
    throw std::runtime_error(error);
  }
}

static void AL_check_and_throw() {
  if (alGetError() != AL_NO_ERROR) {
    std::string error = "error with al: ";
    error += alGetString(alGetError());
    throw std::runtime_error(error);
  }
}