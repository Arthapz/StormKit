dependencies = []

headers = files([
    'include/storm/main/Main.hpp',
])

inls = files([
    'include/storm/main/Main.hpp',
])

sources = files([])
args = {}

if is_windows
    sources += files([
        'src/win32/Main.cpp',
    ])
elif is_linux
    sources += files([
        'src/linux/Main.cpp',
    ])
elif is_macos
    add_languages('objc')
    add_languages('objcpp')
    sources += files([
        'src/macOS/Main.mm',
    ])
    #args += {'cpp_args': '-x objective-c++'}

    cocoa_dep = dependency('Cocoa')

    dependencies += [cocoa_dep]
elif is_ios
    sources += files([
        'src/iOS/Main.mm',
    ])
    args += {'cpp_args': '-x objective-c++'}
endif

main_inc = ['include/']

args += {
    'dependencies': dependencies,
    'include_directories': main_inc
}

main_lib = static_library(
    'StormKit-main' + suffix, 
    sources, 
    headers, 
    kwargs: args,
    install: true
)

main_dep = declare_dependency(
    link_with: main_lib,
    include_directories: main_inc,
    dependencies: dependencies
)

install_headers(headers + inls, subdir : 'storm/main')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Main')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', '')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : 'StormKitTarget.cmake.in',
               output : 'StormKitMainTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitMainTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')