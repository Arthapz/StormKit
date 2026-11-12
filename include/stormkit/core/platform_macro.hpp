// Copryright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_PLATFORM_MACRO_HPP
#define STORMKIT_PLATFORM_MACRO_HPP

#include <stormkit/core/macro_utils.hpp>

#include <version>

#if defined(__cplusplus)
    #ifndef __clang__ // remove when clang support headerunits with clang-scan-deps
        #if not(_MSC_VER >= 1900 or __cplusplus >= 202002L)
            #error "Stormkit need a C++20 compiler"
        #endif
    #endif
#else
    #error "Stormkit need a c++ compiler"
#endif

#define STORMKIT_OS_IS_NX      false
#define STORMKIT_OS_IS_WINDOWS false
#define STORMKIT_OS_IS_WIN32   false
#define STORMKIT_OS_IS_WIN64   false
#define STORMKIT_OS_IS_LINUX   false
#define STORMKIT_OS_IS_ANDROID false
#define STORMKIT_OS_IS_MACOS   false
#define STORMKIT_OS_IS_IOS     false
#define STORMKIT_OS_IS_TVOS    false
#define STORMKIT_OS_IS_POSIX   false
#define STORMKIT_OS_IS_DARWIN  false

#define STORMKIT_OS_IS_32_BITS false
#define STORMKIT_OS_IS_64_BITS false

#define STORMKIT_OS_NX             "Nintendo Switch"
#define STORMKIT_OS_WIN32          "Windows 32 bits"
#define STORMKIT_OS_WIN64          "Windows 64 bits"
#define STORMKIT_OS_ANDROID        "Android"
#define STORMKIT_OS_LINUX64        "Linux 64 bits"
#define STORMKIT_OS_LINUX32        "Linux 32 bits"
#define STORMKIT_OS_MACOS          "Apple macOS"
#define STORMKIT_OS_IOS            "Apple iOS"
#define STORMKIT_OS_IOS_SIMULATOR  "Apple iOS (simulator)"
#define STORMKIT_OS_TVOS           "Apple tvOS"
#define STORMKIT_OS_TVOS_SIMULATOR "Apple tvOS (simulator)"

#if defined(__SWITCH__)
    #undef STORMKIT_OS_IS_64_BITS
    #define STORMKIT_OS_IS_64_BITS true
    #undef STORMKIT_OS_IS_NX
    #define STORMKIT_OS_IS_NX true

    #define STORMKIT_OS STORMKIT_OS_NX
#elif defined(_WIN64)
    #undef STORMKIT_OS_IS_64_BITS
    #define STORMKIT_OS_IS_64_BITS true
    #undef STORMKIT_OS_IS_WIN64
    #define STORMKIT_OS_IS_WIN64 true
    #undef STORMKIT_OS_IS_WINDOWS
    #define STORMKIT_OS_IS_WINDOWS true

    #define STORMKIT_OS STORMKIT_OS_WIN64
#elif defined(_WIN32)
    #undef STORMKIT_OS_IS_32_BITS
    #define STORMKIT_OS_IS_32_BITS true
    #undef STORMKIT_OS_IS_WIN32
    #define STORMKIT_OS_IS_WIN32 true
    #undef STORMKIT_OS_IS_WINDOWS
    #define STORMKIT_OS_IS_WINDOWS true

    #define STORMKIT_OS STORMKIT_OS_WIN32
#elif defined(__linux__)
    #if defined(__x86_64__)
        #undef STORMKIT_OS_IS_64_BITS
        #define STORMKIT_OS_IS_64_BITS true
    #else
        #undef STORMKIT_OS_IS_32_BITS
        #define STORMKIT_OS_IS_32_BITS true
    #endif

    #undef STORMKIT_OS_IS_LINUX
    #define STORMKIT_OS_IS_LINUX true

    #if defined(__ANDROID__)
        #undef STORMKIT_OS_IS_ANDROID
        #define STORMKIT_OS_IS_ANDROID true
    #endif

    #define STORMKIT_OS STORMKIT_OS_LINUX
#elif defined(__MACH__)
extern "C" {
    #include <TargetConditionals.h>
}

    #undef STORMKIT_OS_IS_64_BITS
    #define STORMKIT_OS_IS_64_BITS true

    #undef STORMKIT_OS_IS_DARWIN
    #define STORMKIT_OS_IS_DARWIN true
    #undef STORMKIT_OS_IS_POSIX
    #define STORMKIT_OS_IS_POSIX true

    #if TARGET_OS_IPHONE
        #undef STORMKIT_OS_IS_IOS
        #define STORMKIT_OS_IS_IOS true
        #define STORMKIT_OS        STORMKIT_OS_IOS
    #elif TARGET_OS_MAC
        #undef STORMKIT_OS_IS_MACOS
        #define STORMKIT_OS_IS_MACOS true
        #define STORMKIT_OS          STORMKIT_OS_MACOS
    #endif
#else
    #error "Targeted platform not supported !"
#endif

#define STORMKIT_CPPLIB_LIBCPP    "libc++"
#define STORMKIT_CPPLIB_MSSTL     "MSSTL"
#define STORMKIT_CPPLIB_LIBSTDCPP "libstdc++"

#define STORMKIT_COMPILER_CLANG "clang"
#define STORMKIT_COMPILER_GCC   "gcc"
#define STORMKIT_COMPILER_CL    "cl"

#define STORMKIT_COMPILER_IS_CLANG false
#define STORMKIT_COMPILER_IS_GCC   false
#define STORMKIT_COMPILER_IS_CL    false
#define STORMKIT_COMPILER_IS_MINGW false

#define STORMKIT_CPPLIB_IS_LIBCPP    false
#define STORMKIT_CPPLIB_IS_MSSTL     false
#define STORMKIT_CPPLIB_IS_LIBSTDCPP false

#ifdef __clang__
    #undef STORMKIT_COMPILER_IS_CLANG
    #define STORMKIT_COMPILER_IS_CLANG true
    #define STORMKIT_COMPILER          STORMKIT_COMPILER_CLANG
    #ifdef __MINGW32__
        #undef STORMKIT_COMPILER_IS_MINGW
        #define STORMKIT_COMPILER_IS_MINGW
        #define STORMKIT_COMPILER_VERSION string { "MinGW CLang " } + __clang_version__
    #else
        #define STORMKIT_COMPILER_VERSION string { "CLang " } + __clang_version__
    #endif

    #if defined(_LIBCPP_VERSION)
        #undef STORMKIT_CPPLIB_IS_LIBCPP
        #define STORMKIT_CPPLIB_IS_LIBCPP true
        #define STORMKIT_CPPLIB           STORMKIT_CPPLIB_LIBCPP
    #elif defined(_MSC_VER)
        #undef STORMKIT_CPPLIB_IS_MSSTL
        #define STORMKIT_CPPLIB_IS_MSSTL true
        #define STORMKIT_CPPLIB          STORMKIT_CPPLIB_MSSTL
    #else
    #endif
    #define STORMKIT_COMPILER STORMKIT_COMPILER_CLANG

    #define STORMKIT_INTRINSIC
    #define STORMKIT_FORCE_INLINE [[gnu::always_inline]]

    #define STORMKIT_PUSH_WARNINGS _Pragma("clang diagnostic push")
    #undef STORMKIT_IGNORE_WARNING_CLANG
    #define STORMKIT_IGNORE_WARNING_CLANG(warning) STORMKIT_PRAGMA_FROM_STRING(clang diagnostic ignore warning)
    #define STORMKIT_POP_WARNINGS                  _Pragma("clang diagnostic pop")
#elifdef _MSC_VER
    #undef STORMKIT_COMPILER_IS_CL
    #define STORMKIT_COMPILER_IS_CL   true
    #define STORMKIT_COMPILER         STORMKIT_COMPILER_CL
    #define STORMKIT_COMPILER_VERSION string { "cl " } + _MSC_VER

    #undef STORMKIT_COMPILER_IS_MSSTL
    #define STORMKIT_COMPILER_IS_MSSTL true
    #define STORMKIT_CPPLIB            STORMKIT_CPPLIB_MSSTL

    #define STORMKIT_INTRINSIC    [[msvc::intrinsic]]
    #define STORMKIT_FORCE_INLINE [[msvc::forceinline]]

    #define STORMKIT_PUSH_WARNINGS _Pragma("warning(push)")
    #undef STORMKIT_IGNORE_WARNING_MSVC
    #define STORMKIT_IGNORE_WARNING_MSVC(value) STORMKIT_PRAGMA_FROM_STRING(warning(disable : value))
    #define STORMKIT_POP_WARNINGS               _Pragma("warning(pop)")
#elifdef __GNUC__
    #undef STORMKIT_COMPILER_IS_GCC
    #define STORMKIT_COMPILER_IS_GCC true
    #define STORMKIT_COMPILER        STORMKIT_COMPILER_GCC
    #ifdef __MINGW32__
        #undef STORMKIT_COMPILER_IS_MINGW
        #define STORMKIT_COMPILER_IS_MINGW
        #define STORMKIT_COMPILER_VERSION      \
            "MinGW GCC "                       \
              + std::to_string(__GNUC__)       \
              + "."                            \
              + std::to_string(__GNUC_MINOR__) \
              + "."                            \
              + "."                            \
              + std::to_string(__GNUC_PATCHLEVEL__)
    #else
        #define STORMKIT_COMPILER_VERSION      \
            "MinGW GCC "                       \
              + std::to_string(__GNUC__)       \
              + "."                            \
              + std::to_string(__GNUC_MINOR__) \
              + "."                            \
              + "."                            \
              + std::to_string(__GNUC_PATCHLEVEL__)
    #endif

    #if defined(_LIBCPP_VERSION)
        #undef STORMKIT_CPPLIB_IS_LIBCPP
        #define STORMKIT_CPPLIB_IS_LIBCPP true
        #define STORMKIT_CPPLIB           STORMKIT_CPPLIB_LIBCPP
    #else
        #undef STORMKIT_CPPLIB_IS_LIBSTDCPP
        #define STORMKIT_CPPLIB_IS_LIBSTDCPP true
        #define STORMKIT_CPPLIB              STORMKIT_CPPLIB_LIBSTDCPP
    #endif

    #define STORMKIT_INTRINSIC
    #define STORMKIT_FORCE_INLINE [[gnu::always_inline]]

    #define STORMKIT_PUSH_WARNINGS _Pragma("GCC diagnostic push")
    #undef STORMKIT_IGNORE_WARNING_GCC
    #define STORMKIT_IGNORE_WARNING_GCC(warning) STORMKIT_PRAGMA_FROM_STRING(GCC diagnostic ignored warning)
    #define STORMKIT_POP_WARNINGS                _Pragma("GCC diagnostic pop")
#endif

#if STORMKIT_OS_IS_WINDOWS
    #define STORMKIT_EXPORT            __declspec(dllexport)
    #define STORMKIT_IMPORT            __declspec(dllimport)
    #define STORMKIT_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
    #if STORMKIT_COMPILER_IS_CL
        #define STORMKIT_PRIVATE [[gnu::visibility("hidden")]]
    #else
        #define STORMKIT_PRIVATE
    #endif
#else
    #define STORMKIT_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #define STORMKIT_IMPORT            [[gnu::visibility("default")]]
    #define STORMKIT_EXPORT            [[gnu::visibility("default")]]
    #define STORMKIT_PRIVATE           [[gnu::visibility("hidden")]]
#endif

#if __has_cpp_attribute(lifetimebound)
    #define STORMKIT_LIFETIMEBOUND [[lifetimebound]]
#elif __has_cpp_attribute(msvc::lifetimebound)
    #define STORMKIT_LIFETIMEBOUND [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
    #define STORMKIT_LIFETIMEBOUND [[clang::lifetimebound]]
#elif __has_cpp_attribute(gnu::no_dangling)
    #define STORMKIT_LIFETIMEBOUND [[gnu::no_dangling]]
#else
    #define STORMKIT_LIFETIMEBOUND
#endif

#if __has_cpp_attribute(lifetime_capture_by)
    #define STORMKIT_LIFETIME_CAPTURE_BY(BY) [[lifetime_capture_by(BY)]]
#elif __has_cpp_attribute(msvc::lifetime_capture_by)
    #define STORMKIT_LIFETIME_CAPTURE_BY(BY) [[msvc::lifetime_capture_by(BY)]]
#elif __has_cpp_attribute(clang::lifetime_capture_by)
    #define STORMKIT_LIFETIME_CAPTURE_BY(BY) [[clang::lifetime_capture_by(BY)]]
#else
    #define STORMKIT_LIFETIME_CAPTURE_BY(BY)
#endif

#if __has_cpp_attribute(lifetime_capture_by_this)
    #define STORMKIT_LIFETIME_CAPTURE_BY_THIS [[lifetime_capture_by_this]]
#elif __has_cpp_attribute(msvc::lifetime_capture_by_this)
    #define STORMKIT_LIFETIME_CAPTURE_BY_THIS [[msvc::lifetime_capture_by_this]]
#elif __has_cpp_attribute(clang::lifetime_capture_by_this)
    #define STORMKIT_LIFETIME_CAPTURE_BY_THIS [[clang::lifetime_capture_by_this]]
#else
    #define STORMKIT_LIFETIME_CAPTURE_BY_THIS
#endif

#if __has_cpp_attribute(lifetime_capture_by_global)
    #define STORMKIT_LIFETIME_CAPTURE_BY_GLOBAL [[lifetime_capture_by_global]]
#elif __has_cpp_attribute(msvc::lifetime_capture_by_global)
    #define STORMKIT_LIFETIME_CAPTURE_BY_GLOBAL [[msvc::lifetime_capture_by_global]]
#elif __has_cpp_attribute(clang::lifetime_capture_by_global)
    #define STORMKIT_LIFETIME_CAPTURE_BY_GLOBAL [[clang::lifetime_capture_by_global]]
#else
    #define STORMKIT_LIFETIME_CAPTURE_BY_GLOBAL
#endif

#if __has_cpp_attribute(lifetime_capture_by_unknown)
    #define STORMKIT_LIFETIME_CAPTURE_BY_UNKNOWN [[lifetime_capture_by_unknown]]
#elif __has_cpp_attribute(msvc::lifetime_capture_by_unknown)
    #define STORMKIT_LIFETIME_CAPTURE_BY_UNKNOWN [[msvc::lifetime_capture_by_unknown]]
#elif __has_cpp_attribute(clang::lifetime_capture_by_unknown)
    #define STORMKIT_LIFETIME_CAPTURE_BY_UNKNOWN [[clang::lifetime_capture_by_unknown]]
#else
    #define STORMKIT_LIFETIME_CAPTURE_BY_UNKNOWN
#endif

#if __has_cpp_attribute(clang::guarded_by)
    #define STORMKIT_GUARDED_BY(x) [[clang::guarded_by(x)]]
#else
    #define STORMKIT_GUARDED_BY(_)
#endif

#if __has_cpp_attribute(gnu::pure)
    #define STORMKIT_PURE [[gnu::pure]]
#else
    #define STORMKIT_PURE
#endif

#if __has_cpp_attribute(gsl::Owner)
    #define STORMKIT_OWNER [[gsl::Owner]]
#else
    #define STORMKIT_OWNER [[gsl::Owner]]
#endif

#if __has_cpp_attribute(gsl::Pointer)
    #define STORMKIT_VIEW [[gsl::Pointer]]
#else
    #define STORMKIT_VIEW [[gsl::Pointer]]
#endif

#if __has_cpp_attribute(trivially_relocatable)
    #define STORMKIT_TRIVIALLY_RELOCATABLE [[trivially_relocatable]]
#else
    #define STORMKIT_TRIVIALLY_RELOCATABLE [[clang::trivial_abi]]
#endif

#if not defined(STORMKIT_COMPILER_IS_CL)
    #if __has_cpp_attribute(gnu::const)
        #define STORMKIT_CONST [[gnu::const]]
    #endif
#endif

#if not defined(STORMKIT_CONST)
    #define STORMKIT_CONST
#endif

#if not defined(STORMKIT_PACKED_BEGIN)
    #define STORMKIT_PACKED_AS_BEGIN(x) _Pragma("pack(push, " STORMKIT_STRINGIRY(x) ")")
    #define STORMKIT_PACKED_AS_END      _Pragma("pack(pop)")
#endif

#define STORMKIT_IGNORE_WARNING_GCC_CLANG(warning) \
    STORMKIT_WARNING_IGNORE_GCC(warning)           \
    STORMKIT_WARNING_IGNORE_CLANG(warning)

#ifdef STORMKIT_BUILD
    #define STORMKIT_API STORMKIT_EXPORT
#else
    #define STORMKIT_API STORMKIT_IMPORT
#endif

inline constexpr auto stormkit_cpplib = STORMKIT_CPPLIB;

#ifdef NDEBUG
    #define STORMKIT_RELEASE_MODE
#else
    #define STORMKIT_DEBUG_MODE
#endif

#endif
