#include "assetslib/assets/AssetFile.hpp"
#include <assetslib/assets/AudioInfo.hpp>
#include <cstring>
// TODO: use libogg to compress/uncompress audio
#include <lz4.h>
#include <nlohmann/json.hpp>

namespace assetslib {
AudioFormat parse_format(const char *f) {
  if (strcmp(f, "MONO16") == 0) {
    return AudioFormat::Mono16;
  } else if (strcmp(f, "STEREO16") == 0) {
    return AudioFormat::Stereo16;
  } else if (strcmp(f, "STEREO8") == 0) {
    return AudioFormat::Stereo8;
  } else if (strcmp(f, "3D_16") == 0) {
    return AudioFormat::B3D_16;
  } else {
    return AudioFormat::Unknown;
  }
}

std::string unparse_format(AudioFormat f) {
  switch (f) {
  case AudioFormat::Mono16:
    return "MONO16";
  case AudioFormat::Stereo16:
    return "STEREO16";
  case AudioFormat::Stereo8:
    return "STEREO8";
  case AudioFormat::B3D_16:
    return "3D_16";
  default:
    return "UNKNOWN";
  }
}

AudioInfo read_audio_info(const AssetFile *file) {
  AudioInfo info;

  nlohmann::json audio_metadata = nlohmann::json::parse(file->json);

  std::string formatString = audio_metadata["format"];
  info.audio_format = parse_format(formatString.c_str());

  std::string compressionString = audio_metadata["compression"];
  info.compression_mode = parse_compression(compressionString.c_str());

  info.audio_size = audio_metadata["buffer_size"];
  info.original_file = audio_metadata["original_file"];

  return info;
}

void unpack_audio(AudioInfo *info, const char *source_buffer,
                  size_t source_size, char *destination) {
  if (info->compression_mode == CompressionMode::LZ4) {
    LZ4_decompress_safe(source_buffer, destination, source_size,
                        info->audio_size);
  } else {
    std::memcpy(destination, source_buffer, source_size);
  }
}

AssetFile pack_audio(AudioInfo *info, void *audio_data) {
  nlohmann::json audio_metadata;
  audio_metadata["buffer_size"] = info->audio_size;
  audio_metadata["original_file"] = info->original_file;

  // core file header
  AssetFile file;
  file.type[0] = 'A';
  file.type[1] = 'U';
  file.type[2] = 'D';
  file.type[3] = 'I';
  file.version = 1;

  // compress audio data
  int compress_staging = LZ4_compressBound(info->audio_size);
  file.binany_blob.resize(compress_staging);

  int compressed_size =
      LZ4_compress_default((const char *)audio_data, file.binany_blob.data(),
                           info->audio_size, compress_staging);

  file.binany_blob.resize(compressed_size);

  audio_metadata["compression"] = "LZ4";
  audio_metadata["format"] = unparse_format(info->audio_format);

  std::string stringified = audio_metadata.dump();
  file.json = stringified;

  return file;
}
} // namespace assetslib