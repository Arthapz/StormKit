dependencies = [core_dep, render_dep, window_dep, main_dep]

shader_files = shader_compiler.process('shaders/vertex.vert.glsl', 'shaders/fragment.frag.glsl')

directory = 'stormkit/Texture/'

arguments = []
if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    arguments += ['/DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"']
else
    arguments += ['-DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"']
endif

arguments += examples_arguments

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp'
])

executable(
    'stormkit_Texture',
    [sources, shader_files],
    dependencies: dependencies,
    cpp_args: arguments,
    install: true
)

install_subdir('textures/', install_dir: get_option('datadir') / directory)
