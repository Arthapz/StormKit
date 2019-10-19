cmake_minimum_required(VERSION 3.0...3.14)

function(message)
    if (NOT MESSAGE_QUIET)
        _message(${ARGN})
    endif()
endfunction()

macro(set_common_properties TARGET)
	set_target_properties(${TARGET} PROPERTIES VERSION 0.1.0 SOVERSION 0.1.0)
	set_target_properties(${TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endmacro()

include(CMakeFindDependencyMacro)
include(CMakeDependentOption)
include(FeatureSummary)

option(STORM_ENABLE_DOC                    "Build doc" ON)
option(STORM_ENABLE_EXAMPLES               "Build examples" ON)
option(STORM_ENABLE_CORE                   "Build storm core library" ON)
cmake_dependent_option(STORM_ENABLE_SHARED "Build shared libraries" ON
    "NOT STORM_ENABLE_STATIC" OFF)
cmake_dependent_option(STORM_ENABLE_STATIC "Build static libraries" ON
    "NOT STORM_ENABLE_SHARED" OFF)
cmake_dependent_option(STORM_ENABLE_LOG "Build storm log library" ON
    "STORM_ENABLE_CORE" ON)
cmake_dependent_option(STORM_ENABLE_ENTITIES "Build storm entities library" ON
    "STORM_ENABLE_CORE" ON)
cmake_dependent_option(STORM_ENABLE_WINDOW "Build storm window library" ON
    "STORM_ENABLE_CORE;STORM_ENABLE_LOG" ON)
cmake_dependent_option(STORM_ENABLE_IMAGE "Build storm image library" ON
    "STORM_ENABLE_CORE;STORM_ENABLE_LOG" ON)
cmake_dependent_option(STORM_ENABLE_MODULE "Build storm module library" ON
    "STORM_ENABLE_CORE;STORM_ENABLE_LOG" ON)
cmake_dependent_option(STORM_ENABLE_SERIALIZE "Build storm serializer library" ON
    "STORM_ENABLE_CORE;STORM_ENABLE_LOG" ON)
cmake_dependent_option(STORM_ENABLE_RENDER "Build storm render library" ON
    "STORM_ENABLE_CORE;STORM_ENABLE_LOG;STORM_ENABLE_SERIALIZE" ON)
cmake_dependent_option(STORM_ENABLE_RENDER_WINDOW "Build storm render window support" ON
    "STORM_ENABLE_RENDER;STORM_ENABLE_WINDOW" ON)
cmake_dependent_option(STORM_ENABLE_GRAPHICS "Build storm graphics library" ON
    "STORM_ENABLE_RENDER" ON)


#for IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

project(StormKit VERSION 0.1.0 LANGUAGES CXX)

set_package_properties(StormKit PROPERTIES
    URL "http://arthapz.stormkit.fr"
    DESCRIPTION "StormKit C++ ToolKit"
    PURPOSE "Some usefull libraries to make multimedia application")

set(STORM_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(STORM_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(STORM_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(STORM_MILESTONE     "alpha")
set(STORM_BUILD_TYPE    0)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(STORM_BUILD_TYPE 1)
endif()
if(STORM_ENABLE_SHARED)
    set(STORM_OUTPUT_TYPE SHARED)
elseif(STORM_ENABLE_STATIC)
    set(STORM_OUTPUT_TYPE STATIC)
endif()

include(cmake/Tools/SystemDetection.cmake)
include(cmake/Tools/Git.cmake)
include(cmake/Tools/CopyImportedTarget.cmake)
include(cmake/Tools/ExportMacro.cmake)
include(cmake/Tools/EnableModernCPP.cmake)

if(STORM_OS_MACOS)
	set(STORM_ENABLE_RENDER_PLUGIN_GLES OFF)
endif()

message(STATUS "-- -- -- Configuring StormKit project -- -- --")
message(STATUS "-- -- Detecting Git revision -- --")

detect_git_version()
detect_git_branch()

set(STORM_GIT_BRANCH  ${GIT_COMMIT_BRANCH})
set(STORM_GIT_COMMIT_HASH ${GIT_COMMIT_HASH})

message(STATUS "-- -- Detecting OS -- --")

detect_os()

message(STATUS "-- -- Checking Compiler -- --")

detect_cpp_compiler()
detect_cpp_types()
detect_cpp_headers()

message("-- -- STATUS -- --")

string(REGEX MATCH "\\.el[1-9]" os_version_suffix ${CMAKE_SYSTEM})

macro(RESIZE_ON INPUT OUTPUT)
    set(${OUTPUT} "OFF")
    if(${INPUT})
        set(${OUTPUT} "ON ")
    endif()
endmacro()

RESIZE_ON(STORM_ENABLE_SHARED STORM_ENABLE_SHARED_PRINTABLE)
RESIZE_ON(STORM_ENABLE_STATIC STORM_ENABLE_STATIC_PRINTABLE)
RESIZE_ON(STORM_ENABLE_EXAMPLES STORM_ENABLE_EXAMPLES_PRINTABLE)
RESIZE_ON(STORM_ENABLE_CORE STORM_ENABLE_CORE_PRINTABLE)
RESIZE_ON(STORM_ENABLE_LOG STORM_ENABLE_LOG_PRINTABLE)
RESIZE_ON(STORM_ENABLE_ENTITIES STORM_ENABLE_ENTITIES_PRINTABLE)
RESIZE_ON(STORM_ENABLE_WINDOW STORM_ENABLE_WINDOW_PRINTABLE)
RESIZE_ON(STORM_ENABLE_IMAGE STORM_ENABLE_IMAGE_PRINTABLE)
RESIZE_ON(STORM_ENABLE_MODULE STORM_ENABLE_MODULE_PRINTABLE)
RESIZE_ON(STORM_ENABLE_SERIALIZE STORM_ENABLE_SERIALIZE_PRINTABLE)
RESIZE_ON(STORM_ENABLE_RENDER STORM_ENABLE_RENDER_PRINTABLE)
RESIZE_ON(STORM_ENABLE_RENDER_PLUGIN_VULKAN STORM_ENABLE_RENDER_PLUGIN_VULKAN_PRINTABLE)
RESIZE_ON(STORM_ENABLE_RENDER_PLUGIN_GLES STORM_ENABLE_RENDER_PLUGIN_GLES_PRINTABLE)
RESIZE_ON(STORM_ENABLE_RENDER_WINDOW STORM_ENABLE_RENDER_WINDOW_PRINTABLE)
RESIZE_ON(STORM_ENABLE_GRAPHICS STORM_ENABLE_GRAPHICS_PRINTABLE)

message("-- MAJOR_VERSION:    ${STORM_MAJOR_VERSION}")
message("-- MINOR_VERSION:    ${STORM_MINOR_VERSION}")
message("-- PATCH_VERSION:    ${STORM_PATCH_VERSION}")
message("-- MILESTONE:        ${STORM_MILESTONE}")
message("-- BUILD_TYPE:       ${STORM_BUILD_TYPE}")
message("-- OUTPUT_TYPE:      ${STORM_OUTPUT_TYPE}")
message("-- COMMIT_HASH:      ${STORM_GIT_COMMIT_HASH}")
message("-- BRANCH:           ${STORM_GIT_BRANCH}")
message("-- OS:               ${STORM_OS}")
message("-- SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message("-- SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message("-- SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- SYSTEM:           ${CMAKE_SYSTEM}")
message("-- COMPILER:         ${STORM_COMPILER}")
message("------------------------------------------------------")
message("|STORM_ENABLE_DOC                  |     ${STORM_ENABLE_DOC_PRINTABLE}|")
message("|STORM_ENABLE_SHARED               |     ${STORM_ENABLE_SHARED_PRINTABLE}|")
message("|STORM_ENABLE_STATIC               |     ${STORM_ENABLE_STATIC_PRINTABLE}|")
message("|STORM_ENABLE_EXAMPLES             |     ${STORM_ENABLE_EXAMPLES_PRINTABLE}|")
message("|STORM_ENABLE_CORE                 |     ${STORM_ENABLE_CORE_PRINTABLE}|")
message("|STORM_ENABLE_LOG                  |     ${STORM_ENABLE_LOG_PRINTABLE}|")
message("|STORM_ENABLE_ENTITIES             |     ${STORM_ENABLE_ENTITIES_PRINTABLE}|")
message("|STORM_ENABLE_WINDOW               |     ${STORM_ENABLE_WINDOW_PRINTABLE}|")
message("|STORM_ENABLE_IMAGE                |     ${STORM_ENABLE_IMAGE_PRINTABLE}|")
message("|STORM_ENABLE_MODULE               |     ${STORM_ENABLE_MODULE_PRINTABLE}|")
message("|STORM_ENABLE_SERIALIZE            |     ${STORM_ENABLE_SERIALIZE_PRINTABLE}|")
message("|STORM_ENABLE_RENDER               |     ${STORM_ENABLE_RENDER_PRINTABLE}|")
message("|STORM_ENABLE_RENDER_PLUGIN_VULKAN |     ${STORM_ENABLE_RENDER_PLUGIN_VULKAN_PRINTABLE}|")
message("|STORM_ENABLE_RENDER_PLUGIN_GLES   |     ${STORM_ENABLE_RENDER_PLUGIN_GLES_PRINTABLE}|")
message("|STORM_ENABLE_RENDER_WINDOW        |     ${STORM_ENABLE_RENDER_WINDOW_PRINTABLE}|")
message("|STORM_ENABLE_GRAPHICS             |     ${STORM_ENABLE_GRAPHICS_PRINTABLE}|")
message("-------------------------------------------------------")

message("-- -- Detecting Global Dependencies -- --")
find_dependency(Threads QUIET REQUIRED)
if(TARGET Threads::Threads)
        message("-- Thread Found")
else()
        message(FATAL "Missing Thread library")
endif()

if(STORM_OS_LINUX)
        set(FILESYSTEM_LIBRARIES -lstdc++fs)
endif()


if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/GSL/include")
    message(STATUS "Downloading external/GSL deps via git submodule")
	clone_git_submodule(external/GSL)
endif()

add_library(GSL::GSL IMPORTED INTERFACE)
target_include_directories(GSL::GSL INTERFACE "${PROJECT_SOURCE_DIR}/external/GSL/include")

if(STORM_ENABLE_CORE)
    add_subdirectory(core)
endif()

if(STORM_ENABLE_LOG)
    add_subdirectory(log)
endif()

if(STORM_ENABLE_ENTITIES)
    add_subdirectory(entities)
endif()

if(STORM_ENABLE_WINDOW)
    add_subdirectory(window)
endif()

if(STORM_ENABLE_IMAGE)
    add_subdirectory(image)
endif()

if(STORM_ENABLE_MODULE)
    add_subdirectory(module)
endif()

if(STORM_ENABLE_RENDER)
    set(SPIRV_CROSS_CLI            OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_SHARED         OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_STATIC         ON CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_HLSL    OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_MSL     OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_ENABLE_C_API   OFF CACHE BOOL "" FORCE)
    set(SPIRV_CROSS_SKIP_INSTALL   OFF CACHE BOOL "" FORCE)

    set(SKIP_GLSLANG_INSTALL    ON  CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS       OFF  CACHE BOOL "" FORCE)
    set(BUILD_TESTING           OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTERNAL          ON  CACHE BOOL "" FORCE)
    set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
    set(ENABLE_HLSL             OFF CACHE BOOL "" FORCE)
    set(ENABLE_GLSLANG_WEB      OFF CACHE BOOL "" FORCE)

    set(SPIRV_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
    set(SKIP_SPIRV_TOOLS_INSTALL ON CACHE BOOL "" FORCE)
    set(SPIRV_SKIP_TESTS ON CACHE BOOL "" FORCE)

    set(MESSAGE_QUIET ON)
    add_subdirectory(external/glslang)
    add_subdirectory(external/spv-cross)
    add_subdirectory(external/volk)
    unset(MESSAGE_QUIET)

    add_subdirectory(render)
endif()

if(STORM_ENABLE_GRAPHICS)
    add_subdirectory(graphics)
endif()

if(EDO)
    add_subdirectory(edo)
endif()
