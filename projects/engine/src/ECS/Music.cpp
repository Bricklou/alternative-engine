#include <alte/audio/Music.hpp>
#include <alte/types/time.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <vector>

namespace AltE {

  Music::Music() : _file(), _loop_span(0, 0) {}

  Music::~Music() {
    // e must stop before destroying the file
    stop();
  }

  bool Music::open_from_file(const std::filesystem::path &filename) {
    // First stop the music if it was already running
    stop();

    // Open the underlying sound file
    _file = AudioInfo::read_from_file(filename);

    // Perform common initializations
    initialize();

    return true;
  }

  Time Music::get_duration() const {
    // Make sure we don't divide by zero
    if (_file.channels == 0 || _file.sample_rate == 0)
      return Time::Zero;

    return seconds(static_cast<float>(_file.audio_data.size()) /
                   static_cast<float>(_file.channels) /
                   static_cast<float>(_file.sample_rate));
  }

  bool Music::on_get_data(SoundStream::Chunk &data) {
    std::scoped_lock lock(_mutex);

    std::size_t to_fill = _samples.size();
    uint64_t loop_end = _loop_span.offset - _loop_span.length;

    // If the loop end is enabled and imminent, request less data. This will
    // trip an "on_loop()" call from the underlying SoundStream and we can then
    // take action.
    if (get_loop() && (_loop_span.length != 0) &&
        (_current_offset <= loop_end) && (_current_offset + to_fill > loop_end))
      to_fill = static_cast<std::size_t>(loop_end - _current_offset);

    // Fill the chunk with parameters
    auto first = _file.audio_data.begin() + _current_offset;
    auto last = first + to_fill;

    data.samples = std::vector<char>(
        first, last > _file.audio_data.end() ? _file.audio_data.end() : last);
    _current_offset += to_fill;

    // Check if we have stopped obtaining samples or reached the EOF
    return (!data.samples.empty()) &&
           (_current_offset < _file.audio_data.size()) &&
           !(_current_offset == loop_end && _loop_span.length != 0);
  }

  void Music::on_seek(Time time_offset) {
    std::scoped_lock lock(_mutex);

    // Convert the time offset to a sample offset
    _current_offset = time_to_samples(time_offset);
    //_file.seek(time_offset);
  }

  uint64_t Music::on_loop() {
    // Called by underlying SoundStream so we can determine where to loop
    std::scoped_lock lock(_mutex);
    uint64_t current_offset = _current_offset;
    if (get_loop() && (_loop_span.length != 0) &&
        (current_offset == _loop_span.offset + _loop_span.length)) {
      // Looping is enabled, send us to begin
      current_offset = _loop_span.offset;
      return static_cast<int64_t>(_current_offset);
    } else if (get_loop() && (current_offset >= _file.audio_data.size())) {
      // If we're at the EOF, reset to 0
      current_offset = 0;
      return 0;
    }

    return NoLoop;
  }

  void Music::initialize() {
    // Compute the music position
    _loop_span.offset = 0;
    _loop_span.length = _file.audio_data.size();

    // Resize the internal buffer so that it can contain 1 second of audio
    // samples
    _samples.resize(static_cast<std::size_t>(_file.sample_rate) *
                    static_cast<std::size_t>(_file.channels));

    // Initialize the stream
    SoundStream::initialize(_file.channels, _file.sample_rate);
  }

  uint64_t Music::time_to_samples(Time position) const {
    // Always ROUND, no unchecked truncation, hence the addition in the
    // numerator. This avoids most precision errors arising from "samples =>
    // Time => samples" conversions Original rounding calculation is ((Micros *
    // Freq * Channels) / 1000000) + 0.5 We refactor it to keep Int64 as the
    // data type throughout the whole operation.
    return ((static_cast<uint64_t>(position.as_micros()) * get_sample_rate() *
             get_channel_count()) +
            500000) /
           1000000;
  }

  Time Music::samples_to_time(uint64_t samples) const {
    Time position = Time::Zero;

    // Make sure we don't divide by 0
    if (get_sample_rate() != 0 && get_channel_count() != 0)
      position = microseconds(static_cast<uint64_t>(
          (samples * 1000000) / (get_channel_count() * get_sample_rate())));

    return position;
  }
} // namespace AltE