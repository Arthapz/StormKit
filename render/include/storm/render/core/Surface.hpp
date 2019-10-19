// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Fwd.hpp>
#include <storm/render/sync/Semaphore.hpp>

#include <storm/window/Window.hpp>

namespace storm::render {
    class STORM_PUBLIC Surface: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Surface;

        enum class Buffering { Simple, Double, Triple, Swapchain };

        struct Frame {
            core::UInt32 current_frame;
            core::UInt32 texture_index;

            SemaphoreConstObserverPtr texture_available;
            SemaphoreConstObserverPtr render_finished;
            FenceObserverPtr in_flight;
        };

        Surface(const window::Window &window,
                const Instance &instance,
                Buffering buffering = Buffering::Triple);
        ~Surface();

        Surface(Surface &&);
        Surface &operator=(Surface &&);

        void initialize(const Device &device);
        void recreate();
        void destroy();

        Frame acquireNextFrame();
        void present(const Frame &frame);

        inline storm::core::span<Texture> textures() noexcept;
        inline storm::core::span<const Texture> textures() const noexcept;

        inline PixelFormat pixelFormat() const noexcept;
        inline core::UInt32 bufferingCount() const noexcept;
        inline core::UInt32 textureCount() const noexcept;

        inline bool needRecreate() const noexcept;

        inline const core::Extentu &extent() const noexcept;

        inline vk::SurfaceKHR vkSurface() const noexcept;
        inline operator vk::SurfaceKHR() const noexcept;
        inline vk::SurfaceKHR vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      public:
        void createSwapchain();
        void destroySwapchain();

        vk::SurfaceFormatKHR
            chooseSwapSurfaceFormat(storm::core::span<const vk::SurfaceFormatKHR> formats) noexcept;
        vk::PresentModeKHR chooseSwapPresentMode(
            storm::core::span<const vk::PresentModeKHR> present_modes) noexcept;
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept;
        core::UInt32 chooseImageCount(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept;

        InstanceConstObserverPtr m_instance;
        DeviceConstObserverPtr m_device      = nullptr;
        QueueConstObserverPtr m_graphics_queue = nullptr;

        window::WindowConstObserverPtr m_window;

        vk::UniqueSurfaceKHR m_vk_surface;
        RAIIVkSwapchain m_vk_swapchain;
        RAIIVkSwapchain m_vk_old_swapchain;

        std::vector<vk::Image> m_vk_images;

        core::Extentu m_extent;
        PixelFormat m_pixel_format = PixelFormat::Undefined;
        core::UInt32 m_image_count = 1;

        Buffering m_buffering;
        core::UInt32 m_buffering_count;

        TextureArray m_textures;

        SemaphoreArray m_texture_availables;
        SemaphoreArray m_render_finisheds;
        FenceArray m_in_flight_fences;

        bool m_need_recreate = false;

        core::UInt32 m_current_frame = 0;
    };
} // namespace storm::render

#include "Surface.inl"
