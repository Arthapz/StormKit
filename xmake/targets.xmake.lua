modules = {
    core = {
        public_packages = { "frozen", "unordered_dense", "tl_function_ref" },
        modulename = "core",
        has_headers = true,
        custom = function()
            if is_plat("windows") then add_packages("wil") end

            set_configdir("$(builddir)/.gens/include/")
            add_configfiles("include/(stormkit/core/config.hpp.in)")
            add_headerfiles("$(builddir)/.gens/include/(stormkit/core/*.hpp)")
            -- add_cxflags("clang::-Wno-language-extension-token")

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
        end,
    },
    test = {
        modulename = "test",
        public_deps = { "core" },
        has_headers = true,
    },
    log = {
        modulename = "log",
        public_deps = table.join("core", get_config("luau") and "luau" or {}),
        public_packages = get_config("luau") and { "luau", "luabridge3" } or {},
        has_headers = true,
    },
    entities = {
        modulename = "entities",
        public_deps = table.join("core", get_config("luau") and "luau" or {}),
        public_packages = get_config("luau") and { "luau", "luabridge3" } or {},
    },
    image = {
        modulename = "image",
        packages = { "libktx", "libpng", "libjpeg-turbo" },
        public_deps = table.join("core", get_config("luau") and "luau" or {}),
        public_packages = get_config("luau") and { "luau", "luabridge3" } or {},
    },
    main = {
        modulename = "main",
        has_headers = true,
        deps = { "core" },
        custom = function()
            add_cxflags("-Wno-main")
            set_strip("debug")
        end,
        frameworks = is_plat("macosx") and { "CoreFoundation" } or nil,
    },
    luau = get_config("luau") and {
        modulename = "luau",
        has_headers = true,
        public_deps = { "core" },
        public_packages = { "luau", "luabridge3" },
        public_defines = { "STORMKIT_LUA_BINDING", 'LUA_API=extern __attribute__((visibility("default")))' },
    } or nil,
    wsi = {
        modulename = "wsi",
        public_deps = table.join("core", get_config("luau") and "luau" or {}),
        deps = { "log" },
        public_packages = get_config("luau") and { "luau", "luabridge3" } or {},
        packages = is_plat("linux") and {
            "libxcb",
            "xcb-util-keysyms",
            "xcb-util",
            "xcb-util-image",
            "xcb-util-wm",
            "xcb-util-errors",
            "wayland",
            "wayland-protocols",
            "libxkbcommon",
        } or {},
        -- frameworks = is_plat("macosx") and { "CoreFoundation", "Foundation", "AppKit", "Metal", "IOKit", "QuartzCore" }
        -- or nil,
        custom = function()
            if is_plat("macosx", "iphoneos", "tvos") then
                if is_plat("macosx") then
                    add_files("src/wsi/macos/swift/*.swift", { public = true })
                    add_files("src/wsi/macos/swift/*.m")
                    set_values("swift.modulename", "macOS")
                    add_scflags("-Isrc/wsi/macos/swift")
                elseif is_plat("iphoneos") then
                    add_files("src/wsi/ios/swift/*.swift", { public = true })
                    add_scflags("-Isrc/wsi/ios/swift")
                    set_values("swift.modulename", "iOS")
                elseif is_plat("tvos") then
                    add_files("src/wsi/tvos/swift/*.swift", { public = true })
                    add_scflags("-Isrc/wsi/tvos/swift")
                    set_values("swift.modulename", "tvOS")
                end
                set_values("swift.interop", "cxx")
            elseif is_plat("linux") then
                add_rules("wayland.protocols")

                on_load(function(target)
                    assert(target:pkg("wayland-protocols"))
                    local wayland_protocols_dir =
                        path.join(target:pkg("wayland-protocols"):installdir() or "/usr", "share", "wayland-protocols")
                    assert(wayland_protocols_dir, "wayland protocols directory not found")

                    local protocols = {
                        path.join("stable", "xdg-shell", "xdg-shell.xml"),
                        path.join("stable", "tablet", "tablet-v2.xml"),
                        path.join("stable", "viewporter", "viewporter.xml"),
                        path.join("staging", "content-type", "content-type-v1.xml"),
                        path.join("staging", "pointer-warp", "pointer-warp-v1.xml"),
                        path.join("staging", "cursor-shape", "cursor-shape-v1.xml"),
                        path.join("staging", "single-pixel-buffer", "single-pixel-buffer-v1.xml"),
                        path.join("unstable", "xdg-decoration", "xdg-decoration-unstable-v1.xml"),
                        path.join("unstable", "pointer-constraints", "pointer-constraints-unstable-v1.xml"),
                        path.join("unstable", "relative-pointer", "relative-pointer-unstable-v1.xml"),
                    }

                    for _, protocol in ipairs(protocols) do
                        target:add("files", path.join(wayland_protocols_dir, protocol))
                    end
                end)
            elseif is_plat("windows", "mingw") then
                add_syslinks("User32", "Shell32", "Gdi32", "Shcore", "Gdiplus")
            end
        end,
    },
    gpu = {
        modulename = "gpu",
        has_headers = true,
        public_packages = table.join({
            "frozen",
            "volk",
            "vulkan-headers",
            "vulkan-memory-allocator",
        }, get_config("luau") and { "luau", "luabridge3" } or {}),
        public_deps = table.join("core", "wsi", "image", get_config("luau") and "luau" or {}),
        deps = { "log" },
        packages = is_plat("linux") and {
            "libxcb",
            "wayland",
        } or nil,
        public_defines = {
            "STORMKIT_GPU_VULKAN",
        },
        custom = function() add_cxflags("clang::-Wno-missing-declarations") end,
    },
}
