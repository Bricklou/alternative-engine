#include <cstdint>
#include <memory>
#include <mutex>

#include "AudioDevice.hpp"
#include <alte/audio/AudioResource.hpp>

namespace {
  // OpenAL resources counter and its mutex
  uint32_t count = 0;
  std::recursive_mutex mutex;

  // The audio device is instanciated on demand rather than at global startup,
  // which solves a lot of weird crashed and errors.
  // It is destroyed when it is no longer needed.
  std::unique_ptr<AltE::AudioDevice> global_device;
} // namespace

namespace AltE {
  AudioResource::AudioResource() {
    // Protect from concurrent access
    std::scoped_lock lock(mutex);

    // If this is the very first resource, trigger the global device
    // initialization
    if (count == 0)
      global_device = std::make_unique<AltE::AudioDevice>();

    // Increment the resources counter
    count++;
  }

  AudioResource::~AudioResource() {
    // Project from concurrent access
    std::scoped_lock lock(mutex);

    // Decrement the resources counter
    count--;

    // If there's no more resource alive, we can destroy the device
    if (count == 0)
      global_device.reset();
  }
} // namespace AltE