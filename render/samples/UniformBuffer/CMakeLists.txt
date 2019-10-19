project(UniformBuffer VERSION 0.2.0 LANGUAGES C CXX)

set(RESOURCE_FILES
    shaders/vertex.glsl
    shaders/fragment.glsl
)

add_executable(
    uniform_buffer
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    ${RESOURCE_FILES}
)

target_link_libraries(
    uniform_buffer
    PUBLIC
        StormKit::core
        StormKit::window
        StormKit::render
        StormKit::log
)


configure_file(shaders/vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(shaders/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.glsl COPYONLY)

enable_cpp17(uniform_buffer ${STORM_COMPILER})
export_macos_bundle(uniform_buffer "${RESOURCE_FILES}" "StormKit uniform buffer sample" "2019 Arthapz")
set(APP "${CMAKE_CURRENT_BINARY_DIR}/uniform_buffer.app")
install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APP}\" \"\" \"${CMAKE_CURRENT_BINARY_DIR}\")")

copy_imported_targets(
    uniform_buffer
    StormKit::core
    StormKit::window
    StormKit::render
    StormKit::log
    StormKit::image
)

if(${STORM_BUILD_TYPE} EQUAL 1)
    enable_sanitizers(uniform_buffer)
endif()
