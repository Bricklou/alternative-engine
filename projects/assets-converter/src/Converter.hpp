#pragma once

#include <filesystem>

namespace Converter {

void convert(const std::filesystem::path &input_file,
             const std::filesystem::path &output_file);
}; // namespace Converter