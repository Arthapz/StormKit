add_requires("frozen", { system = false, configs = { modules = true, std_import = true, cpp = "latest" } })
add_requires("unordered_dense", { system = false, configs = { modules = true, std_import = true } })
add_requires("tl_function_ref", { system = false, configs = { modules = true, std_import = true } })

target("core", function()
    set_kind("$(kind)")

    set_languages("cxxlatest", "clatest")

    add_defines("STORMKIT_BUILD", { public = false })
    if is_mode("debug") then add_defines("STORMKIT_BUILD_DEBUG", { public = true }) end

    if is_kind("static") then add_defines("STORMKIT_STATIC", { public = true }) end

    add_files("$(projectdir)/modules/stormkit/core.cppm", { public = true })
    add_files("$(projectdir)/modules/stormkit/core/**.cppm", { public = true })
    add_files("*.cpp", "*.cppm")

    if is_plat("linux", "macosx", "ios", "tvos", "android") then add_files("posix/**.cpp") end
    if is_plat("linux") then add_files("linux/**.cpp") end
    if is_plat("windows") then add_files("win32/**.cpp") end
    if is_plat("macosx", "ios", "tvos", "watchos") then add_files("darwin/**.cpp", "darwin/**.m") end

    set_configdir("$(builddir)/.gens/include/")
    add_configfiles("$(projectdir)/include/(stormkit/core/config.hpp.in)")
    add_headerfiles(
        "$(builddir)/.gens/include/(stormkit/core/*.hpp)",
        "$(projectdir)/include/(stormkit/core/**.inl)",
        "$(projectdir)/include/(stormkit/core/**.hpp)"
    )
    add_includedirs("$(projectdir)/include", { public = true })

    add_packages("frozen", "unordered_dense", "tl_function_ref", { public = true })

    on_config(function(target)
        local output, errors = os.iorunv("git", { "rev-parse", "--abbrev-ref", "HEAD" })

        if not errors == "" then
            print("Failed to get git hash and branch, reason: ", errors, output)
            target:set("configvar", "STORMKIT_GIT_BRANCH", " ")
            target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", " ")
            return
        end

        target:set("configvar", "STORMKIT_GIT_BRANCH", output:trim())
        output, errors = os.iorunv("git", { "rev-parse", "--verify", "HEAD" })

        target:set("configvar", "STORMKIT_GIT_COMMIT_HASH", output:trim())
    end)

    set_group("libraries")
end)
