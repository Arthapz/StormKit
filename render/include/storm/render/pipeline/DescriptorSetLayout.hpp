// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>

namespace storm::render {
    struct DescriptorSetLayoutBinding {
        core::UInt32 binding;
        DescriptorType type;
        ShaderStage stages;
        core::ArraySize descriptor_count;
    };

    class STORM_PUBLIC DescriptorSetLayout: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Set_Layout;

        explicit DescriptorSetLayout(const Device &device);
        ~DescriptorSetLayout();

        DescriptorSetLayout(DescriptorSetLayout &&rhs) noexcept;
        DescriptorSetLayout &operator=(DescriptorSetLayout &&);

        inline void addBinding(DescriptorSetLayoutBinding binding);

        void bake();

        inline core::Hash64 hash() const noexcept;
        inline DescriptorSetLayoutBindingConstSpan bindings() const noexcept;

        inline vk::DescriptorSetLayout vkDescriptorSetLayout() const noexcept;
        inline operator vk::DescriptorSetLayout() const noexcept;
        inline vk::DescriptorSetLayout vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

        inline bool operator==(const DescriptorSetLayout &second);

      private:
        void updateHash() noexcept;

        DeviceConstObserverPtr m_device;

        DescriptorSetLayoutBindingArray m_bindings;

        core::Hash64 m_hash;

        RAIIVkDescriptorSetLayout m_vk_descriptor_set_layout;
    };
} // namespace storm::render

HASH_FUNC(storm::render::DescriptorSetLayoutBinding)
HASH_FUNC(storm::render::DescriptorSetLayoutBindingArray)

#include "DescriptorSetLayout.inl"
