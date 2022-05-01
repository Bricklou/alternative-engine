#pragma once

#include "../fs.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assetslib/assets/TextureInfo.hpp>

void convert_image(const fs::path& input, const fs::path& output) {
  int width, height, channels;

  stbi_uc* pixels = stbi_load(input.c_str(), &width, &height, &channels, STBI_rgb_alpha);

  if (!pixels) {
    throw std::runtime_error("Failed to load texture file: " + input.string());
  }

  assetslib::TextureInfo textureInfo;
  textureInfo.texture_size = width*height*4;
  textureInfo.texture_format = assetslib::TextureFormat::RGBA8;
  textureInfo.original_file = input.string();
  textureInfo.width = width;
  textureInfo.height = height;

  assetslib::AssetFile newImage = assetslib::pack_texture(&textureInfo, pixels);

  stbi_image_free(pixels);

  assetslib::save_binaryfile(output.string(), newImage);
  std::cout << "Texture saved to: " << output << std::endl;
}