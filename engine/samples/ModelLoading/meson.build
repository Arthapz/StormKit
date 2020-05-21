dependencies = [core_dep, engine_dep, render_dep, window_dep, main_dep]

sources = files([
    'src/main.cpp',
    'src/App.cpp',
    'src/App.hpp',
    'src/GLTFScene.cpp',
    'src/GLTFScene.hpp'
])

executable(
    'ModelLoading',
    sources,
    dependencies: dependencies,
    cpp_args: examples_arguments,
    install: true
)
