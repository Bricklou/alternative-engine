#include "AL/al.hpp"
#include "AudioDevice.hpp"
#include <alte/audio/SoundStream.hpp>
#include <alte/types/sleep.hpp>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>

namespace AltE {
  SoundStream::SoundStream()
      : _thread{}, _thread_mutex{}, _thread_start_state{Status::Stopped},
        _is_streaming{false}, _buffers{}, _channel_count{0},
        _sample_rate{0}, _format{0}, _loop{false}, _samples_processed{0},
        _buffer_seeks{}, _processing_interval{milliseconds(100)} {}

  SoundStream::~SoundStream() {
    // Stop the sound if it was playing

    // Wait for the thread to join
    await_streaming_thread();
  }

  void SoundStream::initialize(unsigned int channel_count,
                               unsigned int sample_rate) {
    _channel_count = channel_count;
    _sample_rate = sample_rate;
    _samples_processed = 0;

    {
      std::scoped_lock lock{_thread_mutex};
      _is_streaming = false;
    }

    // Deduce the format from the number of channels
    _format = AudioDevice::get_format_from_channel_count(channel_count);

    // Check if the format is valid
    if (_format == 0) {
      _channel_count = 0;
      _sample_rate = 0;
      SPDLOG_ERROR("Unsupported number of channels: {}", channel_count);
    }
  }

  void SoundStream::play() {
    // Check if the sound parameters has been set
    if (_format == 0) {
      SPDLOG_ERROR("Failed to play audio stream: sound parameters have not "
                   "been initialized (call initialize() first)");
      return;
    }

    bool is_streaming = false;
    Status thread_start_state = Stopped;

    {
      std::scoped_lock lock{_thread_mutex};
      is_streaming = _is_streaming;
      thread_start_state = _thread_start_state;
    }

    if (is_streaming && (thread_start_state == Paused)) {
      // If the sound is pause, resume it
      std::scoped_lock lock(_thread_mutex);
      _thread_start_state = Playing;
      al_check(alSourcePlay(_source));
      return;
    } else if (is_streaming && (thread_start_state == Playing)) {
      // If the sound is already playing, stop it and continue as if it was
      // stopped
      stop();
    } else if (!is_streaming && _thread.joinable()) {
      // If the streaming thread reached its end, let it join so it can be
      // restarted Also reset the playing offset at the beginning.
      stop();
    }

    // Start updating the stream in a separate thread to avoid blocking the
    // application
    launch_streaming_thread(Playing);
  }

  void SoundStream::pause() {
    // Handle pause() being called before the thread has started
    {
      std::scoped_lock lock{_thread_mutex};

      if (!_is_streaming)
        return;

      _thread_start_state = Paused;
    }

    al_check(alSourcePause(_source));
  }

  void SoundStream::stop() {
    // Wait for the thread to join
    await_streaming_thread();

    // Move too the beginning
    on_seek(Time::Zero);
  }

  unsigned int SoundStream::get_channel_count() const { return _channel_count; }

  unsigned int SoundStream::get_sample_rate() const { return _sample_rate; }

  SoundStream::Status SoundStream::get_status() const {
    Status status = SoundSource::get_status();

    // To compensate for the lag between play() and alSourcePlay()
    if (status == Stopped) {
      std::scoped_lock lock{_thread_mutex};

      if (_is_streaming)
        status = _thread_start_state;
    }
    return status;
  }

  void SoundStream::set_playing_offset(Time &time_offset) {
    // Get old playing status
    Status old_status = get_status();

    // Stop the stream
    stop();

    // Let the derived class update the current position
    on_seek(time_offset);

    // Restart the stream
    _samples_processed =
        static_cast<uint64_t>(time_offset.as_seconds() *
                              static_cast<float>(_sample_rate)) *
        _channel_count;

    if (old_status == Stopped) {
      return;
    }

    launch_streaming_thread(old_status);
  }

  Time SoundStream::get_playing_offset() const {
    if (_sample_rate && _channel_count) {
      ALfloat secs = 0.f;
      al_check(alGetSourcef(_source, AL_SEC_OFFSET, &secs));

      return seconds(secs + static_cast<float>(_samples_processed) /
                                static_cast<float>(_sample_rate) /
                                static_cast<float>(_channel_count));

    } else {
      return Time::Zero;
    }
  }

  void SoundStream::set_loop(bool loop) { _loop = loop; }

  bool SoundStream::get_loop() const { return _loop; }

  uint64_t SoundStream::on_loop() {
    on_seek(Time::Zero);
    return 0;
  }

  void SoundStream::set_processing_interval(Time interval) {
    _processing_interval = interval;
  }

  void SoundStream::stream_data() {
    bool requested_stop = false;

    {
      std::scoped_lock lock{_thread_mutex};

      // Check if the thread was launched Stopped
      if (_thread_start_state == Stopped) {
        _is_streaming = false;
        return;
      }
    }

    // Create the buffers
    al_check(alGenBuffers(BufferCount, _buffers.data()));
    for (uint64_t &buffer_seek : _buffer_seeks) {
      buffer_seek = NoLoop;
    }

    // Fill the queue
    requested_stop = fill_queue();

    // Play the sound
    al_check(alSourcePlay(_source));

    {
      std::scoped_lock lock{_thread_mutex};

      // Check if the thread was launcher Paused
      if (_thread_start_state == Paused)
        al_check(alSourcePause(_source));
    }

    while (true) {
      {
        std::scoped_lock lock{_thread_mutex};
        if (!_is_streaming)
          return;
      }

      // The stream has been interrupted !
      if (SoundSource::get_status() == Stopped) {
        if (!requested_stop) {
          // Just continue
          al_check(alSourcePlay(_source));
        } else {
          // End streaming
          std::scoped_lock lock{_thread_mutex};
          _is_streaming = false;
        }
      }

      // Get the nummber of buffers that have been processed (i.e. ready for
      // reuse)
      ALint nb_processed = 0;
      al_check(alGetSourcei(_source, AL_BUFFERS_PROCESSED, &nb_processed));

      while (nb_processed--) {
        // Pop the first unused buffer from the queue
        ALuint buffer;
        al_check(alSourceUnqueueBuffers(_source, 1, &buffer));

        // Find its number
        unsigned int buffer_num = 0;
        for (unsigned int i = 0; i < BufferCount; i++) {
          if (_buffers[i] == buffer) {
            buffer_num = i;
            break;
          }
        }

        // Retrieve its size and add it to the samples count
        if (_buffer_seeks[buffer_num] != NoLoop) {
          // This was the last buffer before EOF or Loop End: reset the sample
          // count
          _samples_processed = static_cast<uint64_t>(_buffer_seeks[buffer_num]);
          _buffer_seeks[buffer_num] = NoLoop;
        } else {
          ALint size, bits;
          al_check(alGetBufferi(buffer, AL_SIZE, &size));
          al_check(alGetBufferi(buffer, AL_BITS, &bits));

          // Bits can be 0 if the format or parameters are corrupt, avoid
          // division by zero
          if (bits == 0) {
            SPDLOG_ERROR(
                "Bits in sound stream are 0: make sure that the audio format "
                "is not currupt and initialize() has been called correctly");

            // Abord streaming (exit main loop)
            std::scoped_lock lock{_thread_mutex};
            _is_streaming = false;
            requested_stop = true;
            break;
          } else {
            _samples_processed += static_cast<uint64_t>(size / (bits / 8));
          }
        }

        // Fill it and push it back into the playing queue
        if (!requested_stop) {
          if (fill_and_push_buffer(buffer_num))
            requested_stop = true;
        }
      }

      // Check if any error has occured
      if (al_get_last_error() != AL_NO_ERROR) {
        // Abord streaming (exit main loop)
        std::scoped_lock lock{_thread_mutex};
        _is_streaming = false;
        break;
      }

      // Leave some time for the other threads if the stream is still playing
      if (SoundSource::get_status() != Stopped) {
        sleep(_processing_interval);
      }
    }

    // Stop the playback
    al_check(alSourceStop(_source));

    // Dequeue any buffer left in the queue
    clear_queue();

    _samples_processed = 0;

    // Delete the buffers
    al_check(alSourcei(_source, AL_BUFFER, 0));
    al_check(alDeleteBuffers(BufferCount, _buffers.data()));
  }

  bool SoundStream::fill_and_push_buffer(unsigned int buffer_num,
                                         bool immediate_loop) {
    bool requested_stop = false;

    // Acquire audio data, also address EOF cases if they occur
    Chunk data{{}};

    for (uint32_t retry_count = 0;
         !on_get_data(data) && (retry_count < BufferRetries); ++retry_count) {
      // Check if the stream must loop or stop
      if (!_loop) {
        // Not looping: Mark this buffer as ending with 0 and request stop
        if (!data.samples.empty())
          _buffer_seeks[buffer_num] = 0;
        requested_stop = true;
        break;
      }

      // Return to the beginning or loop-start of the stream source using
      // onLoop(), and store the result in the buffer seek array This marks
      // the buffer as the "last" one (so that we know where to reset the
      // playing position)
      _buffer_seeks[buffer_num] = on_loop();

      // If we got data, break and process it, else try to fill the buffer
      // once again
      if (!data.samples.empty())
        break;

      // If immediateLoop is specified, we have to immediately adjust the
      // sample count
      if (immediate_loop && (_buffer_seeks[buffer_num] != NoLoop)) {
        // We just tried to begin preloading at EOF or Loop End: reset the
        // sample count
        _samples_processed = static_cast<uint64_t>(_buffer_seeks[buffer_num]);
        _buffer_seeks[buffer_num] = NoLoop;
      }

      // We're a looping sound that got no data, so we retry onGetData()
    }

    // Fill the buffer if some data was returned
    if (!data.samples.empty()) {
      unsigned int buffer = _buffers[buffer_num];

      // Fill the buffer
      auto size = static_cast<ALsizei>(data.samples.size() * sizeof(char));
      al_check(alBufferData(buffer, _format, data.samples.data(), size,
                            static_cast<ALsizei>(_sample_rate)));

      // Push it into the sound queue
      al_check(alSourceQueueBuffers(_source, 1, &buffer));
    } else {
      // If we get here, we most likely ran out of retries
      requested_stop = true;
    }

    return requested_stop;
  }

  bool SoundStream::fill_queue() {
    // Fill and enqueue all the available buffers
    bool requested_stop = false;
    for (unsigned int i = 0; (i < BufferCount) && !requested_stop; ++i) {
      // Since no sound has been loaded yet, we can't schedule loop seeks
      // preemptively, So if we start on EOF or Loop End, we let
      // fill_and_push_buffer() adjust the sample count
      if (fill_and_push_buffer(i, (i == 0)))
        requested_stop = true;
    }

    return requested_stop;
  }

  void SoundStream::clear_queue() {
    // Get the number of buffers still in the queue
    ALint nbQueued;
    al_check(alGetSourcei(_source, AL_BUFFERS_QUEUED, &nbQueued));

    // Dequeue them all
    ALuint buffer;
    for (ALint i = 0; i < nbQueued; ++i)
      al_check(alSourceUnqueueBuffers(_source, 1, &buffer));
  }

  void SoundStream::launch_streaming_thread(Status threadStartState) {
    {
      std::scoped_lock lock(_thread_mutex);
      _is_streaming = true;
      _thread_start_state = threadStartState;
    }

    assert(!_thread.joinable());
    _thread = std::thread(&SoundStream::stream_data, this);
  }

  void SoundStream::await_streaming_thread() {
    // Request the thread to join
    {
      std::scoped_lock lock(_thread_mutex);
      _is_streaming = false;
    }

    if (_thread.joinable())
      _thread.join();
  }
} // namespace AltE