cmake_minimum_required(VERSION 3.17)

# ====================
# Project information
# ====================
project(alternative-engine
    VERSION 1.0.0
    LANGUAGES CXX
)

# ====================
# CMake Configuration
# ====================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(cmake/glslvalidator.cmake)

# ====================
# Find Dependencies
# ====================
find_package(Vulkan REQUIRED)
add_subdirectory(third_party)

# ====================
# Files and executable
# ====================
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/engine/application/App.hpp src/engine/application/App.cpp
    src/engine/application/Window.hpp src/engine/application/Window.cpp
    src/engine/core/logger.hpp src/engine/core/logger.cpp
)

# ====================
# Linking
# ====================
target_link_libraries(${PROJECT_NAME} vk-bootstrap::vk-bootstrap VulkanMemoryAllocator glm imgui stb_image)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan SDL2)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

# ====================
# Build options
# ====================
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
