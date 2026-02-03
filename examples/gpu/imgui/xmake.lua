add_requires("imgui", {
    configs = { vulkan = true, debug = true, cxxflags = { "-DIMGUI_IMPL_VULKAN_NO_PROTOTYPES" } },
    system = false,
    debug = true,
})
target("imgui", function()
    set_kind("binary")
    set_languages("cxxlatest", "clatest")

    add_rules("stormkit.flags")
    -- add_rules("platform.windows.subsystem.windows")
    add_rules("platform.windows.subsystem.console")

    add_deps("core", "main", "log", "wsi", "gpu", "stormkit")

    add_packages("imgui")

    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        add_defines("STORMKIT_ASSERT=1")
        set_suffixname("-d")
    else
        add_defines("STORMKIT_ASSERT=0")
    end

    add_files("src/*.cpp", "src/*.mpp", "../common/app.mpp")
    if is_plat("windows") then add_files("win32/*.manifest") end

    if get_config("devmode") then set_rundir("$(projectdir)") end

    set_group("examples/stormkit-gpu")
end)
