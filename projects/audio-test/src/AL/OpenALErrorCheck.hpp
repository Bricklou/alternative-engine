#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <stdexcept>
#include <string>

static void ALC_CheckAndThrow(ALCdevice *device) {
  if (alcGetError(device) != AL_NO_ERROR) {
    std::string error = "error with alcdevice: ";
    error += alcGetString(device, alcGetError(device));
    throw std::runtime_error(error);
  }
}

static void AL_CheckAndThrow() {
  if (alGetError() != AL_NO_ERROR) {
    std::string error = "error with al: ";
    error += alGetString(alGetError());
    throw std::runtime_error(error);
  }
}