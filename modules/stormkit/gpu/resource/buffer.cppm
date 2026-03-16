// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.resource:buffer;

import std;

import stormkit.core;
import stormkit.gpu.core;

namespace stdr = std::ranges;

namespace cmeta    = stormkit::core::meta;
namespace cmonadic = stormkit::core::monadic;

namespace stormkit::gpu {
    struct BufferAPI;
}

export namespace stormkit::gpu {
    class Buffer;

    namespace view {
        class Buffer;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<Buffer> {
            using Of          = Buffer;
            using ValueType   = VkBuffer;
            using DeleterType = PFN_vkDestroyBuffer VolkDeviceTable::*;
            using ViewType    = view::Buffer;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::BUFFER;
        };
    } // namespace meta

    class STORMKIT_GPU_API Buffer: public OwnedByDevice<Buffer> {
      public:
        struct CreateInfo {
            BufferUsageFlag    usages;
            usize              size;
            MemoryPropertyFlag properties = MemoryPropertyFlag::HOST_VISIBLE | MemoryPropertyFlag::HOST_COHERENT;

            bool persistently_mapped = false;
        };

        ~Buffer();

        Buffer(const Buffer&)                    = delete;
        auto operator=(const Buffer&) -> Buffer& = delete;

        Buffer(Buffer&&) noexcept;
        auto operator=(Buffer&&) noexcept -> Buffer&;

        [[nodiscard]]
        auto usages() const noexcept -> BufferUsageFlag;
        [[nodiscard]]
        auto size() const noexcept -> usize;
        [[nodiscard]]
        auto memory_properties() const noexcept -> MemoryPropertyFlag;
        [[nodiscard]]
        auto is_persistently_mapped() const noexcept -> bool;

        auto map(ioffset offset) noexcept -> Expected<byte*>;
        auto map(ioffset offset, usize size) noexcept -> Expected<std::span<byte>>;

        template<typename T>
        auto map_as(ioffset offset) noexcept -> Expected<ref<T>>;

        bool mapped() const noexcept;

        template<typename Self>
        [[nodiscard]]
        auto data(this Self& self) noexcept -> cmeta::ForwardConst<Self, byte>*;
        template<typename Self>
        [[nodiscard]]
        auto data(this Self& self, usize size) noexcept
          -> cmeta::If<cmeta::IsConst<Self>, std::span<byte>, std::span<const byte>>;

        template<typename T>
        [[nodiscard]]
        auto data_as(this auto& self) noexcept -> ref<T>;

        auto flush(ioffset offset, usize size) noexcept -> Expected<void>;
        auto unmap() noexcept -> void;

        auto upload(std::span<const byte> data, ioffset offset = 0) noexcept -> Expected<void>;

        template<typename T>
            requires(not stormkit::meta::IsSpecializationWithNTTPOf<T, std::span>)
        auto upload(const T& data, ioffset offset = 0) noexcept -> Expected<void>;

        [[nodiscard]]
        auto allocation() const noexcept -> vk::Observer<VmaAllocation>;

        // clang-format off
  // private:
        // clang-format on
        Buffer(PrivateTag, view::Device) noexcept;
        auto do_init(PrivateTag, const CreateInfo&) noexcept -> Expected<void>;

      private:
        static auto find_memory_type(u32,
                                     VkMemoryPropertyFlagBits,
                                     const VkPhysicalDeviceMemoryProperties&,
                                     const VkMemoryRequirements&) noexcept -> u32;

        BufferUsageFlag    m_usages            = {};
        usize              m_size              = 0;
        MemoryPropertyFlag m_memory_properties = {};

        bool  m_is_persistently_mapped = false;
        byte* m_mapped_pointer         = nullptr;

        vk::Owned<VmaAllocation> m_vma_allocation = { cmonadic::discard() };

        friend struct BufferAPI;
    };

    namespace view {
        class STORMKIT_GPU_API Buffer: public DeviceObject<gpu::Buffer> {
          public:
            using ObjectInfo = typename meta::ObjectInfo<gpu::Buffer>;
            using ValueType  = ObjectInfo::ValueType;
            using ViewType   = ObjectInfo::ViewType;

            Buffer(const gpu::Buffer& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::Buffer> T>
            Buffer(const T& of) noexcept;
            ~Buffer() noexcept;

            Buffer(const Buffer&) noexcept;
            auto operator=(const Buffer&) noexcept -> Buffer&;

            Buffer(Buffer&&) noexcept;
            auto operator=(Buffer&&) noexcept -> Buffer&;

            [[nodiscard]]
            auto usages() const noexcept -> BufferUsageFlag;
            [[nodiscard]]
            auto size() const noexcept -> usize;
            [[nodiscard]]
            auto memory_properties() const noexcept -> MemoryPropertyFlag;
            [[nodiscard]]
            auto is_persistently_mapped() const noexcept -> bool;

            auto map(ioffset offset) noexcept -> Expected<byte*>;
            auto map(ioffset offset, usize size) noexcept -> Expected<std::span<byte>>;

            template<typename T>
            auto map_as(ioffset offset) noexcept -> Expected<ref<T>>;

            bool mapped() const noexcept;

            template<typename Self>
            [[nodiscard]]
            auto data(this Self& self) noexcept -> cmeta::ForwardConst<Self, byte>*;
            template<typename Self>
            [[nodiscard]]
            auto data(this Self& self, usize size) noexcept
              -> cmeta::If<cmeta::IsConst<Self>, std::span<byte>, std::span<const byte>>;

            template<typename T>
            [[nodiscard]]
            auto data_as(this auto& self) noexcept -> ref<T>;

            auto flush(ioffset offset, usize size) noexcept -> Expected<void>;
            auto unmap() noexcept -> void;

            auto upload(std::span<const byte> data, ioffset offset = 0) noexcept -> Expected<void>;

            template<typename T>
                requires(not stormkit::meta::IsSpecializationWithNTTPOf<T, std::span>)
            auto upload(const T& data, ioffset offset = 0) noexcept -> Expected<void>;

            [[nodiscard]]
            auto allocation() const noexcept -> vk::Observer<VmaAllocation>;

          private:
            BufferUsageFlag    m_usages            = {};
            usize              m_size              = 0;
            MemoryPropertyFlag m_memory_properties = {};

            bool  m_is_persistently_mapped = false;
            byte* m_mapped_pointer         = nullptr;

            vk::Observer<VmaAllocation> m_vma_allocation;

            friend struct gpu::BufferAPI;
        };
    } // namespace view

    struct BufferMemoryBarrier {
        AccessFlag src;
        AccessFlag dst;

        u32 src_queue_family_index = QUEUE_FAMILY_IGNORED;
        u32 dst_queue_family_index = QUEUE_FAMILY_IGNORED;

        const Buffer& buffer;
        usize         size;
        u64           offset = 0;
    };

    template<cmeta::HashType Ret = hash32>
    constexpr auto hasher(const Buffer::CreateInfo& create_info) noexcept -> Ret;
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline Buffer::Buffer(PrivateTag, view::Device device) noexcept
        : OwnedByDevice<Buffer> { std::move(device), &VolkDeviceTable::vkDestroyBuffer } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Buffer::~Buffer() {
        if (m_mapped_pointer != nullptr and m_vma_allocation) {
            const auto& allocator = device().allocator();

            vk::call(vmaUnmapMemory, allocator, m_vma_allocation);

            m_mapped_pointer = nullptr;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline Buffer::Buffer(Buffer&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::operator=(Buffer&& other) noexcept -> Buffer& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::usages() const noexcept -> BufferUsageFlag {
        EXPECTS(m_vma_allocation and m_vk_handle);
        return m_usages;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::size() const noexcept -> usize {
        EXPECTS(m_vma_allocation and m_vk_handle);
        return m_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::memory_properties() const noexcept -> MemoryPropertyFlag {
        return m_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::is_persistently_mapped() const noexcept -> bool {
        return m_is_persistently_mapped;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::map(ioffset offset, usize size) noexcept -> Expected<std::span<byte>> {
        EXPECTS(m_vma_allocation and m_vk_handle);
        auto   ptr = Try(map(offset));
        Return as_bytes_mut(ptr, size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    STORMKIT_FORCE_INLINE
    inline auto Buffer::map_as(ioffset offset) noexcept -> Expected<ref<T>> {
        EXPECTS(m_vma_allocation and m_vk_handle);

        const auto ptr = Try(map(offset));
        Return     from_bytes_mut<T>(ptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::mapped() const noexcept -> bool {
        return m_mapped_pointer != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto Buffer::data(this Self& self) noexcept -> cmeta::ForwardConst<Self, byte>* {
        EXPECTS(self.m_vma_allocation and self.m_vk_handle);
        EXPECTS(self.m_mapped_pointer);

        using Out = cmeta::ForwardConst<decltype(self), byte>*;
        return std::bit_cast<Out>(self.m_mapped_pointer);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto Buffer::data(this Self& self, usize size) noexcept
      -> cmeta::If<cmeta::IsConst<Self>, std::span<byte>, std::span<const byte>> {
        EXPECTS(self.m_vma_allocation and self.m_vk_handle);
        EXPECTS(self.m_mapped_pointer);

        using Out = std::span<cmeta::ForwardConst<Self, byte>>;
        return Out { std::bit_cast<typename Out::element_type>(self.m_mapped_pointer), size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename Self>
    STORMKIT_FORCE_INLINE
    inline auto Buffer::data_as(this Self& self) noexcept -> ref<T> {
        EXPECTS(self.m_vma_allocation and self.m_vk_handle);
        EXPECTS(self.m_mapped_pointer);

        using Type = cmeta::ForwardConst<decltype(self), T>*;
        return as_ref_like<Self>(std::bit_cast<Type>(self.data()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto Buffer::allocation() const noexcept -> vk::Observer<VmaAllocation> {
        return m_vma_allocation;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
        requires(not stormkit::meta::IsSpecializationWithNTTPOf<T, std::span>)
    STORMKIT_FORCE_INLINE
    inline auto Buffer::upload(const T& data, ioffset offset) noexcept -> Expected<void> {
        const auto bytes = as_bytes(data);
        return upload(bytes, offset);
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Buffer::Buffer(const gpu::Buffer& of) noexcept
            : DeviceObject<gpu::Buffer> { of },
              m_usages { of.usages() },
              m_size { of.size() },
              m_memory_properties { of.memory_properties() },
              m_is_persistently_mapped { of.is_persistently_mapped() },
              m_mapped_pointer { nullptr },
              m_vma_allocation { of.allocation() } {
            if (of.is_persistently_mapped()) m_mapped_pointer = std::bit_cast<byte*>(of.data());
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::Buffer> T>
        STORMKIT_FORCE_INLINE
        inline Buffer::Buffer(const T& of) noexcept
            : DeviceObject<gpu::Buffer> { of },
              m_usages { of->usages() },
              m_size { of->size() },
              m_memory_properties { of->memory_properties() },
              m_is_persistently_mapped { of->is_persistently_mapped() },
              m_mapped_pointer { nullptr },
              m_vma_allocation { of->allocation() } {
            if (of->is_persistently_mapped()) m_mapped_pointer = std::bit_cast<byte*>(of->data());
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Buffer::~Buffer() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Buffer::Buffer(const Buffer& other) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::operator=(const Buffer& other) noexcept -> Buffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline Buffer::Buffer(Buffer&&) noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::operator=(Buffer&&) noexcept -> Buffer& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::usages() const noexcept -> BufferUsageFlag {
            return m_usages;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::size() const noexcept -> usize {
            return m_size;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::memory_properties() const noexcept -> MemoryPropertyFlag {
            return m_memory_properties;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::is_persistently_mapped() const noexcept -> bool {
            return m_is_persistently_mapped;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::map(ioffset offset, usize size) noexcept -> Expected<std::span<byte>> {
            EXPECTS(m_vma_allocation and m_vk_handle);
            auto   ptr = Try(map(offset));
            Return as_bytes_mut(ptr, size);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T>
    STORMKIT_FORCE_INLINE
        inline auto Buffer::map_as(ioffset offset) noexcept -> Expected<ref<T>> {
            EXPECTS(m_vma_allocation and m_vk_handle);

            const auto ptr = Try(map(offset));
            Return     from_bytes_mut<T>(ptr);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::mapped() const noexcept -> bool {
            return m_mapped_pointer != nullptr;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename Self>
    STORMKIT_FORCE_INLINE
        inline auto Buffer::data(this Self& self) noexcept -> cmeta::ForwardConst<Self, byte>* {
            EXPECTS(self.m_vma_allocation and self.m_vk_handle);
            EXPECTS(self.m_mapped_pointer);

            using Out = cmeta::ForwardConst<decltype(self), byte>*;
            return std::bit_cast<Out>(self.m_mapped_pointer);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename Self>
    STORMKIT_FORCE_INLINE
        inline auto Buffer::data(this Self& self, usize size) noexcept
          -> cmeta::If<cmeta::IsConst<Self>, std::span<byte>, std::span<const byte>> {
            EXPECTS(self.m_vma_allocation and self.m_vk_handle);
            EXPECTS(self.m_mapped_pointer);

            using Out = std::span<cmeta::ForwardConst<Self, byte>>;
            return Out { std::bit_cast<typename Out::element_type>(self.m_mapped_pointer), size };
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<typename T, typename Self>
    STORMKIT_FORCE_INLINE
        inline auto Buffer::data_as(this Self& self) noexcept -> ref<T> {
            EXPECTS(self.m_vma_allocation and self.m_vk_handle);
            EXPECTS(self.m_mapped_pointer);

            using Type = cmeta::ForwardConst<decltype(self), T>*;
            return as_ref_like<Self>(std::bit_cast<Type>(self.data()));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto Buffer::allocation() const noexcept -> vk::Observer<VmaAllocation> {
            return m_vma_allocation;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<cmeta::HashType Ret = hash32>
    constexpr auto hasher(const Buffer::CreateInfo& create_info) noexcept -> Ret {
        return hash(create_info.usages, create_info.size, create_info.properties);
    }
} // namespace stormkit::gpu
