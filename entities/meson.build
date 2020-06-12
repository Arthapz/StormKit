dependencies = [core_dep]

#ide:editable-filelist
headers = files([
    'include/storm/entities/Fwd.hpp',
    'include/storm/entities/Component.hpp',
    'include/storm/entities/Entity.hpp',
    'include/storm/entities/EntityManager.hpp',
    'include/storm/entities/System.hpp',
    'include/storm/entities/MessageBus.hpp',
])

#ide:editable-filelist
inls = files([
    'include/storm/entities/EntityManager.inl',
])

#ide:editable-filelist
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
        sources + headers,
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
        sources + headers,
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
