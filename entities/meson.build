dependencies = [core_dep]

headers = files([
    'include/storm/entities/Fwd.hpp',
    'include/storm/entities/Component.hpp',
    'include/storm/entities/Entity.hpp',
    'include/storm/entities/EntityManager.hpp',
    'include/storm/entities/System.hpp',
    'include/storm/entities/MessageBus.hpp',
])

inls = files([
    'include/storm/entities/EntityManager.inl',
])

sources = files([
    'src/EntityManager.cpp',
    'src/System.cpp',
    'src/Component.cpp',
    'src/MessageBus.cpp',
])

entities_inc = include_directories('include/')

if lib_type == 'shared'
    entities_lib_name = 'StormKit-entities' + suffix
    entities_lib = shared_library(
        entities_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: entities_inc,
        install: true
    )
else
    entities_lib_name = 'StormKit-entities-static' + suffix
    entities_lib = static_library(
        entities_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: dependencies,
        include_directories: entities_inc,
        install: true
    )
endif

entities_dep = declare_dependency(
    link_with: entities_lib, 
    include_directories: entities_inc,
    dependencies: dependencies
)

install_headers(headers + inls, subdir : 'storm/entities')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Entities')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitEntitiesTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitEntitiesTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
