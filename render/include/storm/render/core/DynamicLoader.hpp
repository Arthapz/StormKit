// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <filesystem>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/module/Module.hpp>

#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORM_PUBLIC DynamicLoader: public core::NonCopyable {
      public:
#ifdef STORM_OS_LINUX
        // static constexpr auto DEFAULT_VULKAN_LIB = "libashes.so";
        static constexpr auto DEFAULT_VULKAN_LIB = "libvulkan.so";
#elif defined(STORM_OS_WINDOWS)
        static constexpr auto DEFAULT_VULKAN_LIB = "vulkan-1.dll";
#elif defined(STORM_OS_APPLE)
        static constexpr auto DEFAULT_VULKAN_LIB = "libvulkan.dylib";
#endif
        using GetInstanceProcAddrType = PFN_vkVoidFunction(VkInstance, const char *);

        explicit DynamicLoader(std::filesystem::path vulkan_lib_path = DEFAULT_VULKAN_LIB);
        ~DynamicLoader();

        DynamicLoader(DynamicLoader &&);
        DynamicLoader &operator=(DynamicLoader &&);

        inline bool success() const noexcept { return m_success; }

        inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr() const noexcept { return m_pfn; }

      private:
        bool m_success = false;
        module::Module m_module;

        std::function<GetInstanceProcAddrType> m_vkGetInstanceProcAddr;
        PFN_vkGetInstanceProcAddr m_pfn;
    };
} // namespace storm::render
