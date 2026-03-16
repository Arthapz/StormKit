// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/try_expected.hpp>

#include <stormkit/gpu/vulkan.hpp>

module stormkit.gpu.resource;

import std;

import stormkit.core;

import stormkit.gpu.core;

namespace stormkit::gpu {
    struct BufferAPI {
        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsOwnedOrView BufferType>
        static auto map(BufferType& buffer, ioffset offset) noexcept -> Expected<byte*> {
            EXPECTS(buffer.allocation() and buffer.native_handle());
            EXPECTS(offset < as<ioffset>(buffer.size()));

            const auto& device     = buffer.device();
            const auto& allocator  = device.allocator();
            const auto& allocation = buffer.allocation();

            auto ptr = Try(vk::call_checked<void*>(vmaMapMemory, allocator, allocation));

            buffer.m_mapped_pointer = std::bit_cast<byte*>(ptr);
            buffer.m_mapped_pointer += offset;
            return buffer.m_mapped_pointer;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsOwnedOrView BufferType>
        static auto flush(const BufferType& buffer, ioffset offset, usize size) noexcept -> Expected<void> {
            EXPECTS(buffer.allocation() and buffer.native_handle());
            EXPECTS(offset <= as<ioffset>(buffer.size()));
            EXPECTS(size <= buffer.size());

            const auto& device     = buffer.device();
            const auto& allocator  = device.allocator();
            const auto& allocation = buffer.allocation();

            return vk::call_checked(vmaFlushAllocation, allocator, allocation, offset, size);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsOwnedOrView BufferType>
        static auto unmap(BufferType& buffer) noexcept -> void {
            EXPECTS(buffer.allocation() and buffer.native_handle());
            // expects(buffer.is_persistently_mapped(), "unmapping persistent buffer !");

            const auto& device     = buffer.device();
            const auto& allocator  = device.allocator();
            const auto& allocation = buffer.allocation();

            vk::call(vmaUnmapMemory, allocator, allocation);

            buffer.m_mapped_pointer = nullptr;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<meta::IsOwnedOrView BufferType>
        static auto upload(BufferType& buffer, std::span<const byte> data, ioffset offset) noexcept -> Expected<void> {
            EXPECTS(stdr::size(data) <= buffer.size());

            if (buffer.is_persistently_mapped()) {
                stdr::copy(data, buffer.m_mapped_pointer);
                Return {};
            }

            auto gpu_data = Try(buffer.map(offset, stdr::size(data)));
            stdr::copy(data, stdr::begin(gpu_data));
            buffer.unmap();

            Return {};
        }
    };

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::map(ioffset offset) noexcept -> Expected<byte*> {
        return BufferAPI::map(*this, offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::flush(ioffset offset, usize size) noexcept -> Expected<void> {
        return BufferAPI::flush(*this, offset, size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::unmap() noexcept -> void {
        return BufferAPI::unmap(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::upload(std::span<const byte> data, ioffset offset) noexcept -> Expected<void> {
        return BufferAPI::upload(*this, std::move(data), offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::do_init(PrivateTag, const CreateInfo& _create_info) noexcept -> Expected<void> {
        m_usages                 = _create_info.usages;
        m_size                   = _create_info.size;
        m_memory_properties      = _create_info.properties;
        m_is_persistently_mapped = _create_info.persistently_mapped;

        const auto& device       = this->device();
        const auto& device_table = device.device_table();
        const auto  create_info  = VkBufferCreateInfo {
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = 0,
            .size                  = m_size,
            .usage                 = vk::to_vk<VkBufferUsageFlags>(m_usages),
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
        };
        m_vk_handle = Try(vk::call_checked<VkBuffer>(device_table.vkCreateBuffer, device, &create_info, nullptr));

        const auto vma_create_info = VmaAllocationCreateInfo {
            .flags          = 0,
            .usage          = VMA_MEMORY_USAGE_UNKNOWN,
            .requiredFlags  = vk::to_vk<VkMemoryPropertyFlags>(m_memory_properties),
            .preferredFlags = 0,
            .memoryTypeBits = 0,
            .pool           = nullptr,
            .pUserData      = nullptr,
            .priority       = 0
        };
        const auto allocator = device.allocator();
        auto       out       = VmaAllocation { VK_NULL_HANDLE };
        Try(vk::call_checked(vmaAllocateMemoryForBuffer, allocator, m_vk_handle, &vma_create_info, &out, nullptr));
        m_vma_allocation = { [allocator](VmaAllocation handle) noexcept {
            if (handle) { vmaFreeMemory(allocator, handle); }
        } };
        m_vma_allocation = std::move(out);
        Try(vk::call_checked(vmaBindBufferMemory, allocator, m_vma_allocation, m_vk_handle));

        if (m_is_persistently_mapped) Try(map(0u));

        Return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::find_memory_type(u32                                     type_filter,
                                  VkMemoryPropertyFlagBits                properties,
                                  const VkPhysicalDeviceMemoryProperties& mem_properties,
                                  const VkMemoryRequirements&) noexcept -> u32 {
        for (const auto i : range(mem_properties.memoryTypeCount)) {
            if ((type_filter & (1 << i))
                and (check_flag_bit(static_cast<VkMemoryPropertyFlagBits>(mem_properties.memoryTypes[i].propertyFlags),
                                    properties)))
                return i;
        }

        return 0;
    }

    namespace view {
        /////////////////////////////////////
        /////////////////////////////////////
        auto Buffer::map(ioffset offset) noexcept -> Expected<byte*> {
            return BufferAPI::map(*this, offset);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Buffer::flush(ioffset offset, usize size) noexcept -> Expected<void> {
            return BufferAPI::flush(*this, offset, size);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Buffer::unmap() noexcept -> void {
            return BufferAPI::unmap(*this);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto Buffer::upload(std::span<const byte> data, ioffset offset) noexcept -> Expected<void> {
            return BufferAPI::upload(*this, std::move(data), offset);
        }
    } // namespace view
} // namespace stormkit::gpu
