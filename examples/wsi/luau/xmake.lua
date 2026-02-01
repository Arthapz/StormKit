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
            if is_plat("windows") then add_files("win32/*.manifest") end

            if get_config("devmode") then
                add_defines('LUAU_DIR="$(builddir)/luau"')
                set_rundir("$(projectdir)")

                after_build(function(target) os.cp("examples/wsi/luau/luau", "$(builddir)") end)
            end

            set_group("examples/stormkit-wsi/luau")
        end)
    end)
end
