private_dependencies = [window_dep]

sources += files([
    'src/WindowImpl.cpp',
    'src/Plugin.cpp',
    'src/Utils.cpp',
    'src/InputHandlerImpl.cpp',
    'src/WindowImpl.hpp',
    'src/Utils.hpp',
    'src/InputHandlerImpl.hpp'
])

win32_plugin = shared_library(
    'StormKit-window-win32' + suffix,
    sources,
    dependencies: private_dependencies,
    install: true
)
