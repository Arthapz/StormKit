compiler = meson.get_compiler('cpp')

dl_dep = compiler.find_library('dl', required: false)

dependencies = [core_dep, dl_dep]

headers = files([
    'include/storm/module/Fwd.hpp',
    'include/storm/module/Module.hpp',
    'include/storm/module/AbstractModule.hpp',
])

inls = files([
    'include/storm/module/Module.inl',
    'include/storm/module/AbstractModule.inl',
])

sources = files([
    'src/Module.cpp',
    'src/AbstractModule.cpp',
])

if is_windows
    sources += files([
        'src/win32/ModuleImpl.hpp',
        'src/win32/ModuleImpl.cpp',
    ])
else
    sources += files([
        'src/posix/ModuleImpl.hpp',
        'src/posix/ModuleImpl.cpp',
    ])
endif

module_inc = ['include/']

if lib_type == 'shared'
    module_lib_name = 'StormKit-module' + suffix
    module_lib = shared_library(
        module_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: module_inc,
        install: true
    )
else
    module_lib_name = 'StormKit-module-static' + suffix
    module_lib = static_library(
        module_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: module_inc,
        install: true
    )
endif

module_dep = declare_dependency(
    link_with: module_lib,
    include_directories: module_inc,
    dependencies: dependencies
)

install_headers(headers + inls, subdir : 'storm/module')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Module')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core ${CMAKE_DL_LIBS}')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitModuleTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitModuleTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
