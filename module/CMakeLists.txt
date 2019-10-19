project(StormKit-Module VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCE_FILES
    src/Module.cpp
    src/AbstractModule.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/module/Module.hpp
    include/storm/module/AbstractModule.hpp
)
    
source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/module
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

set(WIN32_SOURCE_FILES
    src/win32/ModuleImpl.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/win32
    PREFIX "Win32 Source Files"
    FILES ${WIN32_SOURCE_FILES}
)

set(WIN32_HEADER_FILES
    src/win32/ModuleImpl.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/win32
    PREFIX "Win32 Header Files"
    FILES ${WIN32_HEADER_FILES}
)

set(POSIX_SOURCE_FILES
    src/posix/ModuleImpl.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/posix
    PREFIX "Posix Source Files"
    FILES ${POSIX_SOURCE_FILES}
)

set(POSIX_HEADER_FILES
    src/posix/ModuleImpl.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/posix
    PREFIX "Posix Header Files"
    FILES ${WIN32_HEADER_FILES}
)

add_library(
    stormkit-module
    ${STORM_OUTPUT_TYPE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    $<$<BOOL:${STORM_OS_WIN32}>:${WIN32_SOURCE_FILES}>
    $<$<BOOL:${STORM_OS_WIN32}>:${WIN32_HEADER_FILES}>
    $<$<BOOL:${STORM_OS_POSIX}>:${POSIX_SOURCE_FILES}>
    $<$<BOOL:${STORM_OS_POSIX}>:${POSIX_HEADER_FILES}>
)

add_dependencies(
	stormkit-module
	StormKit::core
	StormKit::log
)

target_link_libraries(
    stormkit-module
    PUBLIC
        StormKit::core
        StormKit::log
        GSL::GSL
        ${FILESYSTEM_LIBRARIES}
        ${CMAKE_DL_LIBS}
)

target_include_directories(
	stormkit-module
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(
	stormkit-module
    PUBLIC
	PRIVATE 
		STORM_BUILD
)


add_library(StormKit::module ALIAS stormkit-module)


if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-module PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-module)
enable_cpp17(stormkit-module STORM_COMPILER)
append_prefix_and_suffix(stormkit-module)
export_macos_framework("${HEADER_FILES}" image)

