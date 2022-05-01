# ====================
# Files and executable
# ====================
add_executable(assets-editor
        main.cpp
        App.cpp App.hpp
        AppOnRender.cpp
        )

# ====================
# Linking
# ====================
target_link_libraries(assets-editor
        ${PROJECT_NAME})

# ====================
# Build options
# ====================
target_compile_features(assets-editor PUBLIC cxx_std_20)
set_target_properties(assets-editor PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
