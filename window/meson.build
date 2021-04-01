public_dependencies = [core_dep, module_dep, log_dep]
private_dependencies = []

#ide:editable-filelist
public_headers = files([
    'include/storm/window/Event.hpp',
    'include/storm/window/EventHandler.hpp',
    'include/storm/window/Fwd.hpp',
    'include/storm/window/Mouse.hpp',
    'include/storm/window/Keyboard.hpp',
    'include/storm/window/Key.hpp',
    'include/storm/window/MouseButton.hpp',
    'include/storm/window/VideoSettings.hpp',
    'include/storm/window/Window.hpp',
    'include/storm/window/WindowStyle.hpp',

    'include/storm/window/EventHandler.inl',
    'include/storm/window/Mouse.inl',
    'include/storm/window/Keyboard.inl',
    'include/storm/window/Window.inl',
])

private_headers = files([
    'src/Log.hpp',
    'src/AbstractWindow.hpp',
    'src/AbstractWindow.inl',
])

x11_headers = files([
    'src/Linux/X11/Log.hpp',
    'src/Linux/X11/Utils.hpp',
    'src/Linux/X11/X11Mouse.hpp',
    'src/Linux/X11/X11Keyboard.hpp',
    'src/Linux/X11/X11Window.hpp',
])

#ide:editable-filelist
sources = files([
    'src/AbstractWindow.cpp',
    'src/EventHandler.cpp',
    'src/Window.cpp',
])

x11_sources = files([
    'src/Linux/X11/Utils.cpp',
    'src/Linux/X11/X11Mouse.cpp',
    'src/Linux/X11/X11Keyboard.cpp',
    'src/Linux/X11/X11Window.cpp',
])

headers = public_headers + private_headers + x11_headers
extra_files = headers

if is_linux
    sources += x11_sources

    xcb_dep           = dependency('xcb')
    xcb_key_dep       = dependency('xcb-keysyms')
    xcb_icccm_dep     = dependency('xcb-icccm')
    xcb_randr_dep     = dependency('xcb-randr')
    xcb_xkb_dep       = dependency('xcb-xkb')
    xkbcommon_dep     = dependency('xkbcommon')
    xkbcommon_x11_dep = dependency('xkbcommon-x11')

    private_dependencies += [xcb_dep, xcb_key_dep, xcb_icccm_dep, xcb_randr_dep, xcb_xkb_dep, xkbcommon_dep, xkbcommon_x11_dep]
else
    extra_files += x11_sources
endif

dependencies = public_dependencies + private_dependencies

window_inc = ['include/']

if lib_type == 'shared'
    window_lib_name = 'StormKit-window' + suffix
    window_lib = shared_library(
        window_lib_name,
        sources,
        extra_files: extra_files,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: window_inc,
        install: true
    )
    destination_path = join_paths(meson.project_build_root(), 'output/bin/')
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources,
        extra_files: extra_files,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
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

if get_option('enable_examples')
    subdir('samples')
endif

install_headers(public_headers, subdir : 'storm/window')
