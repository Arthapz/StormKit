// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void Device::waitForFence(const Fence &fence,
                                     std::uint64_t timeout) const noexcept {
        auto fences = std::array{std::cref(fence)};
        waitForFences(fences, true, timeout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template <
        typename Container,
        std::enable_if_t<!std::is_same_v<Container, core::filesystem::path>>>
    ShaderOwnedPtr Device::createShader(const Container &container,
                                        ShaderType type) const {
        return createShader(std::data(container), std::size(container), type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr
        Device::createVertexBuffer(std::size_t size, MemoryProperty property,
                                   bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Vertex | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Vertex,
                                    size, property);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr
        Device::createIndexBuffer(std::size_t size, MemoryProperty property,
                                  bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Index | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Index,
                                    size, property);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr
        Device::createUniformBuffer(std::size_t size, MemoryProperty property,
                                    bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Uniform | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging)
                                        ? with_staging_buffer_usage
                                        : HardwareBufferUsage::Uniform,
                                    size, property);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr
        Device::createStagingBuffer(std::size_t size,
                                    MemoryProperty property) const {
        return createHardwareBuffer(HardwareBufferUsage::Transfert_Src, size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::graphicsQueue() const noexcept {
        return *m_graphics_queue;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::transfertQueue() const noexcept {
        return *m_transfert_queue;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::computeQueue() const noexcept {
        return *m_compute_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const PhysicalDevice &Device::physicalDevice() const noexcept {
        return *m_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline VkDevice Device::vkDevice() const noexcept {
        STORM_EXPECTS(m_vk_device != VK_NULL_HANDLE);
        return m_vk_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Device::operator VkDevice() const noexcept {
        STORM_EXPECTS(m_vk_device != VK_NULL_HANDLE);
        return m_vk_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const VolkDeviceTable &Device::vkDeviceTable() const noexcept {
        return m_device_table;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline VmaAllocator Device::vmaAllocator() const noexcept {
        return m_vma_allocator;
    }
} // namespace storm::render
