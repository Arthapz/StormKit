project(Triangle VERSION 0.2.0 LANGUAGES C CXX)

set(RESOURCE_FILES
    shaders/vertex.glsl
    shaders/fragment.glsl
)

add_executable(
    triangle
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    ${RESOURCE_FILES}
)

target_link_libraries(
    triangle
    PUBLIC
        StormKit::core
        StormKit::window
        StormKit::render
        StormKit::log
)


configure_file(shaders/vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(shaders/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

enable_cpp17(triangle ${STORM_COMPILER})
export_macos_bundle(triangle "${RESOURCE_FILES}" "StormKit Triangle sample" "2019 Arthapz")

set(APP "${CMAKE_CURRENT_BINARY_DIR}/triangle.app")

copy_imported_targets(
    triangle
    StormKit::core
    StormKit::window
    StormKit::render
    StormKit::log
    StormKit::image
)

install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APP}\" \"\" \"${CMAKE_CURRENT_BINARY_DIR}\")")

if(${STORM_BUILD_TYPE} EQUAL 1)
    enable_sanitizers(triangle)
endif()
