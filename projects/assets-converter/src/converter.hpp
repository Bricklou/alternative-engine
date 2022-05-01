#pragma once

#include "fs.hpp"

namespace Converter {

  void convert(const fs::path &input_file,
               const fs::path &output_file);
};