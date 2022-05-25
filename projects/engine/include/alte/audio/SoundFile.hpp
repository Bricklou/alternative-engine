#pragma once

#include <alte/audio/SoundStream.hpp>
#include <alte/export.hpp>
#include <alte/files/AudioFile.hpp>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <stdint.h>
#include <vector>

namespace AltE {
  class Time;
  class InputStream;

  class ALTE_API SoundFile : public SoundStream {
    public:
      /**
       * @brief Structure defining a time range using the template type
       */
      template <typename T> struct Span {
          /**
           * @brief Default constructor
           */
          Span() {}

          /**
           * @brief Initialize constructor
           *
           * @param off Initialize offset
           * @param len Initialize length
           */
          Span(T off, T len) : offset(off), length(len) {}

          T offset;
          T length;
      };

      // Define the relevant Span types
      using TimeSpan = Span<Time>;

      /**
       * @brief Default constructor
       */
      SoundFile();

      /**
       * @brief Destructor
       */
      ~SoundFile() override;

      /**
       * @brief Open a music from an asset file
       *
       * This functions doesn't start playing the music (call play() to do so)
       *
       * @warning Since the music is not loaded at once but rather streamed
       * continuously, the file must remain accessible until the Music object
       * loads a new music or is destroyed.
       *
       * @param filename Path of the music file to open
       * @return True if loading was successful, false if it failed
       */
      [[nodiscard]] bool open_from_file(const std::filesystem::path &filename);

      /**
       * @brief Get the total duration of the music
       *
       * @return Music duration
       */
      [[nodiscard]] Time get_duration() const;

    protected:
      /**
       * @brief Request a new chunk of audio samples from the stream source
       *
       * This function fills the chunk from the next samples to read from the
       * audio file.
       *
       * @param data Cunk of data to fill
       *
       * @return True to continue playback, false to stop
       */
      [[nodiscard]] bool on_get_data(Chunk &data) override;

      /**
       * @brief Change the current playing position in the stream source
       *
       * This is called by the underlying SoundStream whenever it needs us to
       * reset the seek position for a loop. We then determine whether we are
       * looping on the end-of-file, perform the seek, and return the new
       * position.
       *
       * @return The seek position after looping (or -1 if there's no loop)
       */
      void on_seek(Time time_offset) override;

      /**
       * @brief Change the current playing position in the stream source to the
       * loop offset
       *
       * This is called by the underlying SoundStream whenever it needs us to
       * reset the seek position for a loop. We then determine whether we are
       * looping on a loop point or the end-of-file, perform the seek, and
       * return the new position.
       *
       * @return The seek position after looping (or -1 if there's no loop)
       */
      uint64_t on_loop() override;

    private:
      /**
       * @brief Initialize the internal state after loading a new music
       */
      void initialize();

      /**
       * @brief Helper to convert an Time to a sample position
       */
      uint64_t time_to_samples(Time time) const;

      /**
       * @brief Helper to convert a sample position to an Time
       */
      Time samples_to_time(uint64_t samples) const;

      AudioInfo _file;
      std::vector<char> _samples;
      uint64_t _current_offset;
      std::recursive_mutex _mutex;
      Span<uint64_t> _loop_span;
  };
} // namespace AltE