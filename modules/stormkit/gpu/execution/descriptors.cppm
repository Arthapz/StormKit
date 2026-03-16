// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/api.hpp>
#include <stormkit/gpu/vulkan.hpp>

export module stormkit.gpu.execution:descriptors;

import std;

import stormkit.core;
import stormkit.gpu.core;
import stormkit.gpu.resource;

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace cmonadic = stormkit::core::monadic;
namespace cmeta    = stormkit::core::meta;

export namespace stormkit::gpu {
    class DescriptorSet;
    class DescriptorSetLayout;
    class DescriptorPool;

    namespace view {
        class DescriptorSet;
        class DescriptorSetLayout;
        class DescriptorPool;
    } // namespace view

    namespace meta {
        template<>
        struct ObjectInfo<DescriptorSet> {
            using Of          = DescriptorSet;
            using ElementType = VkDescriptorSet;
            using DeleterType = decltype(cmonadic::noop());
            using ViewType    = view::DescriptorSet;
            using OwnedBy     = Device;

            static constexpr auto DISABLE_CREATE_ALLOCATE = true;
            static constexpr auto DEBUG_TYPE              = DebugObjectType::DESCRIPTOR_SET;
        };

        template<>
        struct ObjectInfo<DescriptorSetLayout> {
            using Of          = DescriptorSetLayout;
            using ElementType = VkDescriptorSetLayout;
            using DeleterType = PFN_vkDestroyDescriptorSetLayout VolkDeviceTable::*;
            using ViewType    = view::DescriptorSetLayout;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::DESCRIPTOR_SET_LAYOUT;
        };

        template<>
        struct ObjectInfo<DescriptorPool> {
            using Of          = DescriptorPool;
            using ElementType = VkDescriptorPool;
            using DeleterType = PFN_vkDestroyDescriptorPool VolkDeviceTable::*;
            using ViewType    = view::DescriptorPool;
            using OwnedBy     = Device;

            static constexpr auto DEBUG_TYPE = DebugObjectType::DESCRIPTOR_POOL;
        };
    } // namespace meta

    struct BufferDescriptor {
        DescriptorType     type = DescriptorType::UNIFORM_BUFFER;
        u32                binding;
        view::Buffer       buffer;
        std::optional<u32> range  = std::nullopt;
        u32                offset = 0;
    };

    struct ImageDescriptor {
        DescriptorType  type = DescriptorType::COMBINED_IMAGE_SAMPLER;
        u32             binding;
        ImageLayout     layout;
        view::ImageView image_view;
        view::Sampler   sampler;
    };

    using Descriptor = std::variant<BufferDescriptor, ImageDescriptor>;

    class STORMKIT_GPU_API DescriptorSet: public OwnedByDevice<DescriptorSet> {
      public:
        using Deleter = std::function<void(VkDescriptorSet)>;
        ~DescriptorSet() noexcept;

        DescriptorSet(const DescriptorSet&)                    = delete;
        auto operator=(const DescriptorSet&) -> DescriptorSet& = delete;

        DescriptorSet(DescriptorSet&&) noexcept;
        auto operator=(DescriptorSet&&) noexcept -> DescriptorSet&;

        auto update(std::span<const Descriptor> descriptors) const noexcept -> void;

        // clang-format off
  // private:
        // clang-format on
        DescriptorSet(PrivateTag, view::Device&&) noexcept;

      private:
        auto do_init(VkDescriptorSet&&, Deleter&&) noexcept -> void;

        static auto create(view::Device&&, VkDescriptorSet&&, Deleter&&) noexcept -> DescriptorSet;
        static auto allocate(view::Device&&, VkDescriptorSet&&, Deleter&&) noexcept -> Heap<DescriptorSet>;

        Deleter m_deleter;
        friend class DescriptorPool;
        friend class view::DescriptorPool;
    };

    struct DescriptorSetLayoutBinding {
        u32             binding;
        DescriptorType  type;
        ShaderStageFlag stages;
        usize           descriptor_count;
    };

    namespace view {
        class DescriptorSet: public DeviceObject<gpu::DescriptorSet> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::DescriptorSet>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            using DeviceObject<gpu::DescriptorSet>::DeviceObject;
            ~DescriptorSet() noexcept;

            DescriptorSet(const DescriptorSet&) noexcept;
            auto operator=(const DescriptorSet&) noexcept -> DescriptorSet&;

            DescriptorSet(DescriptorSet&&) noexcept;
            auto operator=(DescriptorSet&&) noexcept -> DescriptorSet&;

            auto update(std::span<const gpu::Descriptor> descriptors) const noexcept -> void;
        };
    } // namespace view

    class STORMKIT_GPU_API DescriptorSetLayout: public OwnedByDevice<DescriptorSetLayout> {
      public:
        ~DescriptorSetLayout() noexcept;

        DescriptorSetLayout(const DescriptorSetLayout&)                    = delete;
        auto operator=(const DescriptorSetLayout&) -> DescriptorSetLayout& = delete;

        DescriptorSetLayout(DescriptorSetLayout&&) noexcept;
        auto operator=(DescriptorSetLayout&&) noexcept -> DescriptorSetLayout&;

        [[nodiscard]]
        auto bindings() const noexcept -> std::span<const DescriptorSetLayoutBinding>;

        // clang-format off
  // private:
        // clang-format on
        DescriptorSetLayout(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, std::vector<DescriptorSetLayoutBinding>&&) noexcept -> Expected<void>;

      private:
        std::vector<DescriptorSetLayoutBinding> m_bindings;
    };

    namespace view {
        class DescriptorSetLayout: public DeviceObject<gpu::DescriptorSetLayout> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::DescriptorSetLayout>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            DescriptorSetLayout(const gpu::DescriptorSetLayout& of) noexcept;
            template<cmeta::IsContainerOrPointerOf<gpu::DescriptorSetLayout> T>
            DescriptorSetLayout(const T& of) noexcept;
            ~DescriptorSetLayout() noexcept;

            DescriptorSetLayout(const DescriptorSetLayout&) noexcept;
            auto operator=(const DescriptorSetLayout&) noexcept -> DescriptorSetLayout&;

            DescriptorSetLayout(DescriptorSetLayout&&) noexcept;
            auto operator=(DescriptorSetLayout&&) noexcept -> DescriptorSetLayout&;

            auto bindings() const noexcept -> std::span<const gpu::DescriptorSetLayoutBinding>;

          private:
            std::span<const gpu::DescriptorSetLayoutBinding> m_bindings;
        };
    } // namespace view

    class STORMKIT_GPU_API DescriptorPool: public OwnedByDevice<DescriptorPool> {
      public:
        struct Size {
            DescriptorType type;
            u32            descriptor_count;
        };

        ~DescriptorPool() noexcept;

        DescriptorPool(const DescriptorPool&)                    = delete;
        auto operator=(const DescriptorPool&) -> DescriptorPool& = delete;

        DescriptorPool(DescriptorPool&&) noexcept;
        auto operator=(DescriptorPool&&) noexcept -> DescriptorPool&;

        auto create_descriptor_set(view::DescriptorSetLayout layout) const noexcept -> Expected<DescriptorSet>;
        auto create_descriptor_sets(usize count, view::DescriptorSetLayout layout) const noexcept
          -> Expected<std::vector<DescriptorSet>>;

        auto allocate_descriptor_set(view::DescriptorSetLayout layout) const noexcept -> Expected<Heap<DescriptorSet>>;
        auto allocate_descriptor_sets(usize count, view::DescriptorSetLayout layout) const noexcept
          -> Expected<std::vector<Heap<DescriptorSet>>>;

        // clang-format off
  // private:
        // clang-format on
        DescriptorPool(PrivateTag, view::Device&&) noexcept;
        auto do_init(PrivateTag, std::span<const Size>&&, u32) noexcept -> Expected<void>;

      private:
        auto create_vk_descriptor_sets(usize, view::DescriptorSetLayout&&) const noexcept
          -> Expected<std::vector<VkDescriptorSet>>;

        static auto delete_vk_descriptor_set(view::Device, view::DescriptorPool, VkDescriptorSet) noexcept -> void;

        friend class view::DescriptorPool;
    };

    namespace view {
        class DescriptorPool: public DeviceObject<gpu::DescriptorPool> {
          public:
            using ObjectInfo  = typename meta::ObjectInfo<gpu::DescriptorPool>;
            using ElementType = ObjectInfo::ElementType;
            using ViewType    = ObjectInfo::ViewType;

            using DeviceObject<gpu::DescriptorPool>::DeviceObject;
            ~DescriptorPool() noexcept;

            DescriptorPool(const DescriptorPool&) noexcept;
            auto operator=(const DescriptorPool&) noexcept -> DescriptorPool&;

            DescriptorPool(DescriptorPool&&) noexcept;
            auto operator=(DescriptorPool&&) noexcept -> DescriptorPool&;

            auto create_descriptor_set(DescriptorSetLayout layout) const noexcept -> Expected<gpu::DescriptorSet>;
            auto create_descriptor_sets(usize count, DescriptorSetLayout layout) const noexcept
              -> Expected<std::vector<gpu::DescriptorSet>>;

            auto allocate_descriptor_set(DescriptorSetLayout layout) const noexcept -> Expected<Heap<gpu::DescriptorSet>>;
            auto allocate_descriptor_sets(usize count, DescriptorSetLayout layout) const noexcept
              -> Expected<std::vector<Heap<gpu::DescriptorSet>>>;

          private:
            auto create_vk_descriptor_sets(usize, DescriptorSetLayout&&) const noexcept -> Expected<std::vector<VkDescriptorSet>>;

            static auto delete_vk_descriptor_set(Device, DescriptorPool, VkDescriptorSet) noexcept -> void;
        };
    } // namespace view

    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(view::DescriptorSetLayout value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const DescriptorSetLayoutBinding& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const BufferDescriptor& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const ImageDescriptor& value) noexcept -> Ret;
    template<core::meta::HashType Ret = hash32>
    constexpr auto hasher(const Descriptor& value) noexcept -> Ret;
} // namespace stormkit::gpu

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSet::DescriptorSet(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<DescriptorSet> { std::move(device), cmonadic::noop() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSet::~DescriptorSet() noexcept {
        if (m_vk_handle != VK_NULL_HANDLE) {
            m_deleter(m_vk_handle);
            m_vk_handle = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorSet::operator=(DescriptorSet&& other) noexcept -> DescriptorSet& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorSet::create(view::Device&& device, VkDescriptorSet&& handle, Deleter&& deleter) noexcept
      -> DescriptorSet {
        auto out = DescriptorSet { PRIVATE, std::move(device) };
        out.do_init(std::move(handle), std::move(deleter));
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorSet::allocate(view::Device&& device, VkDescriptorSet&& handle, Deleter&& deleter) noexcept
      -> Heap<DescriptorSet> {
        auto out = core::allocate_unsafe<DescriptorSet>(PRIVATE, std::move(device));
        out->do_init(std::move(handle), std::move(deleter));
        return out;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSet::~DescriptorSet() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSet::DescriptorSet(const DescriptorSet&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorSet::operator=(const DescriptorSet&) noexcept -> DescriptorSet& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSet::DescriptorSet(DescriptorSet&&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorSet::operator=(DescriptorSet&&) noexcept -> DescriptorSet& = default;
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSetLayout::DescriptorSetLayout(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<DescriptorSetLayout> { std::move(device), &VolkDeviceTable::vkDestroyDescriptorSetLayout } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSetLayout::~DescriptorSetLayout() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept -> DescriptorSetLayout& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorSetLayout::bindings() const noexcept -> std::span<const DescriptorSetLayoutBinding> {
        return m_bindings;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSetLayout::DescriptorSetLayout(const gpu::DescriptorSetLayout& of) noexcept
            : DeviceObject<gpu::DescriptorSetLayout> { of }, m_bindings { of.bindings() } {
        }

        ///////////////////////////////////
        ///////////////////////////////////
        template<cmeta::IsContainerOrPointerOf<gpu::DescriptorSetLayout> T>
        STORMKIT_FORCE_INLINE
        inline DescriptorSetLayout::DescriptorSetLayout(const T& of) noexcept
            : DeviceObject<gpu::DescriptorSetLayout> { of }, m_bindings { of->bindings() } {
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSetLayout::~DescriptorSetLayout() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayout&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorSetLayout::operator=(const DescriptorSetLayout&) noexcept -> DescriptorSetLayout& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorSetLayout::operator=(DescriptorSetLayout&&) noexcept -> DescriptorSetLayout& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorSetLayout::bindings() const noexcept -> std::span<const gpu::DescriptorSetLayoutBinding> {
            return m_bindings;
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorPool::DescriptorPool(PrivateTag, view::Device&& device) noexcept
        : OwnedByDevice<DescriptorPool> { std::move(device), &VolkDeviceTable::vkDestroyDescriptorPool } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorPool::~DescriptorPool() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorPool::operator=(DescriptorPool&& other) noexcept -> DescriptorPool& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorPool::create_descriptor_set(view::DescriptorSetLayout layout) const noexcept
      -> Expected<DescriptorSet> {
        auto   vk_handle = Try(create_vk_descriptor_sets(1, std::move(layout))).front();
        Return DescriptorSet::create(auto(device()),
                                     std::move(vk_handle),
                                     bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorPool::create_descriptor_sets(usize count, view::DescriptorSetLayout layout) const noexcept
      -> Expected<std::vector<DescriptorSet>> {
        Return transform(Try(create_vk_descriptor_sets(count, std::move(layout))), [this](auto vk_handle) noexcept {
            return DescriptorSet::create(auto(device()),
                                         std::move(vk_handle),
                                         bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorPool::allocate_descriptor_set(view::DescriptorSetLayout layout) const noexcept
      -> Expected<Heap<DescriptorSet>> {
        auto   vk_handle = Try(create_vk_descriptor_sets(1, std::move(layout))).front();
        Return DescriptorSet::allocate(auto(device()),
                                       std::move(vk_handle),
                                       bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto DescriptorPool::allocate_descriptor_sets(usize count, view::DescriptorSetLayout layout) const noexcept
      -> Expected<std::vector<Heap<DescriptorSet>>> {
        Return transform(Try(create_vk_descriptor_sets(count, std::move(layout))), [this](auto vk_handle) noexcept {
            return DescriptorSet::allocate(auto(device()),
                                           std::move(vk_handle),
                                           bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
        });
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorPool::~DescriptorPool() noexcept = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorPool::DescriptorPool(const DescriptorPool&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::operator=(const DescriptorPool&) noexcept -> DescriptorPool& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline DescriptorPool::DescriptorPool(DescriptorPool&&) noexcept = default;
        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::operator=(DescriptorPool&&) noexcept -> DescriptorPool& = default;

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::create_descriptor_set(DescriptorSetLayout layout) const noexcept
          -> Expected<gpu::DescriptorSet> {
            auto   vk_handle = Try(create_vk_descriptor_sets(1, std::move(layout))).front();
            Return gpu::DescriptorSet::create(auto(device()),
                                              std::move(vk_handle),
                                              bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::create_descriptor_sets(usize count, DescriptorSetLayout layout) const noexcept
          -> Expected<std::vector<gpu::DescriptorSet>> {
            Return transform(Try(create_vk_descriptor_sets(count, std::move(layout))), [this](auto vk_handle) noexcept {
                return gpu::DescriptorSet::create(auto(device()),
                                                  std::move(vk_handle),
                                                  bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
            });
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::allocate_descriptor_set(DescriptorSetLayout layout) const noexcept
          -> Expected<Heap<gpu::DescriptorSet>> {
            auto   vk_handle = Try(create_vk_descriptor_sets(1, std::move(layout))).front();
            Return gpu::DescriptorSet::allocate(auto(device()),
                                                std::move(vk_handle),
                                                bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
        }

        /////////////////////////////////////
        /////////////////////////////////////
        STORMKIT_FORCE_INLINE
        inline auto DescriptorPool::allocate_descriptor_sets(usize count, DescriptorSetLayout layout) const noexcept
          -> Expected<std::vector<Heap<gpu::DescriptorSet>>> {
            Return transform(Try(create_vk_descriptor_sets(count, std::move(layout))), [this](auto vk_handle) noexcept {
                return gpu::DescriptorSet::allocate(auto(device()),
                                                    std::move(vk_handle),
                                                    bind_front(delete_vk_descriptor_set, device(), as_view(*this)));
            });
        }
    } // namespace view

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(view::DescriptorSetLayout value) noexcept -> Ret {
        auto out = Ret {};
        for (const auto binding : value.bindings()) out = hash_combine(out, binding);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const DescriptorSetLayoutBinding& value) noexcept -> Ret {
        return hash<Ret>(value.binding, value.type, value.stages, value.descriptor_count);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const BufferDescriptor& value) noexcept -> Ret {
        return hash<Ret>(value.type, value.binding, value.buffer, value.range, value.offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const ImageDescriptor& value) noexcept -> Ret {
        return hash<Ret>(value.type, value.binding, value.layout, value.image_view, value.sampler);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::meta::HashType Ret>
    STORMKIT_FORCE_INLINE
    constexpr auto hasher(const Descriptor& value) noexcept -> Ret {
        return std::visit([](const auto& descriptor) static noexcept { return hash<Ret>(descriptor); }, value);
    }
} // namespace stormkit::gpu
