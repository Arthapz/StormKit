xcb_dep = dependency('xcb')
xcb_key_dep = dependency('xcb-keysyms')
xcb_icccm_dep = dependency('xcb-icccm')
xcb_randr_dep = dependency('xcb-randr')

private_dependencies = [window_dep, xcb_dep, xcb_key_dep, xcb_icccm_dep, xcb_randr_dep]

sources += files([
    'src/InputHandlerImpl.cpp',
    'src/WindowImpl.cpp',
    'src/Plugin.cpp',
    'src/Utils.cpp',
    'src/InputHandlerImpl.hpp',
    'src/WindowImpl.hpp',
    'src/Utils.hpp',
])

x11_plugin = shared_library(
    'StormKit-window-x11' + suffix,
    sources,
    dependencies: private_dependencies,
    install: true
)
