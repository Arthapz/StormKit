project(StormKit-Image VERSION 0.1.0 LANGUAGES C CXX)

message("-- -- Detecting Storm::Image Dependencies -- --")

find_dependency(PNG QUIET REQUIRED)
if(TARGET PNG::PNG)
    message("-- PNG Found")
else()
    message(FATAL_ERROR "Fatal: Missing PNG library")
endif()

find_dependency(TURBOJPEG QUIET REQUIRED)
if(TARGET TURBOJPEG::TURBOJPEG)
    message("-- TURBOJPEG Found")
else()
    message(FATAL_ERROR "Fatal: Missing TURBOJPEG library")
endif()

set(SOURCE_FILES
    src/Image.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(PRIVATE_SOURCE_FILES
    src/private/ImageLoader.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/private/
    PREFIX "Private Source Files"
    FILES ${PRIVATE_SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/image/Image.hpp
    include/storm/image/ForwardDeclarations.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/image
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

set(PRIVATE_HEADER_FILES
    src/private/ImageData.hpp
	src/private/ImageLoader.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/private/
    PREFIX "Private Header Files"
    FILES ${PRIVATE_HEADER_FILES}
)

add_library(
    stormkit-image
    ${STORM_OUTPUT_TYPE}
    ${SOURCE_FILES}
    ${PRIVATE_SOURCE_FILES}
    ${HEADER_FILES}
    ${PRIVATE_HEADER_FILES}
)

add_dependencies(
	stormkit-image
	StormKit::core
	StormKit::log
)

target_link_libraries(
    stormkit-image
    PUBLIC
        StormKit::core
        StormKit::log
        PNG::PNG
        TURBOJPEG::TURBOJPEG
		GSL::GSL
		${FILESYSTEM_LIBRARIES}
)

target_include_directories(
    stormkit-image
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(
    stormkit-image
    PUBLIC
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::image ALIAS stormkit-image)

if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-image PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-image)
enable_cpp17(stormkit-image "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-image)
export_macos_framework("${HEADER_FILES}" image)


