project(Texture VERSION 0.2.0 LANGUAGES C CXX)

add_executable(
    obj
    src/main.cpp
    src/tiny_obj_loader.h
)

target_link_libraries(
    obj
    PUBLIC
        StormKit::core
        StormKit::window
        StormKit::render
		StormKit::image
)

target_compile_definitions(
    obj
    PUBLIC
        TINYOBJLOADER_IMPLEMENTATION
)

configure_file(shaders/vertex.spv ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.spv COPYONLY)
configure_file(shaders/fragment.spv ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.spv COPYONLY)
configure_file(shaders/vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertex.glsl COPYONLY)
configure_file(shaders/fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragment.glsl COPYONLY)
file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

enable_cpp17(obj)
copy_imported_targets(obj 
    StormKit::core
    StormKit::window
    StormKit::module
    StormKit::render
	StormKit::image
)

copy_imported_plugins(obj
    stormkit-render-vulkan
)

if(${STORM_BUILD_TYPE} EQUAL 1)
    enable_sanitizers(obj)
endif()
