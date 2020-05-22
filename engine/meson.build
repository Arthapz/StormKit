cmake = import('cmake')

static = 'OFF'
if lib_type == 'static'
    static = 'ON'
endif

buildtype = 'Debug'
if build_type == 'release'
    buildtype = 'Release'
endif

public_dependencies = [core_dep, render_dep, log_dep]
private_dependencies = []

prog_glslang = find_program('glslangValidator')
command_glslang = ['--target-env', 'vulkan1.0', '-V', '-x', '-o', '@OUTPUT@', '@INPUT@']

shader_compiler = generator(
    prog_glslang,
    output: '@BASENAME@.spv.hpp',
    arguments: command_glslang
)

shader_files = shader_compiler.process(
    'shaders/PBRColorPass.vert.glsl',
    'shaders/PBRColorPass.frag.glsl',
    'shaders/CubeMap.vert.glsl',
    'shaders/CubeMap.frag.glsl',
)

base_headers = files([
    'include/storm/engine/Engine.hpp',
])

base_inls = files([
    'include/storm/engine/Engine.inl',
])

core_headers = files([
    'include/storm/engine/core/Fwd.hpp',
    'include/storm/engine/core/Bindable.hpp',
    'include/storm/engine/core/DebugGUI.hpp',
    'include/storm/engine/core/Profiler.hpp',
    'include/storm/engine/core/RingHardwareBuffer.hpp',
    'include/storm/engine/core/Transform.hpp',
    'include/storm/engine/core/Vertex.hpp',
])

core_inls = files([
    'include/storm/engine/core/Bindable.inl',
    'include/storm/engine/core/DebugGUI.inl',
    'include/storm/engine/core/Profiler.inl',
    'include/storm/engine/core/RingHardwareBuffer.inl',
    'include/storm/engine/core/Transform.inl',
    'include/storm/engine/core/Vertex.inl',
])

material_headers = files([
    'include/storm/engine/material/Fwd.hpp',
    'include/storm/engine/material/Material.hpp',
    'include/storm/engine/material/MaterialInstance.hpp',
    'include/storm/engine/material/PBRMaterial.hpp',
    'include/storm/engine/material/PBRMaterialInstance.hpp',
    'include/storm/engine/material/CubeMapMaterial.hpp',
    'include/storm/engine/material/CubeMapMaterialInstance.hpp',
])

material_inls = files([
    'include/storm/engine/material/Material.inl',
    'include/storm/engine/material/MaterialInstance.inl',
    'include/storm/engine/material/PBRMaterial.inl',
    'include/storm/engine/material/PBRMaterialInstance.inl',
    'include/storm/engine/material/CubeMapMaterial.inl',
    'include/storm/engine/material/CubeMapMaterialInstance.inl',
])

scene_headers = files([
    'include/storm/engine/scene/Fwd.hpp',
    'include/storm/engine/scene/Camera.hpp',
    'include/storm/engine/scene/FPSCamera.hpp',
    'include/storm/engine/scene/Scene.hpp',
])

scene_inls = files([
    'include/storm/engine/scene/Camera.inl',
    'include/storm/engine/scene/FPSCamera.inl',
    'include/storm/engine/scene/Scene.inl',
])

drawable_headers = files([
    'include/storm/engine/drawable/Fwd.hpp',
    'include/storm/engine/drawable/Drawable.hpp',
    'include/storm/engine/drawable/Mesh.hpp',
    'include/storm/engine/drawable/SubMesh.hpp',
    'include/storm/engine/drawable/Model.hpp',
    'include/storm/engine/drawable/CubeMap.hpp',
])

drawable_inls = files([
    'include/storm/engine/drawable/Drawable.inl',
    'include/storm/engine/drawable/Mesh.inl',
    'include/storm/engine/drawable/SubMesh.inl',
    'include/storm/engine/drawable/Model.inl',
])

framegraph_headers = files([
    'include/storm/engine/framegraph/Fwd.hpp',
    'include/storm/engine/framegraph/FrameGraph.hpp',
    'include/storm/engine/framegraph/FramePass.hpp',
    'include/storm/engine/framegraph/FramePassBuilder.hpp',
    'include/storm/engine/framegraph/FramePassResource.hpp',
    'include/storm/engine/framegraph/FramePassResourceHandle.hpp',
    'include/storm/engine/framegraph/FramePassResources.hpp',
])

framegraph_inls = files([
    'include/storm/engine/framegraph/FrameGraph.inl',
    'include/storm/engine/framegraph/FramePass.inl',
    'include/storm/engine/framegraph/FramePassBuilder.inl',
    'include/storm/engine/framegraph/FramePassResource.inl',
    'include/storm/engine/framegraph/FramePassResourceHandle.inl',
    'include/storm/engine/framegraph/FramePassResources.inl',
])

sources = files([
    'src/Engine.cpp', 
    'src/Tinyobj.cpp',
    'src/stb_image.c',
    'src/stb_image_write.c',

    'src/core/imgui/imconfig.hpp',
    'src/core/imgui/imgui_internal.hpp',
    'src/core/imgui/imgui_widgets.cpp',
    'src/core/imgui/imgui_stdlib.cpp',
    'src/core/imgui/imgui_stdlib.hpp',
    'src/core/imgui/imgui_draw.cpp',
    'src/core/imgui/imgui.cpp',
    'src/core/imgui/imgui.hpp',
    'src/core/imgui/imgui_impl_vulkan.hpp',
    'src/core/imgui/imgui_impl_vulkan.cpp',
    'src/core/imgui/imstb_rectpack.hpp',
    'src/core/imgui/imstb_textedit.hpp',
    'src/core/imgui/imstb_truetype.hpp',
    'src/core/imgui/imgui_demo.cpp',
    'src/core/imgui/imgui_widget_flamegraph.cpp',
    'src/core/imgui/imgui_widget_flamegraph.hpp',

    'src/core/Bindable.cpp',
    'src/core/DebugGUI.cpp',
    'src/core/Profiler.cpp',
    'src/core/RingHardwareBuffer.cpp',
    'src/core/Transform.cpp',
    'src/core/Vertex.cpp',

    'src/material/Material.cpp',
    'src/material/MaterialInstance.cpp',
    'src/material/PBRMaterial.cpp',
    'src/material/PBRMaterialInstance.cpp',
    'src/material/CubeMapMaterial.cpp',
    'src/material/CubeMapMaterialInstance.cpp',

    'src/scene/Camera.cpp',
    'src/scene/FPSCamera.cpp',
    'src/scene/Scene.cpp',

    'src/drawable/Drawable.cpp',
    'src/drawable/Mesh.cpp',
    'src/drawable/SubMesh.cpp',
    'src/drawable/Model.cpp',
    'src/drawable/CubeMap.cpp',

    'src/framegraph/FrameGraph.cpp',
    'src/framegraph/FramePass.cpp',
    'src/framegraph/FramePassBuilder.cpp',
    'src/framegraph/FramePassResource.cpp',
    'src/framegraph/FramePassResourceHandle.cpp',
    'src/framegraph/FramePassResources.cpp',
])

headers = base_headers + core_headers + material_headers + scene_headers + drawable_headers + framegraph_headers

engine_inc = ['include/']

if lib_type == 'shared'
    engine_lib_name = 'StormKit-engine' + suffix
    engine_lib = shared_library(
        engine_lib_name,
        sources,
        headers,
        shader_files,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: engine_inc,
        install: true
    )
else
    engine_lib_name = 'StormKit-engine-static' + suffix
    engine_lib = static_library(
        engine_lib_name,
        sources,
        headers,
        shader_files,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: engine_inc,
        install: true
    )
endif

engine_dep = declare_dependency(
    link_with: engine_lib,
    dependencies: public_dependencies,
    include_directories: engine_inc,
)

if get_option('enable_examples')
    subdir('samples')
endif

install_headers(base_headers + base_inls, subdir : 'storm/engine')
install_headers(core_headers + core_inls, subdir : 'storm/engine/core')
install_headers(material_headers + material_inls, subdir : 'storm/engine/material')
install_headers(scene_headers + scene_inls, subdir : 'storm/engine/scene')
install_headers(drawable_headers + drawable_inls, subdir : 'storm/engine/drawable')
install_headers(framegraph_headers + framegraph_inls, subdir : 'storm/engine/framegraph')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Engine')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core StormKit::Render StormKit::Log')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitEngineTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitEngineTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
