package("assetslib")
    set_description("The assetslib package")
    set_license("GPL-3.0")

    add_urls("https://github.com/bricklou/assetslib.git")
    add_versions("v1.0.1", "61d142c55ba067706b31565d79dfc183308f86f4")
    add_versions("v1.0.0", "a2c5682c1edf701f136ca05e9d477237e8ab658d")

    on_install(function (package)
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        end
        local opt = {}
        opt.target = "assetslib"
        import("package.tools.xmake").install(package, configs, opt)
    end)