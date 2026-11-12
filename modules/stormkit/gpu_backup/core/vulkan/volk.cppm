module;

#include <stormkit/core/platform_macro.hpp>

export module stormkit.gpu.core:vulkan.volk;

STORMKIT_PUSH_WARNINGS
#if STORMKIT_COMPILER_IS_CLANG
    #pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#elif STORMKIT_COMPILER_IS_CL
    #pragma warning(disable: 5244)
#endif
export extern "C++" {
}

STORMKIT_POP_WARNINGS
