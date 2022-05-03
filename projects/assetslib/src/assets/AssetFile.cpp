#include <assetslib/assets/AssetFile.hpp>
#include <cstdint>
#include <cstring>
#include <fstream>

namespace assetslib {
  bool save_binaryfile(const std::string &path, const AssetFile &file) {
    std::ofstream outfile;
    outfile.open(path, std::ios::binary | std::ios::out);

    outfile.write(file.type, 4);
    uint32_t version = file.version;
    // version
    outfile.write((char *)&version, sizeof(uint32_t));

    // json length
    uint32_t json_length = file.json.length();
    outfile.write((char *)&json_length, sizeof(uint32_t));

    // blob length
    uint32_t blob_length = file.binany_blob.size();
    outfile.write((char *)&blob_length, sizeof(uint32_t));

    // json stream
    outfile.write(file.json.c_str(), json_length);
    // blob data
    outfile.write(file.binany_blob.data(), blob_length);

    outfile.close();

    return true;
  }

  bool load_binaryfile(const std::string &path, AssetFile &file) {
    std::ifstream infile;
    infile.open(path, std::ios::binary);

    if (!infile.is_open())
      return false;

    // move file cursor to beginning
    infile.seekg(0);

    infile.read(file.type, 4);
    infile.read((char *)&file.version, sizeof(uint32_t));

    uint32_t json_length = 0;
    infile.read((char *)&json_length, sizeof(uint32_t));

    uint32_t blob_length = 0;
    infile.read((char *)&blob_length, sizeof(uint32_t));

    file.json.resize(json_length);
    infile.read(file.json.data(), json_length);

    file.binany_blob.resize(blob_length);
    infile.read(file.binany_blob.data(), blob_length);

    infile.close();

    return true;
  }

  CompressionMode parse_compression(const char *f) {
    if (strcmp(f, "LZ4") == 0) {
      return CompressionMode::LZ4;
    } else {
      return CompressionMode::None;
    }
  }
} // namespace assetslib