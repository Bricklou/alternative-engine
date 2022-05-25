#pragma once

#include <alte/export.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace AltE {
  /// Pixel color format of the texture
  enum class TextureFormat {
    Unknown = 0,
    RGBA,
  };

  /**
   * @brief Information about the texture
   * This structure contains required informations to be able to load a texture
   */
  struct ALTE_API TextureInfo {
      /// Pixel color format of the texture
      TextureFormat format;
      /// Original texture name
      std::string filename;
      /// Texture buffer
      std::vector<char> data;

      /// Width and height of the texture
      int width, height;

      /**
       * @brief Load an audio file from a path
       *
       * @param path path to the file to load
       * @return audio metadata
       */
      static TextureInfo read_from_file(const std::filesystem::path &file_path);
  };
} // namespace AltE