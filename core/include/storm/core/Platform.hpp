#pragma once
/**
 * @namespace storm
 *
 * namespace which group storm library api
 */

#include <storm/core/Configure.hpp>

#if defined(__cplusplus)
    #if !(_MSC_VER >= 1900 || __cplusplus >= 201402L)
        #error "Stormkit need stdc++ >= 14 compiler"
    #endif
#else
    #error "Stormkit need a c++ compiler"
#endif

#if defined(_MSC_VER)
    #pragma warning(disable : 4251)
    #define STORM_COMPILER_MSVC
    #define STORM_COMPILER STORM_COMPILER_MSVC
#elif defined(__clang__)
    #define STORM_COMPILER_CLANG "Clang " + __clang_version__
    #define STORM_COMPILER STORM_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define STORM_COMPILER_GCC                                               \
        "GCC " + std::to_string(__GNUC__) + std::to_string(__GNUC_MINOR__) + \
            std::to_string(__GNUC_PATCHLEVEL__)
    #define STORM_COMPILER STORM_COMPILER_GCC
#endif

#if defined(__SWITCH__)
    #define STORM_OS_NX "Nintendo Switch"
    #define STORM_BITS 64
    #define STORM_IMPORT
    #define STORM_EXPORT
    #define STORM_PRIVATE [[gnu::visibility("hidden")]]
    #define STORM_OS STORM_OS_NX
    #define STORM_DLLSPECTIMPORT __declspec(dllimport)
#elif defined(_WIN64)
extern "C" {
    #include <Windows.h>
}
    #define STORM_OS_WIN64 "Windows 64 bits"
    #define STORM_OS_WINDOWS STORM_OS_WIN64
    #define STORM_BITS_64
    #define STORM_EXPORT __declspec(dllexport)
    #define STORM_IMPORT __declspec(dllimport)
    #define STORM_PRIVATE
    #define STORM_OS STORM_OS_WIN64
#elif defined(_WIN32)
extern "C" {
    #include <windows.h>
}
    #define STORM_OS_WIN32 "Windows 32 bits"
    #define STORM_OS_WINDOWS STORM_OS_WIN32
    #define STORM_BITS_32
    #define STORM_EXPORT __declspec(dllexport)
    #define STORM_IMPORT __declspec(dllimport)
    #define STORM_PRIVATE
    #define STORM_OS STORM_OS_WIN32
#elif defined(__ANDROID__)
    #define STORM_OS_ANDROID "Android"
    #define STORM_OS STORM_OS_ANDROID
    #define STORM_EXPORT [[gnu::visibility("default")]]
    #define STORM_IMPORT [[gnu::visibility("default")]]
    #define STORM_PRIVATE [[gnu::visibility("hidden")]]
#elif defined(__linux__)
extern "C" {
    #include <unistd.h>
}
    #if defined(__x86_64__)
        #define STORM_OS_LINUX64 "Linux 64 bits"
        #define STORM_OS_LINUX STORM_OS_LINUX64
        #define STORM_BITS_64
    #else
        #define STORM_OS_LINUX32 "Linux 32 bits"
        #define STORM_OS_LINUX STORM_OS_LINUX32
        #define STORM_BITS_32
    #endif

    #define STORM_OS STORM_OS_LINUX
    #define STORM_EXPORT [[gnu::visibility("default")]]
    #define STORM_IMPORT [[gnu::visibility("default")]]
    #define STORM_PRIVATE [[gnu::visibility("hidden")]]
#elif defined(__MACH__)
extern "C" {
    #include <TargetConditionals.h>
    #include <unistd.h>
}
    #if TARGET_OS_IPHONE
        #define STORM_OS_IOS "IOS"
        #define STORM_OS_APPLE STORM_OS_IOS
    #elif TARGET_OS_SIMULATOR
        #define STORM_OS_IOS_SIMULATOR "IOS Simulator"
        #define STORM_OS_IOS STORM_OS_IOS_SIMULATOR
        #define STORM_OS_APPLE STORM_OS_IOS
    #elif TARGET_OS_MAC
        #define STORM_OS_MACOS "OSX 64 bits"
        #define STORM_OS_APPLE STORM_OS_MACOS
    #endif
    #define STORM_OS STORM_OS_APPLE
    #define STORM_EXPORT [[gnu::visibility("default")]]
    #define STORM_IMPORT [[gnu::visibility("default")]]
    #define STORM_PRIVATE [[gnu::visibility("hidden")]]
#else
    #error "Targeted platform not supported !"
#endif

#ifndef STORM_STATIC
    #ifdef STORM_BUILD
        #define STORM_PUBLIC STORM_EXPORT
    #else
        #define STORM_PUBLIC STORM_IMPORT
    #endif
#else
    #define STORM_PUBLIC
    #define STORM_PRIVATE
#endif

#ifdef _POSIX_VERSION
    #define STORM_POSIX
#endif

#if STORM_BUILD_TYPE == 1
    #define STORM_BUILD_DEBUG
    #define STORM_BUILD_STRING "Debug"
#else
    #define STORM_BUILD_RELEASE
    #define STORM_BUILD_STRING "Release"
#endif

#define UNUSED(x) (void)x;

#if defined(STORM_COMPILER_GCC) || defined(STORM_COMPILER_CLANG)
    #define STORM_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(STORM_COMPILER_MSVC)
    #define STORM_CURRENT_FUNCTION __FUNCSIG__
#else
    #define STORM_CURRENT_FUNCTION __func__
#endif
