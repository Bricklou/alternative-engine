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
    // TODO: implement audio unpacking
  }

  AssetFile pack_audio(AudioInfo *info, void *audio_data) {
    AssetFile file;
    // TODO: implement audio packing

    return file;
  }
} // namespace assetslib