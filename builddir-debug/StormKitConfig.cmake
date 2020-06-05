
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was /Users/arthurlaurent/Developpement/repos/stormkit/StormKitConfig.cmake.in ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

####################################################################################


if (NOT StormKit_FIND_COMPONENTS)
    message(FATAL_ERROR "find_package(StormKit) called with no component")
endif()

set(FIND_STORMKIT_CORE_DEPENDENCIES "")
set(FIND_STORMKIT_ENGINE_DEPENDENCIES core render log)
set(FIND_STORMKIT_ENTITIES_DEPENDENCIES core)
set(FIND_STORMKIT_IMAGE_DEPENDENCIES core)
set(FIND_STORMKIT_LOG_DEPENDENCIES core)
set(FIND_STORMKIT_MAIN_DEPENDENCIES "")
set(FIND_STORMKIT_MODULE_DEPENDENCIES core)
set(FIND_STORMKIT_RENDER_DEPENDENCIES core log image window module)
set(FIND_STORMKIT_WINDOW_DEPENDENCIES core module)

foreach(component ${StormKit_FIND_COMPONENTS})
    string(TOUPPER "${component}" UPPER_COMPONENT)
    list(APPEND FIND_STORMKIT_ADDITIONAL_COMPONENTS ${FIND_STORMKIT_${UPPER_COMPONENT}_DEPENDENCIES})
endforeach()
list(APPEND StormKit_FIND_COMPONENTS ${FIND_STORMKIT_ADDITIONAL_COMPONENTS})
list(REMOVE_DUPLICATES StormKit_FIND_COMPONENTS)

if(STORMKIT_STATIC_LIBRARIES)
    include(${CMAKE_CURRENT_LIST_DIR}/StormKitStaticTargets.cmake)
else()
    include(${CMAKE_CURRENT_LIST_DIR}/StormKitSharedTargets.cmake)
endif()

foreach(comp ${StormKit_FIND_COMPONENTS})
  if(NOT TARGET StormKit::${comp})
    if(StormKit_FIND_REQUIRED_${comp})
      set(StormKit_FOUND FALSE)
    endif()
  endif()
endforeach(comp)
