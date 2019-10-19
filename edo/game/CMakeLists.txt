project(EDO VERSION 0.1.0 LANGUAGES C CXX)

set(EDO_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(EDO_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(EDO_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(EDO_BUILD_TYPE    0)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(EDO_BUILD_TYPE 1)
endif()

include(../../cmake/Tools/Git.cmake)

detect_git_version()
detect_git_branch()

set(EDO_GIT_BRANCH  ${GIT_COMMIT_BRANCH})
set(EDO_GIT_COMMIT_HASH ${GIT_COMMIT_HASH})

set(RESOURCE_FILES
    shaders/sprite_vertex.glsl
    shaders/sprite_fragment.glsl
    shaders/map_vertex.glsl
    shaders/map_fragment.glsl

    textures/character.png
    textures/Overworld.png
)

add_executable(
    edo
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/EdoApplication.cpp
    src/EdoApplication.hpp
    src/Common.hpp
    src/Tilemap.hpp

    src/ecs/RenderSystem.cpp
    src/ecs/RenderSystem.hpp
    src/ecs/MovementSystem.cpp
    src/ecs/MovementSystem.hpp
    src/ecs/CameraComponent.hpp
    src/ecs/StaticGeometryComponent.hpp
    src/ecs/CustomShaderDataComponent.hpp
    src/ecs/TransformComponent.hpp
    src/ecs/TextureComponent.hpp
    src/ecs/Archetypes.cpp
    src/ecs/Archetypes.hpp

    ${CMAKE_CURRENT_BINARY_DIR}/generated/Configure.hpp

    ${RESOURCE_FILES}
)

target_link_libraries(
    edo
    PRIVATE
        StormKit::core
        StormKit::window
        StormKit::render
        StormKit::graphics
        StormKit::entities
)

target_include_directories(
    edo
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
)

configure_file(shaders/sprite_vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/sprite_vertex.glsl COPYONLY)
configure_file(shaders/sprite_fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/sprite_fragment.glsl COPYONLY)
configure_file(shaders/map_vertex.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/map_vertex.glsl COPYONLY)
configure_file(shaders/map_fragment.glsl ${CMAKE_CURRENT_BINARY_DIR}/shaders/map_fragment.glsl COPYONLY)
configure_file(textures/character.png ${CMAKE_CURRENT_BINARY_DIR}/textures/character.png COPYONLY)
configure_file(textures/Overworld.png ${CMAKE_CURRENT_BINARY_DIR}/textures/Overworld.png COPYONLY)

enable_cpp17(edo ${STORM_COMPILER})
export_macos_bundle(edo "${RESOURCE_FILES}" "EDO" "2019 Arthapz")
set(APP "${CMAKE_CURRENT_BINARY_DIR}/edo.app")
install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APP}\" \"\" \"${CMAKE_CURRENT_BINARY_DIR}\")")

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/Configure.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/Configure.hpp
)
