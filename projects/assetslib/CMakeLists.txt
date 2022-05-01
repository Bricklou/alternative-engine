add_library(assetslib
    src/assetslib.cpp
    src/assets/AssetFile.cpp
    src/assets/TextureInfo.cpp
    src/assets/AudioInfo.cpp
)

target_include_directories(assetslib PUBLIC include)
target_link_libraries(assetslib PUBLIC nlohmann_json::nlohmann_json lz4)

# ====================
# Build options
# ====================
target_compile_features(assetslib PUBLIC cxx_std_20)
set_target_properties(assetslib PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
