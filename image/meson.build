if is_windows
   png_dep = dependency('PNG', fallback: ['libpng', 'png_dep'])
else 
   png_dep = dependency('libpng', fallback: ['libpng', 'png_dep'])
endif

if is_windows
    jpeg_dep = dependency('JPEG', fallback: ['libjpeg', 'jpeg_dep'])
else 
    jpeg_dep = dependency('libjpeg', fallback: ['libjpeg', 'jpeg_dep'])
endif

public_dependencies = [core_dep]
private_dependencies = [png_dep, jpeg_dep]

#ide:editable-filelist
headers = files([
    'include/storm/image/Fwd.hpp',
    'include/storm/image/Image.hpp',
])

#ide:editable-filelist
inls = files([
])

#ide:editable-filelist
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
