add_requires("spdlog", "glm", "entt")

target("engine")
    set_kind("shared")
    add_files("src/**.cpp")
    add_headerfiles("include/**.hpp")

    add_includedirs("include", { public = true })

    add_packages("spdlog", "glm", "entt", { public = true })

    add_defines("ALTE_BUILD")