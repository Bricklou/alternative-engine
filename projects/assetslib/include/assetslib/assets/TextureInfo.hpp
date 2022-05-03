#pragma once

#include <assetslib/assets/AssetFile.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

namespace assetslib {

// Pixel color format of the texture
enum class TextureFormat : uint32_t {
  Unkown = 0,
  RGBA8,
};

/**
 * @brief Information about the texture
 * This structure contains required informations to be able to load a texture
 */
struct ASSETSLIB_API TextureInfo {
  /// Size in bytes of the texture
  uint64_t texture_size;
  /// Pixel color format of the texture
  TextureFormat texture_format;
  /// Compression mode of the asset
  CompressionMode compression_mode;

  /// Texture width (in pixels)
  uint32_t width;
  /// Texture height (in pixels)
  uint32_t height;

  /// Original file name of the texture
  std::string original_file;
};

// Parses the texture metadata from an asset file
TextureInfo ASSETSLIB_API read_texture_info(const AssetFile *file);

/**
 * @brief Unpack the texture into a buffer to use it later
 *
 * @param info texture informations
 * @param source_buffer buffer containing the texture data (from the asset
 * struct)
 * @param source_size size of the source buffer
 * @param destination buffer to store the unpacked texture
 */
void ASSETSLIB_API unpack_texture(TextureInfo *info, const char *source_buffer,
                                  size_t source_size, char *destination);

/**
 * @brief Pack the texture into a buffer to be stored in the asset struct
 *
 * @param info texture informations
 * @param pixel_data buffer containing the texture data
 * @return AssetFile asset file containing the texture metadata and the texture
 * itself
 */
AssetFile ASSETSLIB_API pack_texture(TextureInfo *info, void *pixel_data);
} // namespace assetslib