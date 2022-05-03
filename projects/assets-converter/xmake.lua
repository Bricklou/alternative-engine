add_requires("stb", "openal-soft", "libsndfile")

target("assets-converter")
    set_kind("binary")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_packages("stb", "openal-soft", "libsndfile")
    add_deps("assetslib")