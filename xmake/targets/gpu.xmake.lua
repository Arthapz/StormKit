local src_gpu_dir = path.join(src_dir, "gpu")
local module_gpu_dir = path.join(module_dir, "gpu")

target("gpu", function()
    set_kind("$(kind)")
    add_rules("flags")

    set_languages("cxxlatest", "clatest")

    add_defines("STORMKIT_GPU_BUILD", { public = false })

    add_files(path.join(module_dir, "gpu.cppm"), path.join(module_gpu_dir, "**.cppm"), { public = true })
    add_files(path.join(src_gpu_dir, "**.cpp"))
    -- add_files(path.join(src_gpu_dir, "*.cpp"), path.join(src_gpu_dir, "*.cppm"))

    add_headerfiles(path.join(include_dir, "(stormkit/gpu/**.hpp)"))
    add_includedirs(include_dir, { public = true })

    add_defines("STORMKIT_GPU_VULKAN", { public = true })

    add_deps("core", "wsi", "image")
    add_deps("log", { public = false })

    add_packages("volk", "vulkan-headers", "vulkan-memory-allocator")

    add_options("sanitizers")

    set_group("libraries")
end)
