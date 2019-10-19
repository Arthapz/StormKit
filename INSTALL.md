# Installation

A C++17 complient (C++20 prefered) compilator is needed to build StormKit

## Dependencies
### StormKit-core module
- [{fmt}](https://fmt.dev/latest/index.html) (system-wide, or downloaded by StormKit)

### StormKit-image module
- [LibPNG >= 1.6](http://www.libpng.org/pub/png/libpng.html) (system-wide, or downloaded by StormKit)
- [LibJPEG-turbo](https://www.libjpeg-turbo.org) (system-wide, or downloaded by StormKit)

### StormKit-render module
- [Vulkan](https://vulkan.lunarg.com) (system-wide)

### StormKit-engine module
- [Assimp](https://github.com/assimp/assimp) (system-wide)

## Building
### Windows
First you need to [download](https://vulkan.lunarg.com/sdk/home#sdk/downloadConfirm/latest/windows/vulkan-sdk.exe) and set an env var (VULKAN_SDK) to its root directory
```
set VULKAN_SDK=C:\Dev\VulkanSDK\1.1.121.2
vcpkg install --triplet x64-windows vulkan-sdk libpng libjpeg-turbo fmt assimp

#Configure the project
## Dynamic library
meson builddir -Ddefault_library=shared
## Static library
meson builddir -Ddefault_library=static

#Build the project
ninja -C builddir
```

### Linux / macOS
```
#Configure the project
## Dynamic library
meson builddir -Ddefault_library=shared
## Static library
meson builddir -Ddefault_library=static

#Build the project
ninja -C builddir
``` 

## Meson parameters
You can customize your build (with -Doption=value) with the following parameters 

|       Variable        |         Description          |                                   Default value                                       |
|-----------------------|------------------------------|---------------------------------------------------------------------------------------|
| enable_doc            | Generate documentation       | false                                                                                 |
| enable_examples       | Build examples               | false                                                                                 |
| enable_entities       | Build storm entities library | true                                                                                  |
| enable_log            | Build storm log library      | true                                                                                  |
| enable_window         | Build storm window library   | true                                                                                  |
| enable_image          | Build storm image library    | true                                                                                  |
| enable_module         | Build storm module library   | true                                                                                  |
| enable_render         | Build storm render library   | true                                                                                  |
| enable_engine       | Build storm engine library | true                                                                                  |