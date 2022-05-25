#include <alte/files/TextureFile.hpp>

#include <alte/core/logger.hpp>
#include <cstddef>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace AltE {
  TextureInfo
  TextureInfo::read_from_file(const std::filesystem::path &file_path) {
    TextureInfo info = {};
    int channels;

    // Open the texture file and check that it's usable
    stbi_uc *pixels = stbi_load(file_path.string().c_str(), &info.width,
                                &info.height, &channels, STBI_rgb_alpha);

    if (!pixels) {
      SPDLOG_ERROR("Failed to load texture file \"{}\"", file_path.string());
      throw std::runtime_error("Failed to load texture file:" +
                               file_path.string());
    }

    std::size_t size = info.width * info.height * 4;
    info.data.resize(size);
    std::memcpy(info.data.data(), pixels, size);

    info.format = TextureFormat::RGBA;
    info.filename = file_path.filename().string();

    stbi_image_free(pixels);

    return info;
  }
} // namespace AltE