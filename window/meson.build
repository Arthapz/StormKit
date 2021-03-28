public_dependencies = [core_dep, module_dep, log_dep]
private_dependencies = []

#ide:editable-filelist
headers = files([
    'include/storm/window/InputHandler.hpp',
    'include/storm/window/AbstractInputHandler.hpp',
    'include/storm/window/EventHandler.hpp',
    'include/storm/window/Window.hpp',
    'include/storm/window/AbstractWindow.hpp',
    'include/storm/window/WindowStyle.hpp',
    'include/storm/window/Key.hpp',
    'include/storm/window/Event.hpp',
    'include/storm/window/MouseButton.hpp',
    'include/storm/window/VideoSettings.hpp',
    'include/storm/window/Fwd.hpp',

    'include/storm/window/InputHandler.inl',
    'include/storm/window/EventHandler.inl',
    'include/storm/window/Window.inl',
    'include/storm/window/AbstractWindow.inl',
])

private_headers = files([
    'src/Log.hpp'
])

#ide:editable-filelist
sources = files([
    'src/Window.cpp',
    'src/AbstractWindow.cpp',
    'src/EventHandler.cpp',
    'src/InputHandler.cpp',
    'src/AbstractInputHandler.cpp',
])

window_inc = ['include/']

if lib_type == 'shared'
    window_lib_name = 'StormKit-window' + suffix
    window_lib = shared_library(
        window_lib_name,
        sources,
        extra_files: headers + private_headers,
        dependencies: public_dependencies + private_dependencies,
        include_directories: window_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources,
        extra_files: headers + private_headers,
        dependencies: public_dependencies + private_dependencies,
        include_directories: window_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/lib/')
endif
window_copy_target = custom_target(
    'window_copy_target',
    input: window_lib,
    output: 'window-fake',
    command: [copy_file, '@INPUT@', destination_path],
    install: false,
    build_by_default: true
)

window_dep = declare_dependency(
    link_with: window_lib,
    include_directories: window_inc,
    dependencies: public_dependencies
)

subdir('plugins')
if get_option('enable_examples')
    subdir('samples')
endif

install_headers(headers, subdir : 'storm/window')
