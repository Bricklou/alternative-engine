add_library(imgui STATIC)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

target_sources(imgui
    PRIVATE
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp

    PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl.cpp
)

target_include_directories(imgui
    PUBLIC ${IMGUI_DIR}
    PUBLIC ${IMGUI_DIR}/backends
)
