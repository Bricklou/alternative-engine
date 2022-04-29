find_package(Vulkan REQUIRED)

# GLM
add_subdirectory(glm)

# VMA
add_subdirectory(VulkanMemoryAllocator)

# stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE stb_image)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# imgui
find_package(ImGui REQUIRED)

# spdlog
add_subdirectory(spdlog)

# vulkan-hpp
set ( SAMPLES_BUILD OFF CACHE INTERNAL "Build the vulkan hpp samples" )
set ( VULKAN_HPP_INSTALL OFF CACHE INTERNAL "Install vulkan hpp" )
add_subdirectory(Vulkan-Hpp/Vulkan-Headers)

# std_image
add_library(headers INTERFACE)
target_include_directories(headers INTERFACE headers)