add_requires("libogg", "libvorbis")
add_requires("openal-soft", { configs = { shared = true } })

target("audio-test")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_packages("openal-soft", "libogg", "libvorbis")
