add_requires("spdlog", "glm")

target("engine")
    set_kind("shared")
    add_files("src/**.cpp")

    add_includedirs("include", { public = true })

    add_packages("spdlog", "glm", { public = true })

    add_defines("ALTE_BUILD")