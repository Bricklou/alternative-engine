#pragma once

#include <filesystem>

namespace Converter {
void convert_audio(const std::filesystem::path &input,
                   const std::filesystem::path &output);
}