add_requires("luau", {
    system = false,
    version = "master",
    configs = {
        shared = false,
        extern_c = true,
        build_cli = false,
    },
})
add_requires("luabridge3", {
    system = false,
    version = "master",
})

local src_lua_dir = path.join(src_dir, "lua")
local module_lua_dir = path.join(module_dir, "lua")

target("lua", function()
    set_kind("$(kind)")
    add_rules("flags")

    set_languages("cxxlatest", "clatest")

    add_defines("STORMKIT_LUA_BUILD", { public = false })

    add_files(path.join(module_dir, "lua.cppm"), { public = true })
    add_files(path.join(src_lua_dir, "lua.cpp"))
    add_files(path.join(src_lua_dir, "core.cppm"))
    add_files(path.join(src_lua_dir, "log.cppm"), { public = true })

    if get_config("entities") then add_files(path.join(src_lua_dir, "entities.cppm"), { public = true }) end
    if get_config("image") then add_files(path.join(src_lua_dir, "image.cppm"), { public = true }) end
    if get_config("wsi") then
        add_files(path.join(src_lua_dir, "wsi.cpp"))
        add_files(path.join(src_lua_dir, "wsi.cppm"), { public = true })
    end
    if get_config("gpu") then add_files(path.join(src_lua_dir, "gpu.cppm"), { public = true }) end

    add_headerfiles(path.join(include_dir, "(stormkit/lua/**.hpp)"))
    add_includedirs(include_dir, { public = true })

    add_deps("core", "log")
    if get_config("image") then add_deps("image") end
    if get_config("entities") then add_deps("entities") end
    if get_config("wsi") then add_deps("wsi") end
    if get_config("gpu") then add_deps("gpu") end

    add_packages("luau", "luabridge3", { public = true })

    add_options("sanitizers")

    set_group("libraries")
end)
