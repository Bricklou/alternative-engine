find_package(Vulkan REQUIRED)

# vk-bootstrap
add_subdirectory(vkbootstrap)

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
