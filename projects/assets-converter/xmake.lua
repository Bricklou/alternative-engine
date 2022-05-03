add_requires("stb", "libsndfile")

target("assets-converter")
    set_kind("binary")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_packages("stb", "libsndfile")
    add_deps("assetslib")