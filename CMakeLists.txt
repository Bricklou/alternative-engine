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
        src/engine/App.hpp src/engine/App.cpp
        src/engine/application/Window.hpp src/engine/application/Window.cpp
        src/engine/core/logger.hpp src/engine/core/logger.cpp
        src/engine/rendering/Renderer.hpp src/engine/rendering/Renderer.cpp
        src/engine/rendering/vulkan/VulkanInstance/VulkanInstance.cpp src/engine/rendering/vulkan/VulkanInstance/VulkanInstance.hpp
        src/engine/rendering/vulkan/Device/Device.cpp src/engine/rendering/vulkan/Device/Device.hpp
        src/engine/rendering/vulkan/SwapChain/SwapChain.cpp src/engine/rendering/vulkan/SwapChain/SwapChain.hpp
        src/engine/rendering/vulkan/CommandBuffers/Commandbuffers.cpp src/engine/rendering/vulkan/CommandBuffers/Commandbuffers.hpp
        src/engine/rendering/ImGui/ImGui.cpp src/engine/rendering/ImGui/ImGui.hpp)

# ====================
# Linking
# ====================
target_link_libraries(${PROJECT_NAME} Vulkan::Headers VulkanMemoryAllocator glm imgui stb_image)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan SDL2)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog headers)

# ====================
# Build options
# ====================
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
