#pragma once
#include <alte/audio/SoundStream.hpp>
#include <alte/export.hpp>
#include <alte/files/AudioFile.hpp>


#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace AltE {

  struct ALTE_API AudioComponent {
      AudioComponent();
      AudioComponent(AudioComponent &&other);
      ~AudioComponent();

      AudioComponent &operator=(AudioComponent &&other);

      /**
       * @brief Proxy SoundStream::get_status()
       */
      SoundStream::Status get_status() const;

      /**
       * @brief Proxy SoundStream::play()
       */
      void play();

      /**
       * @brief Proxy SoundStream::pause()
       */
      void pause();

      /**
       * @brief Proxy SoundStream::stop()
       */
      void stop();

      /**
       * @brief Proxy SoundStream::set_volume()
       */
      void set_volume(float volume);

      /**
       * @brief Proxy SoundStream::get_volume()
       */
      float get_volume() const;

      /**
       * @brief Proxy SoundStream::set_position()
       */
      void set_position(const glm::vec3 &position);

      /**
       * @brief Proxy SoundStream::get_position()
       */
      glm::vec3 get_position() const;

      /**
       * @brief Proxy SoundStream::set_loop()
       */
      void set_loop(bool loop);

      /**
       * @brief Proxy SoundStream::get_loop()
       */
      bool get_loop() const;

      /**
       * @brief Set the sound stream source
       *
       * @param source The new source
       */
      void set_source(SoundStream &source);

      /**
       * @brief Get the sound stream source
       *
       * @return The source
       */
      const SoundStream &get_source() const;

    private:
      AudioInfo _audio_info;
      std::unique_ptr<SoundStream> _stream;
  };
} // namespace AltE