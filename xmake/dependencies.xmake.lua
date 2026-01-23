local global_package_configs = {
    configs = {
        shared = get_config("shared_deps"),
        runtimes = get_config("toolchain") == "llvm" and get_config("runtimes") or nil,
    },
}

if get_config("on_ci") then global_package_configs.system = false end

if not is_plat("windows") then add_requires("libdwarf") end

local cxx_isystem = "--cxx-isystem"
local cxx_runtime = nil
if get_config("toolchain") == "llvm" then
    if get_config("sdk") then
        cxx_isystem = cxx_isystem .. path.join(get_config("sdk"), "include", "c++", "v1")
    elseif is_plat("linux") or is_plat("darwin") then
        cxx_isystem = cxx_isystem .. "/usr/include/c++/v1"
    end
    if get_config("runtimes") and get_config("runtimes"):startswith("c++") then cxx_runtime = "-stdlib=libc++" end
end

local package_configs = {
    ["libjpeg-turbo"] = {
        windows = {
            system = false,
            configs = {
                runtimes = "MD",
                shared = true,
            },
        },
    },
    luau = {
        global = {
            system = false,
            version = "master",
            configs = {
                extern_c = true,
                cxflags = { cxx_runtime },
            },
        },
    },
    luabridge3 = {
        global = {
            system = false,
            version = "master",
        },
    },
    libktx = {
        llvm = {
            configs = {
                cxflags = "-Wno-overriding-option",
            },
        },
    },
    libxkbcommon = {
        global = {
            system = false,
            configs = {
                wayland = true,
                x11 = true,
            },
        },
    },
    unordered_dense = {
        global = {
            system = false,
            configs = {
                modules = true,
                std_import = true,
            },
        },
    },
    tl_function_ref = {
        global = {
            system = false,
            configs = {
                modules = true,
                std_import = true,
            },
        },
    },
    frozen = {
        global = {
            system = false,
            configs = {
                modules = true,
                std_import = true,
                cpp = "latest",
            },
        },
    },
    ["vulkan-header"] = {
        global = {
            version = "v1.4.335",
            system = false,
            override = true,
            configs = {
                modules = false,
            },
        },
    },
    ["vulkan-memory-allocator"] = {
        global = {
            version = "v3.3.0",
            system = false,
        },
    },
    -- nzsl = {
    --     windows = {
    --         configs = {
    --             toolchains = "msvc",
    --             runtimes = "MD",
    --         },
    --     },
    --     global = {
    --         override = true,
    --         configs = {
    --             kind = "binary",
    --             fs_watcher = false,
    --         },
    --     },
    -- },
}

function add_requires_with_conf(package)
    local configs = package_configs[package] or {}
    add_requires(
        package,
        table.join(
            global_package_configs,
            configs.global or {},
            is_plat("windows") and configs.windows or {},
            is_plat("linux") and configs.linux or {},
            is_plat("macosx") and configs.macos or {},
            get_config("toolchain") == "llvm" and configs.llvm or {},
            get_config("toolchain") == "msvc" and configs.msvc or {},
            get_config("toolchain") == "gcc" and configs.gcc or {}
        )
    )
end

function add_requires_with_conf_transitive(package)
    local configs = package_configs[package] or {}
    add_requires(
        package,
        table.join(
            global_package_configs,
            configs.global or {},
            is_plat("windows") and configs.windows or {},
            is_plat("linux") and configs.linux or {},
            is_plat("macosx") and configs.macos or {},
            get_config("toolchain") == "llvm" and configs.llvm or {},
            get_config("toolchain") == "msvc" and configs.msvc or {},
            get_config("toolchain") == "gcc" and configs.gcc or {}
        )
    )
    -- add_requireconfs(
    --     package .. ".**",
    --     table.join(
    --         global_package_configs,
    --         configs.global or {},
    --         is_plat("windows") and configs.windows or {},
    --         is_plat("linux") and configs.linux or {},
    --         is_plat("macosx") and configs.macos or {},
    --         get_config("toolchain") == "llvm" and configs.llvm or {},
    --         get_config("toolchain") == "msvc" and configs.msvc or {},
    --         get_config("toolchain") == "gcc" and configs.gcc or {}
    --     )
    -- )
end
