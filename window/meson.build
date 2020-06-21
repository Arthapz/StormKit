public_dependencies = [core_dep, module_dep]
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
])

#ide:editable-filelist
inls = files([
    'include/storm/window/InputHandler.inl',
    'include/storm/window/EventHandler.inl',
    'include/storm/window/Window.inl',
    'include/storm/window/AbstractWindow.inl',
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
        sources + headers,
        dependencies: public_dependencies + private_dependencies,
        include_directories: window_inc,
        install: true
    )
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources + headers,
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
