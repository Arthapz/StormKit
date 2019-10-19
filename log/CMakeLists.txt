project(StormKit-Log VERSION 0.1.0 LANGUAGES C CXX)


set(SOURCE_FILES
    src/LogHandler.cpp
    src/Logger.cpp
    src/ConsoleLogger.cpp
    src/FileLogger.cpp
    src/LogColorizer.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/log/ForwardDeclarations.hpp
    include/storm/log/LogHandler.hpp
    include/storm/log/LogHandler.inl
    include/storm/log/Severity.hpp
    include/storm/log/Logger.hpp
    include/storm/log/Logger.inl
    include/storm/log/Module.hpp
    include/storm/log/ConsoleLogger.hpp
    include/storm/log/FileLogger.hpp
    include/storm/log/LogColorizer.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/log
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

add_library(
    stormkit-log
    ${STORM_OUTPUT_TYPE}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

add_dependencies(
	stormkit-log
	StormKit::core
)

target_link_libraries(
    stormkit-log
    PUBLIC
        StormKit::core
        GSL::GSL
        ${FILESYSTEM_LIBRARIES}
)

target_include_directories(
    stormkit-log
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/core/generated/include>
)

target_compile_definitions(
    stormkit-log
    PUBLIC
    PRIVATE
        STORM_BUILD
)

add_library(StormKit::log ALIAS stormkit-log)

if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-log PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-log)
enable_cpp17(stormkit-log "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-log)
export_macos_framework("${HEADER_FILES}" log)
#export_target(stormkit-log Log)

if(STORM_ENABLE_EXAMPLES)
    add_subdirectory(examples/console_logger)
    add_subdirectory(examples/file_logger)
endif()

