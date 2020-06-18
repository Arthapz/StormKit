dependencies = [core_dep, engine_dep, render_dep, window_dep, main_dep]

directory = 'stormkit/ModelLoading/'

arguments = []
if meson.get_compiler('cpp').get_argument_syntax() == 'msvc'
    arguments += ['/DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '/DSTORM_ENABLE_VALIDATION_LAYERS']
else
    arguments += ['-DEXAMPLES_DATA_DIR="' + examples_data_dir / directory + '"', '-DSTORM_ENABLE_VALIDATION_LAYERS']
endif

arguments += examples_arguments

#ide:editable-filelist
sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp',
    'src/GLTFScene.cpp',
    'src/GLTFScene.hpp'
])

executable(
    'stormkit_ModelLoading',
    sources,
    dependencies: dependencies,
    cpp_args: arguments,
    install: true
)

install_subdir('models/', install_dir: get_option('datadir') / directory)
install_subdir('textures/', install_dir: get_option('datadir') / directory)
