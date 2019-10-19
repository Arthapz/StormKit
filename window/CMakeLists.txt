project(StormKit-Window VERSION 0.2.0 LANGUAGES C CXX)

message("-- -- Detecting Storm::Window Dependencies -- --")

if(STORM_OS_WIN32)
elseif(STORM_OS_MACOS)
    find_library(APPKIT_LIBRARY Cocoa)
    if(APPKIT_LIBRARY)
        message("-- Cocoa Found")
    else()
        message(FATAL_ERROR "Fatal: Missing Cocoa library")
    endif()
    find_library(QUARTZ_CORE_LIBRARY QuartzCore)
    if(QUARTZ_CORE_LIBRARY)
        message("-- QuartzCore Found")
    else()
        message(FATAL_ERROR "Fatal: Missing QuartzCore library")
    endif()

    find_library(IOKIT_LIBRARY IOKit)
    if(IOKIT_LIBRARY)
        message("-- IOKit Found")
    else()
        message(FATAL_ERROR "Fatal: Missing IOKit library")
    endif()

    find_library(CARBON_LIBRARY Carbon)
    if(CARBON_LIBRARY)
        message("-- CARBON Found")
    else()
        message(FATAL_ERROR "Fatal: Missing CARBON library")
    endif()

    mark_as_advanced(APPKIT_LIBRARY)
elseif(STORM_OS_LINUX)
    find_dependency(XCB QUIET REQUIRED COMPONENTS XCB KEYSYMS ATOM ICCCM RANDR)
    if(TARGET XCB::XCB AND TARGET XCB::KEYSYMS AND TARGET XCB::ATOM AND TARGET XCB::ICCCM AND TARGET XCB::RANDR)
        message("-- XCB Found")
    else()
        message(FATAL_ERROR "Fatal: Missing XCB library")
    endif()
endif()

set(SOURCE_FILES
    src/Window.cpp
    src/AbstractWindow.cpp
    src/EventHandler.cpp
    src/InputHandler.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

set(HEADER_FILES
    include/storm/window/InputHandler.hpp
    include/storm/window/EventHandler.hpp
    include/storm/window/Window.hpp
    include/storm/window/AbstractWindow.hpp
    include/storm/window/WindowStyle.hpp
    include/storm/window/Key.hpp
    include/storm/window/Event.hpp
    include/storm/window/MouseButton.hpp
    include/storm/window/VideoSettings.hpp
    include/storm/window/ForwardDeclarations.hpp
)
    
source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/include/storm/window
    PREFIX "Header Files"
    FILES ${HEADER_FILES}
)

set(WIN32_SOURCE_FILES
    src/win32/WindowImpl.cpp
    src/win32/VideoSettings.cpp
    src/win32/Utils.cpp
    src/win32/InputHandlerImpl.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/win32
    PREFIX "Win32 Source Files"
    FILES ${WIN32_SOURCE_FILES}
)

set(WIN32_HEADER_FILES
    src/win32/WindowImpl.hpp
    src/win32/Utils.hpp
    src/win32/InputHandlerImpl.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/win32
    PREFIX "Win32 Header Files"
    FILES ${WIN32_HEADER_FILES}
)

set(COCOA_SOURCE_FILES_OBJCPP
    src/macOS/AutoReleasePoolWrapper.mm
    src/macOS/StormView.mm
    src/macOS/StormWindowController.mm
    src/macOS/Utils.mm
    src/macOS/WindowImpl.mm
    src/macOS/VideoSettings.mm
    src/macOS/InputHandlerImpl.mm
)

set(COCOA_SOURCE_FILES_OBJC
    src/macOS/StormApplication.m
    src/macOS/StormApplicationDelegate.m
    src/macOS/StormWindow.m
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/macOS
    PREFIX "Cocoa Source Files"
    FILES ${COCOA_SOURCE_FILES_OBJCPP} ${COCOA_SOURCE_FILES_OBJC}
)

set(COCOA_HEADER_FILES_OBJCPP
    src/macOS/AutoReleasePoolWrapper.h
    src/macOS/StormView.h
    src/macOS/StormWindowController.h
    src/macOS/Utils.h
    src/macOS/WindowImpl.h
    src/macOS/InputHandlerImpl.h
)

set(COCOA_HEADER_FILES_OBJC
    src/macOS/StormApplication.h
    src/macOS/StormApplicationDelegate.h
    src/macOS/StormWindow.h
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/macOS
    PREFIX "Cocoa Header Files"
    FILES ${COCOA_HEADER_FILES_OBJCPP} ${COCOA_HEADER_FILES_OBJC}
)

set_source_files_properties(
    ${COCOA_SOURCE_FILES_OBJCPP}
    ${COCOA_HEADER_FILES_OBJCPP}
    PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
        LANGUAGE CXX
)

set_source_files_properties(
    ${COCOA_SOURCE_FILES_OBJC}
    ${COCOA_HEADER_FILES_OBJC}
    PROPERTIES
        COMPILE_FLAGS "-x objective-c"
        LANGUAGE C
)

set(XCB_SOURCE_FILES
    src/x11/InputHandlerImpl.hpp
    src/x11/WindowImpl.hpp
    src/x11/Utils.hpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/x11
    PREFIX "XCB Source Files"
    FILES ${XCB_SOURCE_FILES}
)

set(XCB_HEADER_FILES
    src/x11/VideoSettings.cpp
    src/x11/InputHandlerImpl.cpp
    src/x11/WindowImpl.cpp
    src/x11/Utils.cpp
)

source_group(
    FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/x11
    PREFIX "XCB Header Files"
    FILES ${XCB_HEADER_FILES}
)

add_library(
    stormkit-window
    ${STORM_OUTPUT_TYPE}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    $<$<BOOL:${STORM_OS_WIN32}>:${WIN32_SOURCE_FILES}>
    $<$<BOOL:${STORM_OS_WIN32}>:${WIN32_HEADER_FILES}>
    $<$<BOOL:${STORM_OS_LINUX}>:${XCB_SOURCE_FILES}>
    $<$<BOOL:${STORM_OS_LINUX}>:${XCB_HEADER_FILES}>
    $<$<BOOL:${STORM_OS_MACOS}>:${COCOA_SOURCE_FILES_OBJCPP}>
    $<$<BOOL:${STORM_OS_MACOS}>:${COCOA_SOURCE_FILES_OBJC}>
    $<$<BOOL:${STORM_OS_MACOS}>:${COCOA_HEADER_FILES_OBJCPP}>
    $<$<BOOL:${STORM_OS_MACOS}>:${COCOA_HEADER_FILES_OBJC}>
)

add_dependencies(
        stormkit-window
        StormKit::core
)

target_include_directories(
        stormkit-window
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    stormkit-window
    PUBLIC
        StormKit::core
		GSL::GSL
		${FILESYSTEM_LIBRARIES}
    PRIVATE
        $<$<BOOL:${STORM_OS_LINUX}>:XCB::XCB>
        $<$<BOOL:${STORM_OS_LINUX}>:XCB::KEYSYMS>
        $<$<BOOL:${STORM_OS_LINUX}>:XCB::ICCCM>
        $<$<BOOL:${STORM_OS_LINUX}>:XCB::RANDR>
        $<$<BOOL:${STORM_OS_MACOS}>:${APPKIT_LIBRARY}>
        $<$<BOOL:${STORM_OS_MACOS}>:${QUARTZ_CORE_LIBRARY}>
        $<$<BOOL:${STORM_OS_MACOS}>:${IOKIT_LIBRARY}>
        $<$<BOOL:${STORM_OS_MACOS}>:${CARBON_LIBRARY}>
)

target_compile_definitions(
    stormkit-window
    PUBLIC
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    PRIVATE
        STORM_BUILD
        $<$<BOOL:${STORM_OS_LINUX}>:STORM_X11>
)

add_library(StormKit::window ALIAS stormkit-window)
if(NOT STORM_OS_MACOS)
        set_target_properties(stormkit-window PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
endif()
set_common_properties(stormkit-window)
enable_cpp17(stormkit-window "${STORM_COMPILER}")
append_prefix_and_suffix(stormkit-window)
export_macos_framework("${HEADER_FILES}" window)
#export_target(stormkit-window Window)

if(STORM_ENABLE_EXAMPLES)
        add_subdirectory(samples/simple_window)
endif()
