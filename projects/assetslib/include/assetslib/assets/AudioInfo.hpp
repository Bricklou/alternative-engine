#pragma once

#include <assetslib/assets/AssetFile.hpp>
#include <cstddef>
#include <cstdint>

namespace assetslib {
  enum class AudioFormat : uint32_t {
    Unknown = 0,
  };

  struct AudioInfo {
      uint32_t audio_size;
      CompressionMode compression_mode;

      std::string original_file;
  };

  // Parse the audio metadata from as asset file
  AudioInfo read_audio_info(const AssetFile *file);

  void unpack_audio(AudioInfo *info, const char *source_buffer,
                    size_t source_size, char *destination);

  AssetFile pack_audio(AudioInfo *info, void *audio_data);
} // namespace assetslib