#pragma once

#include <alte/audio/SoundSource.hpp>
#include <alte/export.hpp>
#include <alte/types/time.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace AltE {
  /**
   * @brief Abstract base class for streamed audio source
   */
  class ALTE_API SoundStream : public SoundSource {
    public:
      /**
       * @brief Structure defining a chunk of audio data to stream
       */
      struct Chunk {
          /// Vector containing the audio samples
          std::vector<char> samples;
      };

      /**
       * @brief Move assignment constructor
       */
      SoundStream(SoundStream &&other);

      /**
       * @brief Move assignment operator
       */
      SoundStream &operator=(SoundStream &&other) = default;

      /**
       * @brief Deconstructor
       */
      ~SoundStream() override;

      /**
       * @brief Start or resume playig the audio stream
       *
       * This function starts the stream if it was stopped, resumes it if it was
       * paused, and restarts it from the begining if it was already playing.
       * This function uses its own thread so that it doesn't block the rest of
       * the program while the stream is playing.
       *
       * @see pause, stop
       */
      void play() override;

      /**
       * @brief Pause the audio stream
       *
       * This function pauses the stream if it was playing, otherwise (stream
       * already paused or stopped) it has no effect.
       *
       * @see play, stop
       */
      void pause() override;

      /**
       * @brief Stop playing the audio stream
       *
       * This function stops the stream if it was playing or paused, and does
       * nothing if it was already stopped.
       * It also resets the playing position (unlike pause()).
       *
       * @see play, pause
       */
      void stop() override;

      /**
       * @brief Return the number of channels of the stream
       *
       * 1 channel means mono sound, 2 means stereo, etc...
       *
       * @return Number of channels
       */
      unsigned int get_channel_count() const;

      /**
       * @brief Get the stream sample rate of the stream
       *
       * The sample rate is the number of audio samples played per second. The
       * higher, the better the quality.
       *
       * @return Sample rate, in number of samples per second
       */
      unsigned int get_sample_rate() const;

      /**
       * @brief Get the current status of the stream (stopped, paused, playing)
       *
       * @return Current status
       */
      Status get_status() const override;

      /**
       * @brief Change the current playing position of the stream
       *
       * The playing position can be changed when the stream is either paused or
       * playing. Changing the playing position when the stream is stopped has
       * no effect, since playing the stream would reset its position.
       *
       * @param timeOffset New playing position, from the beginning of the
       * stream
       *
       * @see get_playing_offset
       */
      void set_playing_offset(Time &timeOffset);

      /**
       * @brief Get the current playing position of the stream
       *
       * @return Current playing position, from the beginning of the stream
       *
       * @see set_playing_offset
       */
      Time get_playing_offset() const;

      /**
       * @brief Set whether or not the steam should loop after reaching the end
       *
       * If set, the stream will restart from the beginning after reaching the
       * end and so on, until it is stopped or set_loop(false) is called.
       * The default looping state for streams is false.
       *
       * @param loop True to play in a loop, false to play only once
       * @see get_loop
       */
      void set_loop(bool loop);

      /**
       * @brief Tell whether or not the stream is in loop mode
       *
       * @return True if the stream is played in loop, false otherwise
       * @see set_loop
       */
      bool get_loop() const;

    protected:
      enum {
        // "Invalid" end_seekds value, telling us to continue uninterrupted
        NoLoop = -1
      };

      /**
       * @brief Default constructor
       *
       * This constructor is only meant to be called by derived classes.
       */
      SoundStream();

      /**
       * @brief Define the audio stream parameters
       *
       * This function must be called by derived classes as soon as they know
       * the audio settings of the stream to play.
       * Any attempt to manipulate the stream (play(), ...) before calling this
       * function will fail.
       * It can be called multiple times if the settings of the audio stream
       * change, but only when the stream is stopped.
       *
       * @param channel_count Number of channels of the stream
       * @param sample_rate Sample rate, in number of samples per second
       */
      void initialize(unsigned int channel_count, unsigned int sample_rate);

      /**
       * @brief  Request a new chunk of audio samples from the stream source
       *
       * This function must be overridden by derived classes to provide the
       * audio samples to play. It is called continuously by the streaming loop,
       * in a separate thread. The source can choose to stop the streaming loop
       * at any time, by returning false to the caller. If you return true (i.e.
       * continue streaming) it is important that the returned array of samples
       * is not empty; this would stop the stream due to an internal limitation.
       *
       * @param data Chunk of data to fill
       *
       * @return True to continue playback, false to stop
       */
      [[nodiscard]] virtual bool on_get_data(Chunk &data) = 0;

      /**
       * @brief Change the current playing position in the stream source
       *
       * This function must be overridden by derived classes to
       * allow random seeking into the stream source.
       *
       *
       * @param time_offset New playing position, relative to the beginning of
       * the stream
       */
      virtual void on_seek(Time time_offset) = 0;

      /**
       * @brief Change the current playing position in the stream source to the
       * beginning of the loop
       *
       * This function can be overridden by derived classes to
       * allow implementation of custom loop points. Otherwise,
       * it just calls on_seek(Time::Zero) and returns 0.
       *
       * @return The seek position after looping (or -1 if there's no loop)
       *
       */
      virtual uint64_t on_loop();

      /**
       * @brief Set the processing interval
       *
       * The processing interval controls the period at which the  audio buffers
       * are filled by calls to onGetData. A smaller interval may be useful for
       * low-latency streams. Note that the given period is only a hint and the
       * actual period may vary. The default processing interval is 10 ms.
       *
       * @param interval Processing interval in seconds
       */
      void set_processing_interval(Time interval);

    private:
      /**
       * @brief Function called as the entry point of the steam
       *
       * This function starts the streaming loop, and retuns only when the sound
       * is stopped.
       */
      void stream_data();

      /**
       * @brief Fill a new buffer with audio samples, and append it to the
       * playing queue
       *
       * This function is called as soon as a buffer has been fully consumed; it
       * fulls it again and inserts it back into the playing queue.
       *
       * @param buffer_num Number of the buffer to fill (in [0, BufferCount))
       * @param immediate_loop Treat empty buffers as spent, and act on loops
       * immediately
       * @return True if the stream source has requested to stop, false
       * otherwise
       */
      [[nodiscard]] bool fill_and_push_buffer(unsigned int buffer_num,
                                              bool immediate_loop = false);

      /**
       * @brief Fill the audio buffers and put them all into the playing queue
       *
       * This function is called when playing starts and the playing queue is
       * empty.
       *
       * @return True if the derived class has requested to stop, false
       * otherwise
       */
      [[nodiscard]] bool fill_queue();

      /**
       * @brief Clear all the audio buffers and empty the playing queue
       *
       * This function is called when the stream is stopped.
       */
      void clear_queue();

      /**
       * @brief Launch a new stream thread running `stream_data`
       *
       * This function is called when the stream is played or when the playing
       * offset is changed.
       *
       * @param thread_start_state Thread start state
       */
      void launch_streaming_thread(Status thread_start_state);

      /**
       * @brief Stop the stream thread and wait for `_thread`to join
       *
       * This function is called when the playback is stopped or when the sound
       * stream is destroyed.
       */
      void await_streaming_thread();

      enum {
        /// Number of audio buffers used by the streaming loop
        BufferCount = 3,
        /// Number of retries (excluding initial try) for on_get_data()
        BufferRetries = 2
      };

      /// Thread running the background tasks
      std::thread _thread;
      // Thread mutex
      mutable std::recursive_mutex _thread_mutex;
      /// State the thread starts in (Playing, Paused, Stopped)
      Status _thread_start_state;
      /// Streaming state (true = playing, false = stopped)
      bool _is_streaming;
      /// Sound buffers used to store temporary audio data
      std::array<unsigned int, BufferCount> _buffers;
      /// Number of channels (1 = mono, 2 = stereo, ...)
      unsigned int _channel_count;
      /// Frequency (samples / seconds)
      unsigned int _sample_rate;
      /// Format of the internal sound buffers
      int32_t _format;
      /// Loop flag (true to loop, false to play once)
      bool _loop;
      /// Number of samples processed since beginning of the stream
      uint64_t _samples_processed;
      /// If buffer is an "end buffer", holds next seek position, else NoLoop.
      /// For play offset calculation
      std::array<uint64_t, BufferCount> _buffer_seeks;
      /// Interval for checking and filling the internal sound buffers
      Time _processing_interval;
  };
} // namespace AltE