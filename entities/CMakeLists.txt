project(StormKit-Entities VERSION 0.2.0 LANGUAGES C CXX)

set(SOURCE_FILES
    src/EntityManager.cpp
    src/System.cpp
    src/Component.cpp
    src/MessageBus.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/entities/Fwd.hpp
    include/storm/entities/Component.hpp
    include/storm/entities/Entity.hpp
    include/storm/entities/EntityManager.hpp
    include/storm/entities/EntityManager.inl
    include/storm/entities/System.hpp
    include/storm/entities/MessageBus.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/entities
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

add_library(
    stormkit-entities
    ${STORM_OUTPUT_TYPE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

add_dependencies(
	stormkit-entities
        StormKit::core
)

target_link_libraries(
    stormkit-entities
    PUBLIC
        StormKit::core
        GSL::GSL
        ${FILESYSTEM_LIBRARIES}
)

target_include_directories(
    stormkit-entities
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/core/generated/include>
)

target_compile_definitions(
    stormkit-entities
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::entities ALIAS stormkit-entities)

if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-entities PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-entities)
enable_cpp17(stormkit-entities "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-entities)
export_macos_framework("${HEADER_FILES}" entities)
#export_target(stormkit-entities Entities)

