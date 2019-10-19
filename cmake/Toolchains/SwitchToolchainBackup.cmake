if (NOT DEFINED ENV{DEVKITPRO})
    cmake_panic("Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif ()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(SWITCH TRUE) # To be used for multiplatform projects

# devkitPro paths are broken on Windows. We need to use this macro to fix those.
macro(msys_to_cmake_path msys_path resulting_path)
    if (WIN32)
        string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${resulting_path} ${msys_path})
    else ()
        set(${resulting_path} ${msys_path})
    endif ()
endmacro()

msys_to_cmake_path($ENV{DEVKITPRO} DEVKITPRO)
set(DEVKITA64 ${DEVKITPRO}/devkitA64)
set(LLVM_TOOLCHAIN $ENV{AARCHLLVM})
set(LIBNX ${DEVKITPRO}/libnx)
set(PORTLIBS_PATH ${DEVKITPRO}/portlibs)
set(PORTLIBS ${PORTLIBS_PATH}/switch)

set(TOOLCHAIN_PREFIX ${DEVKITA64}/bin/aarch64-none-elf-)
set(LLVM_TOOLCHAIN_PREFIX ${LLVM_TOOLCHAIN}/bin/llvm-)
if (WIN32)
    set(TOOLCHAIN_SUFFIX ".exe")
else ()
    set(TOOLCHAIN_SUFFIX "")
endif ()

#set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_SUFFIX})
#set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_SUFFIX})
set(CMAKE_C_COMPILER ${LLVM_TOOLCHAIN}/bin/clang${TOOLCHAIN_SUFFIX})
set(CMAKE_CXX_COMPILER ${LLVM_TOOLCHAIN}/bin/clang++${TOOLCHAIN_SUFFIX})
set(CMAKE_ASM_COMPILER ${LLVM_TOOLCHAIN_PREFIX}mc${TOOLCHAIN_SUFFIX})

set(CMAKE_AR ${LLVM_TOOLCHAIN_PREFIX}ar${TOOLCHAIN_SUFFIX} CACHE STRING "")
set(CMAKE_RANLIB ${LLVM_TOOLCHAIN_PREFIX}ranlib${TOOLCHAIN_SUFFIX} CACHE STRING "")
set(CMAKE_LD ${LLVM_TOOLCHAIN}/bin/lld${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
#set(CMAKE_LD "${TOOLCHAIN_PREFIX}ld${TOOLCHAIN_SUFFIX}" CACHE INTERNAL "")
set(CMAKE_OBJCOPY ${LLVM_TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE_UTIL ${LLVM_TOOLCHAIN_PREFIX}size${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")

set(PKG_CONFIG_EXECUTABLE ${TOOLCHAIN_PREFIX}pkg-config${TOOLCHAIN_SUFFIX})

set(WITH_PORTLIBS ON CACHE BOOL "use portlibs ?")
if (WITH_PORTLIBS)
    set(CMAKE_FIND_ROOT_PATH ${DEVKITA64} ${DEVKITPRO} ${LIBNX} ${PORTLIBS})
else ()
    set(CMAKE_FIND_ROOT_PATH ${DEVKITA64} ${DEVKITPRO} ${LIBNX})
endif ()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_definitions(-D__SWITCH__)

message(STATUS ${CMAKE_LD})

set(SYS_INCLUDES "-isystem ${DEVKITPRO}/libnx/include -isystem ${LLVM_TOOLCHAIN}/include/c++/v1")

#cortex-a57+crc+crypto+simd+neon
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLIBCXX_USE_COMPILER_RT=YES -fPIE -fexceptions -fuse-ld=lld -mtune=cortex-a53 -target aarch64-arm-none-eabi -nostdlib --rtlib=compiler-rt ${SYS_INCLUDES}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} --stdlib=libc++ -nodefaultlibs -nostdinc++")

set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_CXX_ARCHIVE_CREATE} rcs")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_C_ARCHIVE_CREATE} rcs")
set(ASMFLAGS "-arch=aarch64 -triple aarch64-none-elf")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${ARCH} -L${LLVM_TOOLCHAIN}/lib -rpath ${LLVM_TOOLCHAIN}/lib -T${CMAKE_CURRENT_SOURCE_DIR}/switch-ld/link.ld -Bsymbolic -lc -lm -lclang_rt.builtins-aarch64 -lpthread -llzma -lc++ -lc++abi -lunwind")
set(CMAKE_MODULE_LINKER_FLAGS_INIT ${CMAKE_EXE_LINKER_FLAGS_INIT})

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available")
set(CMAKE_INSTALL_PREFIX ${PORTLIBS})
set(CMAKE_PREFIX_PATH ${PORTLIBS})
