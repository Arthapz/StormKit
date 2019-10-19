// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/Fwd.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    struct BufferDescriptor {
        DescriptorType type = DescriptorType::Uniform_Buffer;
        core::UInt32 binding;
        HardwareBufferConstObserverPtr buffer;
        core::UInt32 range;
        core::UInt32 offset;
    };

    struct TextureDescriptor {
        DescriptorType type = DescriptorType::Combined_Texture_Sampler;
        core::UInt32 binding;
        TextureLayout layout;
        TextureViewConstObserverPtr texture_view;
        SamplerConstObserverPtr sampler;
    };

    using Descriptor      = std::variant<BufferDescriptor, TextureDescriptor>;
    using DescriptorArray = std::vector<Descriptor>;
    template<std::size_t N>
    using DescriptorStaticArray = std::array<Descriptor, N>;

    class STORM_PUBLIC DescriptorSet: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Descriptor_Set;
        DescriptorSet(const render::DescriptorPool &pool,
                      std::vector<DescriptorType> type,
                      RAIIVkDescriptorSet sets);
        ~DescriptorSet();

        DescriptorSet(DescriptorSet &&);
        DescriptorSet &operator=(DescriptorSet &&);

        void update(core::span<const Descriptor> descriptors);

        inline core::span<const DescriptorType> types() const noexcept;

        inline vk::DescriptorSet vkDescriptorSet() const noexcept;
        inline operator vk::DescriptorSet() const noexcept;
        inline vk::DescriptorSet vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstObserverPtr m_device;
        DescriptorPoolConstObserverPtr m_pool;

        std::vector<DescriptorType> m_types;

        RAIIVkDescriptorSet m_vk_descriptor_set;
    };
} // namespace storm::render

#include "DescriptorSet.inl"
