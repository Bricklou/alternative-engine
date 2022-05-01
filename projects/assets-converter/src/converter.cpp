#include "converter.hpp"
#include <iostream>

#include "types/images.hpp"

void Converter::convert(const fs::path &input_file,
                        const fs::path &output_file) {
  if (input_file.extension() == ".png"  || input_file.extension() == ".jpg") {
    std::cout << "Found a texture" << std::endl;

    convert_image(input_file, output_file);
  } else if (input_file.extension() == ".wav" || input_file.extension() == ".mp3" || input_file.extension() == ".ogg") {
    std::cout << "Found an audio track" << std::endl;

  } else {
    std::cerr << "Unknown file type" << std::endl;
  }
}
