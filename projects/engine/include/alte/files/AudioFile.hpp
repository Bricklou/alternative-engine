#pragma once

#include <alte/export.hpp>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace AltE {
  /// Audio channels format of the audio
  enum class AudioFormat {
    Unknown = 0,
    Mono8,
    Mono16,
    Stereo8,
    Stereo16,
  };

  /**
   * @brief Information about the audio
   * This structure contains required informations to be able to load an audio
   * file
   */
  struct ALTE_API AudioInfo {
      /// Original audio name
      std::string filename;
      // Audio buffer
      std::vector<char> audio_data;

      /// Audio channels format of the audio
      AudioFormat format;
      /// Audio sample rat
      uint32_t sample_rate;
      /// Audio channels number
      uint32_t channels;

      /**
       * @brief Load an audio file from a path
       *
       * @param path path to the file to load
       * @return audio metadata
       */
      static AudioInfo read_from_file(const std::filesystem::path &file_path);
  };
} // namespace AltE