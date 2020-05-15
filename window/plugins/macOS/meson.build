add_languages('objc')
add_languages('objcpp')

cocoa_dep = dependency('Cocoa')
quartzcore_dep = dependency('QuartzCore')
iokit_dep = dependency('IOKit')
carbon_dep = dependency('Carbon')

private_dependencies = [window_dep, cocoa_dep, quartzcore_dep, iokit_dep, carbon_dep]

sources += files([
    'src/StormApplication.m',
    'src/StormApplicationDelegate.m',
    'src/StormWindow.m',
    'src/AutoReleasePoolWrapper.mm',
    'src/StormView.mm',
    'src/StormWindowController.mm',
    'src/Utils.mm',
    'src/WindowImpl.mm',
    'src/Plugin.mm',
    'src/InputHandlerImpl.mm',
    'src/StormApplication.h',
    'src/StormApplicationDelegate.h',
    'src/StormWindow.h',
    'src/AutoReleasePoolWrapper.h',
    'src/StormView.h',
    'src/StormWindowController.h',
    'src/Utils.h',
    'src/WindowImpl.h',
    'src/InputHandlerImpl.h',
])

macos_plugin = shared_library(
    'StormKit-window-macOS' + suffix,
    sources,
    dependencies: private_dependencies,
    objc_args: ['-std=c11'],
    objcpp_args: ['-std=c++2a'],
    install: true
)

