#include "Converter.hpp"
#include <iostream>

#include "types/audio.hpp"
#include "types/image.hpp"

namespace Converter {
void convert(const std::filesystem::path &input_file,
             const std::filesystem::path &output_file) {
  if (input_file.extension() == ".png" || input_file.extension() == ".jpg") {
    std::cout << "Found a texture" << std::endl;

    convert_image(input_file, output_file);
  } else if (input_file.extension() == ".wav" ||
             input_file.extension() == ".mp3" ||
             input_file.extension() == ".ogg") {
    std::cout << "Found an audio track" << std::endl;

    convert_audio(input_file, output_file);
  } else {
    std::cerr << "Unknown file type" << std::endl;
  }
}
} // namespace Converter