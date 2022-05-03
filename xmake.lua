add_rules("mode.debug", "mode.release")

set_project("alternative-engine")

set_languages("cxx20")
set_symbols("hidden")

set_license("GPL-3.0")

includes("projects/assetslib")
includes("projects/assets-converter")