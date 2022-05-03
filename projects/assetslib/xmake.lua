add_requires("lz4", "nlohmann_json")

target("assetslib")
    set_kind("shared")
    add_files("src/**.cpp")
    
    add_includedirs("include", { public = true })

    add_packages("lz4", "nlohmann_json")

    add_defines("ASSETSLIB_BUILD")
