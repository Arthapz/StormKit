public_dependencies = [core_dep, module_dep]
private_dependencies = []

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
])

inls = files([
    'include/storm/window/InputHandler.inl',
    'include/storm/window/EventHandler.inl',
    'include/storm/window/Window.inl',
    'include/storm/window/AbstractWindow.inl',
])

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
        headers,
        dependencies: public_dependencies + private_dependencies,
        include_directories: window_inc,
        install: true
    )
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources,
        headers,
        dependencies: public_dependencies + private_dependencies,
        include_directories: window_inc,
        install: true
    )
endif

window_dep = declare_dependency(
    link_with: window_lib,
    include_directories: window_inc,
    dependencies: public_dependencies
)

subdir('plugins')
if get_option('enable_examples')
    subdir('samples')
endif

install_headers(headers + inls, subdir : 'storm/window')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Window')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitWindowTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitWindowTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
