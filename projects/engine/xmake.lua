add_requires("glm", "entt", "openal-soft", "fmt")
add_requires("spdlog", { system = false })
add_requires("assetslib")

target("engine")
    set_kind("shared")
    add_files("src/**.cpp")
    add_headerfiles("include/**.hpp")

    add_includedirs("include", { public = true })

    add_packages("spdlog", "glm", "entt", { public = true })
    add_packages("assetslib", "openal-soft")

    add_defines("ALTE_BUILD")
    add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
    add_defines("SPDLOG_FMT_EXTERNAL")