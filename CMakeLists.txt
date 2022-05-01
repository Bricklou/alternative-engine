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
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(cmake/glslvalidator.cmake)

# ====================
# Find Dependencies
# ====================
find_package(Vulkan REQUIRED)
add_subdirectory(third_party)

# ====================
# Assets management
# ====================
add_subdirectory(projects/assetslib)
add_subdirectory(projects/assets-converter)
include(cmake/assets_convertion.cmake)

# ====================
# ENGINE
# ====================
add_subdirectory(projects/engine)
add_subdirectory(projects/assets-editor)
add_subdirectory(projects/app)