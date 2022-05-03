#pragma once

#include "../export.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace assetslib {
/**
 * @brief Asset file information
 * This structure basic metadata about an asset file
 */
struct ASSETSLIB_API AssetFile {
  char type[4];
  int version;
  std::string json;
  std::vector<char> binany_blob;
};

/// Compression mode of the asset
enum class CompressionMode : uint32_t { None, LZ4 };

/**
 * @brief Save an asset to a file from its informations
 *
 * @param path path of the file to save
 * @param file asset file to save
 * @return bool true if the file was saved, false if an error occured
 */
bool ASSETSLIB_API save_binaryfile(const std::string &path,
                                   const AssetFile &file);

/**
 * @brief Load an asset from a file
 *
 * @param path path of the file to load
 * @param file asset file to load
 * @return bool true if the file was loaded, false if an error occured
 */
bool ASSETSLIB_API load_binaryfile(const std::string &path, AssetFile &file);

/**
 * @brief Parse the compression mode from a string
 *
 * @param f string containing the compression mode
 * @return CompressionMode compression mode
 */
CompressionMode ASSETSLIB_API parse_compression(const char *f);
} // namespace assetslib