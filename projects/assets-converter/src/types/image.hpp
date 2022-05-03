#pragma once

#include <filesystem>

namespace Converter {
void convert_image(const std::filesystem::path &input,
                   const std::filesystem::path &output);
}