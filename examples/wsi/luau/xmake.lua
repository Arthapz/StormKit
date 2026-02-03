if get_config("luau") then
    namespace("luau", function()
        target("events", function()
            set_kind("binary")
            set_languages("cxxlatest", "clatest")

            add_rules("stormkit.flags")
            add_rules("platform.windows.subsystem.console")

            add_deps("stormkit::core", "stormkit::main", "stormkit::log", "stormkit::wsi", "stormkit::luau")

            add_packages("luau")

            if is_mode("debug") then
                add_defines("STORMKIT_BUILD_DEBUG")
                add_defines("STORMKIT_ASSERT=1")
                set_suffixname("-d")
            else
                add_defines("STORMKIT_ASSERT=0")
            end

            add_files("src/main.cpp")
            -- if is_plat("windows") then add_files("win32/*.manifest") end

            on_load(function(target)
                if get_config("devmode") then
                    local lua_dir = path.unix(path.join(os.projectdir(), "examples", "wsi", "luau", "luau"))
                    target:add("defines", format('LUAU_DIR="%s"', lua_dir))
                end
            end)

            if get_config("devmode") then set_rundir("$(projectdir)") end

            set_group("examples/stormkit-wsi/luau")
        end)
    end)
end
