project(StormKit-Core VERSION 0.1.0 LANGUAGES C CXX)

message("-- -- Detecting Storm::Core Dependencies -- --")
find_dependency(fmt QUIET REQUIRED)
if(TARGET fmt::fmt AND TARGET fmt::fmt-header-only)
    message("-- {fmt} Found")
else()
    message(FATAL_ERROR "Fatal: Missing {fmt} library")
endif()

add_library(glm INTERFACE IMPORTED)
set_target_properties(glm PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/../external/glm"
)

if(TARGET glm)
    message("-- GLM Found")
else()
    message(FATAL_ERROR "Fatal: Missing GLM library")
endif()

set(SOURCE_FILES
    src/State.cpp
    src/StateManager.cpp
    src/TreeNode.cpp
    src/Math.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/core/ForwardDeclarations.hpp
    include/storm/core/App.hpp
    include/storm/core/Assert.hpp
    include/storm/core/CRTP.hpp
    include/storm/core/Filesystem.hpp
    include/storm/core/Flags.hpp
    include/storm/core/Hash.hpp
    include/storm/core/Memory.hpp
    include/storm/core/NamedType.hpp
    include/storm/core/NonCopyable.hpp
    include/storm/core/NonDefaultInstanciable.hpp
    include/storm/core/NonMovable.hpp
    include/storm/core/Math.hpp
    include/storm/core/Numerics.hpp
    include/storm/core/Numerics.inl
    include/storm/core/Platform.hpp
    include/storm/core/Singleton.hpp
    include/storm/core/Strings.hpp
    include/storm/core/Pimpl.hpp
    include/storm/core/Pimpl.inl
    include/storm/core/Timer.hpp
    include/storm/core/Timer.inl
    include/storm/core/Types.hpp
    include/storm/core/TypeTraits.hpp
    include/storm/core/Reference.hpp
    include/storm/core/Reference.inl
    include/storm/core/Observer.hpp
    include/storm/core/ResourcesPool.hpp
    include/storm/core/State.hpp
    include/storm/core/StateManager.hpp
    include/storm/core/Subject.hpp
    include/storm/core/Subject.inl
    include/storm/core/Tree.hpp
    include/storm/core/Tree.inl
    include/storm/core/TreeNode.hpp
    include/storm/core/TreeNode.inl
    include/storm/core/RingBuffer.hpp
    include/storm/core/RingBuffer.inl
    include/storm/core/Span.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/generated/include/storm/core/Configure.hpp
    include/storm/core/private/Flags.hpp
    include/storm/core/private/incbin.h
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/core
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

add_library(
    stormkit-core
    ${STORM_OUTPUT_TYPE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_link_libraries(
    stormkit-core
    PUBLIC
        GSL::GSL
        ${FILESYSTEM_LIBRARIES}
        fmt::fmt
        fmt::fmt-header-only
        glm
)

target_include_directories(
    stormkit-core
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/include>
)

target_compile_definitions(
    stormkit-core
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::core ALIAS stormkit-core)

set_common_properties(stormkit-core)
enable_cpp17(stormkit-core "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-core)

foreach(HEADER_FILE ${HEADER_FILES})
    string(REPLACE "/Users/arthapz/Dev/build/debug/stormkit/core/generated/" "" HEADER_FILE_STRIPPED ${HEADER_FILE})
    file(RELATIVE_PATH HEADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_FILE_STRIPPED}")
    get_filename_component(HEADER_PATH "${HEADER_PATH}"
                           DIRECTORY)
    set_property(
        SOURCE
            ${HEADER_FILE}
        PROPERTY
            MACOSX_PACKAGE_LOCATION "Headers/${HEADER_PATH}"
    )
endforeach()

set_target_properties(
    stormkit-core
    PROPERTIES
        FRAMEWORK                             TRUE
        FRAMEWORK_VERSION                     A
        MACOSX_FRAMEWORK_IDENTIFIER           org.arthapz.stormkit.core
        MACOSX_FRAMEWORK_BUNDLE_VERSION       ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}"
        VERSION                               ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
        SOVERSION                             ${STORM_MAJOR_VERSION}.${STORM_MINOR_VERSION}.${STORM_PATCH_VERSION}
)


configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/Configure.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/include/storm/core/Configure.hpp
)

#export_target(stormkit-core Core)
