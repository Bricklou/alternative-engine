#pragma once

#include "../export.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace assetslib {
struct ASSETSLIB_API AssetFile {
  char type[4];
  int version;
  std::string json;
  std::vector<char> binany_blob;
};

enum class CompressionMode : uint32_t { None, LZ4 };

bool ASSETSLIB_API save_binaryfile(const std::string &path,
                                   const AssetFile &file);
bool ASSETSLIB_API load_binaryfile(const std::string &path, AssetFile &file);

CompressionMode ASSETSLIB_API parse_compression(const char *f);
} // namespace assetslib