#include "SoundDevice.hpp"
#include "../AL/al.hpp"
#include "AL/al.h"
#include "AL/alc.h"
#include <cstdio>
#include <vector>

static SoundDevice *_instance = nullptr;

SoundDevice *SoundDevice::get() {
  SoundDevice::init();
  return _instance;
}

void SoundDevice::init() {
  if (_instance == nullptr) {
    _instance = new SoundDevice();
  }
}

void SoundDevice::get_location(float &x, float &y, float &z) {
  alGetListener3f(AL_POSITION, &x, &y, &z);
  AL_CheckAndThrow();
}

void SoundDevice::get_orientation(float &ori) {
  alGetListenerfv(AL_ORIENTATION, &ori);
  AL_CheckAndThrow();
}

float SoundDevice::get_gain() {
  float curr_gain;
  alGetListenerf(AL_GAIN, &curr_gain);
  AL_CheckAndThrow();
  return curr_gain;
}

void SoundDevice::set_attunation(int key) {
  if (key < 0xD001 || key > 0xD008) {
    throw std::runtime_error("bad attunation key");
  }
  alDistanceModel(key);
  AL_CheckAndThrow();
}

void SoundDevice::set_location(const float &x, const float &y, const float &z) {
  alListener3f(AL_POSITION, x, y, z);
  AL_CheckAndThrow();
}

void SoundDevice::set_orientation(const float &atx, const float &aty,
                                  const float &atz, const float &upx,
                                  const float &upy, const float &upz) {
  std::vector<float> ori;
  ori.push_back(atx);
  ori.push_back(aty);
  ori.push_back(atz);
  ori.push_back(upx);
  ori.push_back(upy);
  ori.push_back(upz);
  alListenerfv(AL_ORIENTATION, ori.data());
  AL_CheckAndThrow();
}

void SoundDevice::set_gain(const float &gain) {
  // clamp between 0 and 5
  float gain_clamped = std::max(0.0f, std::min(gain, 5.0f));
  alListenerf(AL_GAIN, gain_clamped);
  AL_CheckAndThrow();
}

SoundDevice::SoundDevice() {
  _device = alcOpenDevice(nullptr); // nullptr = default device
  if (!_device)
    throw std::runtime_error("failed to get sound device");

  _context = alcCreateContext(_device, nullptr);
  if (!_context)
    throw std::runtime_error("failed to set sound context");

  if (!alcMakeContextCurrent(_context)) // make context current
    throw std::runtime_error("failed to make context current");

  const ALCchar *name = nullptr;
  if (alcIsExtensionPresent(_device, "ALC_ENUMERATION_ALL_EXT"))
    name = alcGetString(_device, ALC_DEVICE_SPECIFIER);

  if (!name || alcGetError(_device) != AL_NO_ERROR)
    name = alcGetString(_device, ALC_DEVICE_SPECIFIER);

  std::printf("Openeed sound device: %s\n", name);
}

SoundDevice::~SoundDevice() {
  alcMakeContextCurrent(nullptr);
  alcDestroyContext(_context);
  alcCloseDevice(_device);
}