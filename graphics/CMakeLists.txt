project(StormKit-Graphics VERSION 0.2.0 LANGUAGES C CXX)

set(SOURCE_FILES
        src/core/Vertex.cpp
        src/core/DefaultShaders.cpp
        src/core/Material.cpp
        src/core/Renderer.cpp
        src/core/Mesh.cpp
        src/core/MeshCache.cpp

        src/framegraph/FrameGraphBuilder.cpp
        src/framegraph/FrameGraph.cpp
        src/framegraph/FramePass.cpp
        src/framegraph/FrameGraphResources.cpp
        src/framegraph/FramePassBuilder.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
        include/storm/graphics/core/Fwd.hpp
        include/storm/graphics/core/DefaultShaders.hpp
        include/storm/graphics/core/Vertex.hpp
        include/storm/graphics/core/Material.hpp
        include/storm/graphics/core/Renderer.hpp
        include/storm/graphics/core/Mesh.hpp
        include/storm/graphics/core/MeshCache.hpp

        include/storm/graphics/framegraph/FrameGraphBuilder.hpp
        include/storm/graphics/framegraph/FrameGraphBuilder.inl
        include/storm/graphics/framegraph/FrameGraph.hpp
        include/storm/graphics/framegraph/FramePass.hpp
        include/storm/graphics/framegraph/FramePass.inl
        include/storm/graphics/framegraph/FrameGraphResources.hpp
        include/storm/graphics/framegraph/FramePassBuilder.hpp
        include/storm/graphics/framegraph/Fwd.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/graphics
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

add_library(
    stormkit-graphics
    ${STORM_OUTPUT_TYPE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

add_dependencies(
	stormkit-graphics
	StormKit::core
	StormKit::render
	StormKit::log
        StormKit::entities
)

target_link_libraries(
    stormkit-graphics
    PUBLIC
        StormKit::core
        StormKit::render
        StormKit::log
        StormKit::entities
        GSL::GSL
        ${FILESYSTEM_LIBRARIES}
)

target_include_directories(
    stormkit-graphics
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/core/generated/include>
)

target_compile_definitions(
    stormkit-graphics
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::graphics ALIAS stormkit-graphics)

if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-graphics PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-graphics)
enable_cpp17(stormkit-graphics "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-graphics)
export_macos_framework("${HEADER_FILES}" graphics)

if(STORM_ENABLE_EXAMPLES)
    add_subdirectory("samples/RotatingCube")
endif()
