#include <assetslib/assets/TextureInfo.hpp>
#include <cstring>
#include <lz4.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace assetslib {
TextureFormat parse_format(const char *f) {
  if (strcmp(f, "RGBA8") == 0) {
    return TextureFormat::RGBA8;
  } else {
    return TextureFormat::Unkown;
  }
}

TextureInfo read_texture_info(const AssetFile *file) {
  TextureInfo info;

  nlohmann::json texture_metadata = nlohmann::json::parse(file->json);

  std::string formatString = texture_metadata["format"];
  info.texture_format = parse_format(formatString.c_str());

  std::string compressionString = texture_metadata["compression"];
  info.compression_mode = parse_compression(compressionString.c_str());

  info.width = texture_metadata["width"];
  info.height = texture_metadata["height"];
  info.texture_size = texture_metadata["buffer_size"];
  info.original_file = texture_metadata["original_file"];

  return info;
}

void unpack_texture(TextureInfo *info, const char *source_buffer,
                    size_t source_size, char *destination) {
  if (info->compression_mode == CompressionMode::LZ4) {
    LZ4_decompress_safe(source_buffer, destination, source_size,
                        info->texture_size);
  } else {
    std::memcpy(destination, source_buffer, source_size);
  }
}

AssetFile pack_texture(TextureInfo *info, void *pixel_data) {
  nlohmann::json texture_metadata;
  texture_metadata["format"] = "RGBA8";
  texture_metadata["width"] = info->width;
  texture_metadata["height"] = info->height;
  texture_metadata["buffer_size"] = info->texture_size;
  texture_metadata["original_file"] = info->original_file;

  // core file header
  AssetFile file;
  file.type[0] = 'T';
  file.type[1] = 'E';
  file.type[2] = 'X';
  file.type[3] = 'I';
  file.version = 1;

  // compress buffer into blob
  int compress_staging = LZ4_compressBound(info->texture_size);

  file.binany_blob.resize(compress_staging);

  int compressed_size =
      LZ4_compress_default((const char *)pixel_data, file.binany_blob.data(),
                           info->texture_size, compress_staging);

  file.binany_blob.resize(compressed_size);

  texture_metadata["compression"] = "LZ4";

  std::string stringified = texture_metadata.dump();
  file.json = stringified;

  return file;
}
} // namespace assetslib