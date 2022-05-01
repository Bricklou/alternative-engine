#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace assetslib {
  struct AssetFile {
      char type[4];
      int version;
      std::string json;
      std::vector<char> binany_blob;
  };

  enum class CompressionMode : uint32_t { None, LZ4 };

  bool save_binaryfile(const std::string &path, const AssetFile &file);
  bool load_binaryfile(const std::string &path, AssetFile &file);

  CompressionMode parse_compression(const char *f);
} // namespace assetslib