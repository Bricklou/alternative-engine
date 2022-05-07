#pragma once

#include <filesystem>

namespace Converter {
// https://github.com/nothings/stb/blob/master/stb_dxt.h
void convert_image(const std::filesystem::path &input,
                   const std::filesystem::path &output);
} // namespace Converter