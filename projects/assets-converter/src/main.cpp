#include "Converter.hpp"
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <input_file> <output_path>"
              << std::endl;
    return 1;
  }

  std::filesystem::path input_path{argv[1]};

  if (!std::filesystem::exists(input_path)) {
    std::cerr << "Given input_path does not exist: " << input_path << std::endl;
    return 1;
  } else if (!std::filesystem::is_regular_file(input_path)) {
    std::cerr << "Given input path is not a valid file: " << input_path
              << std::endl;
    return 1;
  }

  std::filesystem::path export_path{argv[2]};

  try {
    if (!std::filesystem::exists(export_path.parent_path())) {
      std::filesystem::create_directories(export_path.parent_path());
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to create export path directories: " << e.what()
              << std::endl;
    return 1;
  }

  try {
    Converter::convert(input_path, export_path);
  } catch (const std::exception &e) {
    std::cerr << "Error while converting file: " << std::endl
              << e.what() << std::endl;
  }
}