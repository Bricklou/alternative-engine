#include "assetslib/assets/AssetFile.hpp"
#include <assetslib/assets/AudioInfo.hpp>
#include <cstring>
#include <lz4.h>
#include <nlohmann/json.hpp>

namespace assetslib {
AudioInfo read_audio_info(const AssetFile *file) {
  AudioInfo info;

  nlohmann::json audio_metadata = nlohmann::json::parse(file->json);

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
  audio_metadata["original_siez"] = info->original_file;

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

  std::string stringified = audio_metadata.dump();
  file.json = stringified;

  return file;
}
} // namespace assetslib