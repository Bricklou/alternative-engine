#pragma once

#include <string>

namespace AltE {

  struct AudioComponent {
      std::string filePath;
      bool loop;

      AudioComponent(std::string filePath, bool loop);
      AudioComponent();
      AudioComponent(const AudioComponent &other) = default;

      bool operator==(const AudioComponent &other) const;

      bool operator!=(const AudioComponent &other) const;

      void play();
      void stop();
      void pause();

      bool isPlaying() const;

      bool isLooping() const;
      void setLooping(bool loop);

      void setVolume(float volume);
      float getVolume() const;

      void setPitch(float pitch);
      float getPitch() const;
  };
} // namespace AltE