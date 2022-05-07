add_requires("openal-soft", "libogg", "libvorbis")

target("audio-test")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_packages("openal-soft", "libogg", "libvorbis")
    add_deps("assetslib")