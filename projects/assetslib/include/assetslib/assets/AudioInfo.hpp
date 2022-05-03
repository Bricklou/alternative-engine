#pragma once

#include <assetslib/assets/AssetFile.hpp>
#include <cstddef>
#include <cstdint>

namespace assetslib {
enum class AudioFormat : uint32_t {
  Unknown = 0,
};

struct ASSETSLIB_API AudioInfo {
  uint32_t audio_size;
  CompressionMode compression_mode;
  int sample_rate;
  int channels_count;
  int format;

  std::string original_file;
};

// Parse the audio metadata from as asset file
AudioInfo ASSETSLIB_API read_audio_info(const AssetFile *file);

void ASSETSLIB_API unpack_audio(AudioInfo *info, const char *source_buffer,
                                size_t source_size, char *destination);

AssetFile ASSETSLIB_API pack_audio(AudioInfo *info, void *audio_data);
} // namespace assetslib