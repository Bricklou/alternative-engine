target("app")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_deps("engine")

    if is_mode("valgrind") then
        on_run(function(target)
            import("core.base.option")
            local args = option.get("arguments") or {}
            local program = target:targetfile()

            import("lib.detect.find_program")

            local p = find_program("valgrind")
            if p ~= nil then
                table.insert(args, 1, program)
                os.execv(p, args)
            else
                print("Valgrind not found")
            end
        end)
    end
