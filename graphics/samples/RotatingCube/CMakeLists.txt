project(RotatingCube VERSION 0.2.0 LANGUAGES C CXX)

set(RESOURCE_FILES
    shaders/vertex.glsl
    shaders/fragment.glsl
)

add_executable(
    rotating_cube
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    ${RESOURCE_FILES}
)

target_link_libraries(
    rotating_cube
    PUBLIC
        StormKit::core
        StormKit::window
        StormKit::render
        StormKit::graphics
)

configure_file(shaders/vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(shaders/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

enable_cpp17(rotating_cube ${STORM_COMPILER})
export_macos_bundle(rotating_cube "${RESOURCE_FILES}" "StormKit RotatingCube sample" "2019 Arthapz")
set(APP "${CMAKE_CURRENT_BINARY_DIR}/rotating_cube.app")
install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APP}\" \"\" \"${CMAKE_CURRENT_BINARY_DIR}\")")

if(${STORM_BUILD_TYPE} EQUAL 1)
    enable_sanitizers(rotating_cube)
endif()
