#pragma once

#include <assetslib/assets/AssetFile.hpp>
#include <cstddef>
#include <cstdint>

namespace assetslib {

/// Audio channels format of the audio
enum class AudioFormat : uint32_t {
  Unknown = 0,
  Mono8,
  Mono16,
  Stereo8,
  Stereo16,
  B3D_16,
};

/**
 * @brief Information about the audio
 * This structure contains required informations to be able to load an audio
 */
struct ASSETSLIB_API AudioInfo {
  /// Size in bytes of the audio
  uint32_t audio_size;
  /// Compression mode of the asset
  CompressionMode compression_mode;
  /// Audio sample rate
  int sample_rate;
  /// Audio channels count
  int channels_count;
  /// Audio channels format
  AudioFormat audio_format;

  /// Original file name of the audio
  std::string original_file;
};

// Parse the audio metadata from as asset file
AudioInfo ASSETSLIB_API read_audio_info(const AssetFile *file);

/**
 * @brief Unpack the audio into a buffer to use it later
 *
 * @param info audio informations
 * @param source_buffer buffer containing the audio data (from the asset struct)
 * @param source_size size of the source buffer
 * @param destination buffer to store the unpacked audio
 */
void ASSETSLIB_API unpack_audio(AudioInfo *info, const char *source_buffer,
                                size_t source_size, char *destination);

/**
 * @brief Pack the audio into a buffer to be stored in the asset struct
 *
 * @param info audio informations
 * @param audio_data buffer containing the audio data
 * @return AssetFile asset file containing the audio metadata and the audio
 */
AssetFile ASSETSLIB_API pack_audio(AudioInfo *info, void *audio_data);
} // namespace assetslib