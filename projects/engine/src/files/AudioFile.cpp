#include <alte/core/logger.hpp>
#include <alte/files/AudioFile.hpp>
#include <climits>
#include <sndfile.h>
#include <stdexcept>

namespace AltE {
  AudioInfo AudioInfo::read_from_file(const std::filesystem::path &file_path) {
    SNDFILE *sndfile;
    SF_INFO sfinfo;
    sf_count_t num_frames;

    AudioFormat format;
    AudioInfo info = {};

    // Open the audio file and check that it's usable
    sndfile = sf_open(file_path.string().c_str(), SFM_READ, &sfinfo);
    if (!sndfile) {
      SPDLOG_ERROR("Failed to load audio file \"{}\": {}", file_path.string(),
                   sf_strerror(sndfile));
      throw std::runtime_error("Failed to load audio file:" +
                               file_path.string());
    }

    if (sfinfo.frames < 1 ||
        sfinfo.frames >
            (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
      sf_close(sndfile);
      SPDLOG_ERROR("Bad sample count in audio file \"{}\": {}",
                   file_path.string(), sfinfo.frames);
      throw std::runtime_error("Bad sample count in audio file: " +
                               file_path.string());
    }

    switch (sfinfo.channels) {
      case 1:
        info.format = AudioFormat::Mono16;
        break;
      case 2:
        info.format = AudioFormat::Stereo16;
        break;
      default:
        SPDLOG_ERROR("Unsupported audio format for audio file \"{}\": found {}",
                     file_path.string(), sfinfo.channels);
        throw std::runtime_error("Unsupported audio format for audio file: " +
                                 file_path.string());
    }

    // Devoce the whole audio file to a buffer
    info.audio_data.resize((sfinfo.frames * sfinfo.channels) * sizeof(char) *
                           2);

    num_frames =
        sf_readf_short(sndfile, (short *)info.audio_data.data(), sfinfo.frames);

    if (num_frames < 1) {
      sf_close(sndfile);
      SPDLOG_DEBUG("Failed to read audio samples for \"{}\": {}",
                   file_path.string(), num_frames);
      throw std::runtime_error("Failed to read audio samples: " +
                               file_path.string());
    }

    info.sample_rate = sfinfo.samplerate;
    info.channels = sfinfo.channels;
    info.filename = file_path.filename().string();

    sf_close(sndfile);

    return info;
  };
} // namespace AltE