-- add_requires("nzsl", { configs = { fs_watcher = false, kind = "binary" } })

target("textured_cube", function()
    set_kind("binary")
    set_languages("cxxlatest", "clatest")

    add_rules("stormkit.flags")
    add_rules("platform.windows.subsystem.console")
    -- add_rules("platform.windows.subsystem.windows")

    add_rules("compile.shaders")
    add_deps("core", "main", "log", "wsi", "gpu", "stormkit")

    if is_mode("debug") then
        add_defines("STORMKIT_BUILD_DEBUG")
        add_defines("STORMKIT_ASSERT=1")
        set_suffixname("-d")
    else
        add_defines("STORMKIT_ASSERT=0")
    end

    add_files("src/*.cpp", "src/*.mpp", "../common/app.mpp")
    add_files("shaders/*.nzsl")
    if is_plat("windows") then add_files("win32/*.manifest") end

    add_includedirs("$(builddir)/shaders")

    on_load(function(target)
        if get_config("devmode") then
            import("core.project.config")
            local shader_dir = path.unix(path.join(config.builddir(), "shaders"))
            target:add("defines", format('SHADER_DIR="%s"', shader_dir))
            local texture_dir = path.unix(path.join(os.projectdir(), "examples", "gpu", "textured_cube", "textures"))
            target:add("defines", format('TEXTURE_DIR="%s"', texture_dir))
        end
    end)

    if get_config("devmode") then set_rundir("$(projectdir)") end

    set_group("examples/stormkit-gpu")
end)
