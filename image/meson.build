png_dep = dependency('libpng', method: 'pkg-config', required: false)

if not png_dep.found()
    png_dep = dependency('PNG', method: 'cmake', required: false)
    if not png_dep.found()
        png_dep = dependency('libpng', fallback: ['libpng', 'png_dep'])
    endif
endif

jpeg_dep = dependency('libjpeg-turbo')

public_dependencies = [core_dep]
private_dependencies = [png_dep, jpeg_dep]

headers = files([
    'include/storm/image/Fwd.hpp',
    'include/storm/image/Image.hpp',
])

inls = files([
])

sources = files([
    'src/Image.cpp',
    'src/private/ImageLoader.cpp',
    'src/private/ImageData.hpp',
    'src/private/ImageLoader.hpp',
])

image_inc = ['include/']

if lib_type == 'shared'
    image_lib_name = 'StormKit-image' + suffix
    image_lib = shared_library(
        image_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: image_inc,
        install: true
    )
else
    image_lib_name = 'StormKit-image-static' + suffix
    image_lib = static_library(
        image_lib_name,
        sources,
        headers,
        extra_files: inls,
        gnu_symbol_visibility: 'hidden',
        dependencies: public_dependencies + private_dependencies,
        include_directories: image_inc,
        install: true
    )
endif

image_dep = declare_dependency(
    link_with: image_lib,
    dependencies: public_dependencies,
    include_directories: image_inc,
)

install_headers(headers + inls, subdir : 'storm/image')

conf = configuration_data()
conf.set_quoted('PACKAGE_LIBRARY_NAME', 'Image')
conf.set_quoted('PACKAGE_LIBRARY_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))
conf.set_quoted('PACKAGE_INCLUDE_PATH', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
conf.set('PACKAGE_STORMKIT_DEPENDENCIES', 'StormKit::Core')
conf.set_quoted('PACKAGE_DEPENDENCIES', '')

configure_file(input : '../StormKitTarget.cmake.in',
               output : 'StormKitImageTarget.cmake',
               configuration : conf)

install_data(meson.current_build_dir() / 'StormKitImageTarget.cmake', install_dir: get_option('libdir') / 'cmake/StormKit')
