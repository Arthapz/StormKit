if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)

set(CMAKE_IMPORT_FILE_VERSION 1)

set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget StormKit::Core StormKit::Engine StormKit::Entities StormKit::Image StormKit::Log StormKit::Log StormKit::Module StormKit::Render StormKit::Window)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)

get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

add_library(StormKit::Core SHARED IMPORTED)
target_include_directories(
    StormKit::Core INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Engine SHARED IMPORTED)
target_include_directories(
    StormKit::Engine INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Entities SHARED IMPORTED)
target_include_directories(
    StormKit::Entities INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Image SHARED IMPORTED)
target_include_directories(
    StormKit::Image INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Log SHARED IMPORTED)
target_include_directories(
    StormKit::Log INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Main STATIC IMPORTED)
target_include_directories(
    StormKit::Main INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Module SHARED IMPORTED)
target_include_directories(
    StormKit::Module INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Render SHARED IMPORTED)
target_include_directories(
    StormKit::Render INTERFACE "${_IMPORT_PREFIX}/include")

add_library(StormKit::Window SHARED IMPORTED)
target_include_directories(
    StormKit::Window INTERFACE "${_IMPORT_PREFIX}/include")


if(CMAKE_VERSION VERSION_LESS 3.0.0)
  message(FATAL_ERROR "This file relies on consumers using CMake 3.0.0 or greater.")
endif()

get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/StormKitSharedTargets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

set(_IMPORT_PREFIX)

foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

add_library(StormKit::StormKit IMPORTED)

target_link_libraries(
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)



