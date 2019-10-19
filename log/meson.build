dependencies = [core_dep]

headers = files([
    'include/storm/log/Fwd.hpp',
    'include/storm/log/LogHandler.hpp',
    'include/storm/log/Severity.hpp',
    'include/storm/log/Logger.hpp',
    'include/storm/log/Module.hpp',
    'include/storm/log/ConsoleLogger.hpp',
    'include/storm/log/FileLogger.hpp',
    'include/storm/log/LogColorizer.hpp',
])

inls = files([
    'include/storm/log/LogHandler.inl',
    'include/storm/log/Logger.inl',
])

sources = files([
    'src/LogHandler.cpp',
    'src/Logger.cpp',
    'src/ConsoleLogger.cpp',
    'src/FileLogger.cpp',
    'src/LogColorizer.cpp',
])

log_inc = ['include/']

if lib_type == 'shared'
    log_lib_name = 'StormKit-log' + suffix
    log_lib = shared_library(
        log_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: log_inc,
        cpp_args: ['-DSTORM_BUILD'],
        install: true
    )
else
    log_lib_name = 'StormKit-log-static' + suffix
    log_lib = static_library(
        log_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: log_inc,
        cpp_args: ['-DSTORM_BUILD'],
        install: true
    )
endif

log_dep = declare_dependency(
    link_with: log_lib,
    include_directories: log_inc,
    dependencies: dependencies
)

install_headers(headers + inls, subdir : 'storm/log')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Log')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitLogTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitLogTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
