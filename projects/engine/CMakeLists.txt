# ====================
# Files and library
# ====================
add_library(${PROJECT_NAME}
        src/App.cpp
        src/application/Window.cpp
        src/core/logger.cpp
        src/rendering/Renderer.cpp
        src/rendering/vulkan/VulkanInstance.cpp
        src/rendering/vulkan/Device.cpp
        src/rendering/vulkan/SwapChain.cpp
        src/rendering/vulkan/Commandbuffers.cpp
        src/rendering/ImGui/ImGui.cpp
        )

# ====================
# Linking
# ====================
target_link_libraries(${PROJECT_NAME} PUBLIC
        Vulkan::Headers
        VulkanMemoryAllocator
        glm
        imgui
        Vulkan::Vulkan
        SDL2
        spdlog::spdlog
        stb_image
        )
target_include_directories(${PROJECT_NAME} PUBLIC includes)

# ====================
# Build options
# ====================
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(alternative-engine PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        )
