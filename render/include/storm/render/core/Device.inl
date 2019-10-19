// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline void Device::waitForFence(const Fence &fence, core::UInt64 timeout) const noexcept {
        auto fences = std::array { std::cref(fence) };
        waitForFences(fences, true, timeout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Container,
             std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
    Shader Device::createShader(const Container &container, ShaderStage type) const {
        return createShader({ std::data(container), std::size(container) }, type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Container,
             std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
    ShaderOwnedPtr Device::createShaderPtr(const Container &container, ShaderStage type) const {
        return createShaderPtr({ std::data(container), std::size(container) }, type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBuffer Device::createVertexBuffer(core::ArraySize size,
                                                     MemoryProperty property,
                                                     bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Vertex | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Vertex,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferArray Device::createVertexBuffers(core::ArraySize count,
                                                           core::ArraySize size,
                                                           MemoryProperty property,
                                                           bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createVertexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr Device::createVertexBufferPtr(core::ArraySize size,
                                                                MemoryProperty property,
                                                                bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Vertex | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Vertex,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtrArray Device::createVertexBuffersPtr(core::ArraySize count,
                                                                      core::ArraySize size,
                                                                      MemoryProperty property,
                                                                      bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createVertexBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBuffer Device::createIndexBuffer(core::ArraySize size,
                                                    MemoryProperty property,
                                                    bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Index | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Index,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferArray Device::createIndexBuffers(core::ArraySize count,
                                                          core::ArraySize size,
                                                          MemoryProperty property,
                                                          bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createIndexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr Device::createIndexBufferPtr(core::ArraySize size,
                                                               MemoryProperty property,
                                                               bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Index | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Index,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtrArray Device::createIndexBuffersPtr(core::ArraySize count,
                                                                     core::ArraySize size,
                                                                     MemoryProperty property,
                                                                     bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createIndexBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBuffer Device::createUniformBuffer(core::ArraySize size,
                                                      MemoryProperty property,
                                                      bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Uniform | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Uniform,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferArray Device::createUniformBuffers(core::ArraySize count,
                                                            core::ArraySize size,
                                                            MemoryProperty property,
                                                            bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createUniformBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr Device::createUniformBufferPtr(core::ArraySize size,
                                                                 MemoryProperty property,
                                                                 bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Uniform | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Uniform,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtrArray Device::createUniformBuffersPtr(core::ArraySize count,
                                                                       core::ArraySize size,
                                                                       MemoryProperty property,
                                                                       bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createUniformBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBuffer Device::createStagingBuffer(core::ArraySize size,
                                                      MemoryProperty property) const {
        return createHardwareBuffer(HardwareBufferUsage::Transfert_Src, size, property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferArray Device::createStagingBuffers(core::ArraySize count,
                                                            core::ArraySize size,
                                                            MemoryProperty property) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i) buffers.emplace_back(createStagingBuffer(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtr Device::createStagingBufferPtr(core::ArraySize size,
                                                                 MemoryProperty property) const {
        return createHardwareBufferPtr(HardwareBufferUsage::Transfert_Src, size, property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferOwnedPtrArray
        Device::createStagingBuffersPtr(core::ArraySize count,
                                        core::ArraySize size,
                                        MemoryProperty property) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createStagingBufferPtr(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::graphicsQueue() const noexcept { return *m_graphics_queue; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::asyncTransfertQueue() const noexcept {
        STORM_EXPECTS(m_async_transfert_queue != nullptr);

        return *m_async_transfert_queue;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Queue &Device::asyncComputeQueue() const noexcept {
        STORM_EXPECTS(m_async_compute_queue != nullptr);

        return *m_async_compute_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Device::hasAsyncTransfertQueue() const noexcept {
        return m_async_transfert_queue != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline bool Device::hasAsyncComputeQueue() const noexcept {
        return m_async_compute_queue != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const PhysicalDevice &Device::physicalDevice() const noexcept {
        return *m_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Device Device::vkDevice() const noexcept {
        STORM_EXPECTS(m_vk_device);
        return *m_vk_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Device::operator vk::Device() const noexcept { return vkDevice(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Device Device::vkHandle() const noexcept { return vkDevice(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 Device::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDevice_T *());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline VmaAllocator Device::vmaAllocator() const noexcept { return m_vma_allocator; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const vk::DispatchLoaderDynamic &Device::vkDispatcher() const noexcept {
        return m_vk_dispatcher;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    void Device::setObjectName(const T &object, std::string_view name) const {
        setObjectName(object.vkDebugHandle(), T::DEBUG_TYPE, name);
    }
} // namespace storm::render
