add_rules("mode.debug", "mode.release", "mode.valgrind")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("local-repo xmake/repos")

set_project("alternative-engine")

set_languages("cxx20")

if is_mode("release") then
	set_symbols("hidden")
end

set_license("GPL-3.0")

includes("projects/assetslib")
includes("projects/assets-converter")

includes("projects/audio-test")

includes("projects/engine")
includes("projects/app")