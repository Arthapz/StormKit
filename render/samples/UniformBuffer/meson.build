dependencies = [core_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process('shaders/vertex.vert.glsl', 'shaders/fragment.frag.glsl')

sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

executable(
    'stormkit_UniformBuffer',
    [sources, shader_files],
    dependencies: dependencies,
    cpp_args: examples_arguments,
    install: true
)
