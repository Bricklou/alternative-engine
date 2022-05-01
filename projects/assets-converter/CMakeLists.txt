add_executable(assets-converter
        src/main.cpp
        src/converter.cpp
        src/converter.hpp
        src/types/images.hpp
        src/fs.hpp)

target_link_libraries(assets-converter PUBLIC
        assetslib
        stb_image
        )

# ====================
# Build options
# ====================
target_compile_features(assets-converter PUBLIC cxx_std_20)
set_target_properties(assets-converter PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
