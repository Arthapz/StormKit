local global_package_configs = {
    configs = {
        shared = get_config("shared_deps"),
        ["x11"] = is_plat("linux"),
        wayland = is_plat("linux"),
        modules = true,
        std_import = true,
        cpp = "latest",
    },
}

if get_config("on_ci") then global_package_configs.system = false end

add_requireconfs("nzsl.fmt", { version = "master", override = true, system = false })
if not is_plat("windows") then add_requires("libdwarf") end

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
    libktx = {
        llvm = {
            configs = {
                cxflags = "-Wno-overriding-option",
            },
        },
    },
    frozen = {
        global = {
            system = false,
        },
    },
    ["vulkan-header"] = {
        global = {
            version = "v1.4.309",
            system = false,
        },
    },
    ["vulkan-memory-allocator"] = {
        global = {
            version = "v3.3.0",
            system = false,
        },
    },
    nzsl = {
        linux = {
            configs = {
                toolchains = "gcc",
                runtimes = "stdc++_shared",
            },
        },
        windows = {
            configs = {
                toolchains = "msvc",
                runtimes = "MD",
            },
        },
        global = {
            override = true,
            configs = {
                fs_watcher = false,
                link = {},
            },
        },
    },
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
    add_requireconfs(
        package .. ".**",
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
