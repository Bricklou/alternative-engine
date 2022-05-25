add_requires("glm", "entt", "fmt")
add_requires("spdlog", { system = false })
add_requires("libsndfile", "openal-soft", "stb", { configs = { shared = true } })

target("engine")
    set_kind("shared")
    add_files("src/**.cpp")
    add_headerfiles("include/**.hpp", "include/**.inl")

    add_includedirs("include", { public = true })

    add_packages(
        "spdlog",
        "glm",
        "entt",
        "fmt",
        "libsndfile",
        "stb",
    { public = true })

    add_packages(
        "openal-soft"
    )

    add_defines("ALTE_BUILD")
    add_defines("SPDLOG_FMT_EXTERNAL")

    if is_mode("debug") then
        add_defines("SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
    end

    on_load(function (target)
        if target:kind() == "static" then
            target:add("defines", "ALTE_STATIC", { public = true })
        end 
    end)