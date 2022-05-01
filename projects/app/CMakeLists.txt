# ====================
# Files and executable
# ====================
add_executable(main
        main.cpp
        App.cpp App.hpp
        )

# ====================
# Linking
# ====================
target_link_libraries(main
        ${PROJECT_NAME})

# ====================
# Build options
# ====================
target_compile_features(main PUBLIC cxx_std_20)
set_target_properties(main PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
