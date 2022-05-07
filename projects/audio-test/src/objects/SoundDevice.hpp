#pragma once

#include <AL/alc.h>

class SoundDevice {
public:
  static SoundDevice *get();

  static void init();

  void get_location(float &x, float &y, float &z);
  void get_orientation(float &ori);
  float get_gain();

  void set_attunation(int key);
  void set_location(const float &x, const float &y, const float &z);
  void set_orientation(const float &atx, const float &aty, const float &atz,
                       const float &upx, const float &upy, const float &upz);

  void set_gain(const float &gain);

private:
  SoundDevice();
  ~SoundDevice();

  ALCdevice *_device;
  ALCcontext *_context;
};