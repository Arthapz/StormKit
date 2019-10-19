public_dependencies = [core_dep]
private_dependencies = []

headers = files([
    'include/storm/window/InputHandler.hpp',
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
    'include/storm/window/EventHandler.inl',
    'include/storm/window/Window.inl',
    'include/storm/window/AbstractWindow.inl',
])

sources = files([
    'src/Window.cpp',
    'src/AbstractWindow.cpp',
    'src/EventHandler.cpp',
    'src/InputHandler.cpp',
])

args = {}
if is_windows
    sources += files([
        'src/win32/WindowImpl.cpp',
        'src/win32/VideoSettings.cpp',
        'src/win32/Utils.cpp',
        'src/win32/InputHandlerImpl.cpp',
        'src/win32/WindowImpl.hpp',
        'src/win32/Utils.hpp',
        'src/win32/InputHandlerImpl.hpp'
    ])
    args += {'cpp_args': '-DSTORM_BUILD'}
elif is_linux
    sources += files([
        'src/x11/VideoSettings.cpp',
        'src/x11/InputHandlerImpl.cpp',
        'src/x11/WindowImpl.cpp',
        'src/x11/Utils.cpp',
        'src/x11/InputHandlerImpl.hpp',
        'src/x11/WindowImpl.hpp',
        'src/x11/Utils.hpp',
    ])

    xcb_dep = dependency('xcb')
    xcb_key_dep = dependency('xcb-keysyms')
    xcb_icccm_dep = dependency('xcb-icccm')
    xcb_randr_dep = dependency('xcb-randr')

    private_dependencies += [xcb_dep, xcb_key_dep, xcb_icccm_dep, xcb_randr_dep]
    args += {'cpp_args': ['-DSTORM_BUILD', '-DSTORM_X11']}
elif is_macos
    add_languages('objc')
    add_languages('objcpp')
    sources += files([
        'src/macOS/StormApplication.m',
        'src/macOS/StormApplicationDelegate.m',
        'src/macOS/StormWindow.m',
        'src/macOS/AutoReleasePoolWrapper.mm',
        'src/macOS/StormView.mm',
        'src/macOS/StormWindowController.mm',
        'src/macOS/Utils.mm',
        'src/macOS/WindowImpl.mm',
        'src/macOS/VideoSettings.mm',
        'src/macOS/InputHandlerImpl.mm',
        'src/macOS/StormApplication.h',
        'src/macOS/StormApplicationDelegate.h',
        'src/macOS/StormWindow.h',
        'src/macOS/AutoReleasePoolWrapper.h',
        'src/macOS/StormView.h',
        'src/macOS/StormWindowController.h',
        'src/macOS/Utils.h',
        'src/macOS/WindowImpl.h',
        'src/macOS/InputHandlerImpl.h',
    ])

    cocoa_dep = dependency('Cocoa')
    quartzcore_dep = dependency('QuartzCore')
    iokit_dep = dependency('IOKit')
    carbon_dep = dependency('Carbon')
    private_dependencies += [cocoa_dep, quartzcore_dep, iokit_dep, carbon_dep]

    args += {'objcpp_args': '-std=c++2a', 'objc_args': '-std=c11', 'cpp_args': '-DSTORM_BUILD'}
elif is_ios
    sources += files([
        'src/iOS/View.m',
        'src/iOS/ViewController.m',
        'src/iOS/WindowImpl.mm',
        'src/iOS/InputHandlerImpl.mm',
        'src/iOS/AppDelegate.mm',
        'src/iOS/VideoSettings.mm',
        'src/iOS/iosMain.cpp',
        'src/iOS/View.h',
        'src/iOS/ViewController.h'    ,
        'src/iOS/WindowImpl.hpp',
        'src/iOS/AppDelegate.hpp',
        'src/iOS/InputHandlerImpl.hpp',
        'src/iOS/iosMain.hpp',
    ])
    args += {'objcpp_args': '-std=c++2a', 'objc_args': '-std=c11', 'cpp_args': '-DSTORM_BUILD'}
endif

window_inc = ['include/']
args += {
    'dependencies': public_dependencies + private_dependencies,
    'include_directories': window_inc
}

if lib_type == 'shared'
    window_lib_name = 'StormKit-window' + suffix
    window_lib = shared_library(
        window_lib_name,
        sources,
        headers,
        kwargs: args,
        install: true
    )
else
    window_lib_name = 'StormKit-window-static' + suffix
    window_lib = static_library(
        window_lib_name,
        sources,
        headers,
        kwargs: args,
        install: true
    )
endif

window_dep = declare_dependency(
    link_with: window_lib,
    include_directories: window_inc,
    dependencies: public_dependencies
)

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
