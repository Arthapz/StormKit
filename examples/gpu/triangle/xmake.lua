add_requires("nzsl", {
    configs = {
        fs_watcher = false,
        kind = "binary",
        toolchain = is_plat("windows") and "cl" or nil,
        runtimes = is_plat("windows") and "MD" or nil,
    },
})

target("triangle", function()
    add_rules("stormkit::example", "compile.shaders")

    add_files("src/*.cpp", "src/*.cppm", "../common/app.cppm", "shaders/*.nzsl")

    if get_config("devmode") then
        add_defines(format('RESOURCE_DIR="%s"', path.unix(path.join(os.projectdir(), "gpu/triangle"))))
        add_defines(format('SHADER_DIR="%s"', path.unix("$(builddir)/shaders")))
    end
    add_embeddirs("$(builddir)/shaders")

    if get_config("devmode") then set_rundir("$(projectdir)") end

    add_packages("nzsl")

    set_group("examples/stormkit-gpu")
end)
