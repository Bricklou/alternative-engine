#pragma once

#include <assetslib/assets/AssetFile.hpp>
#include <assetslib/assetslib.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

namespace assetslib {
  enum class TextureFormat : uint32_t {
    Unkown = 0,
    RGBA8,
  };


  struct TextureInfo {
      uint64_t texture_size;
      TextureFormat texture_format;
      CompressionMode compression_mode;

      uint32_t width;
      uint32_t height;

      std::string original_file;
  };

  // Parses the texture metadata from an asset file
  TextureInfo read_texture_info(const AssetFile *file);

  void unpack_texture(TextureInfo *info, const char *source_buffer,
                      size_t source_size, char *destination);

  void unpack_texture_page(TextureInfo *info, int page_index,
                           char *source_buffer, char *destination);

  AssetFile pack_texture(TextureInfo *info, void *pixel_data);
} // namespace assetslib