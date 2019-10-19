// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::Texture &FrameGraphTexture::texture() noexcept {
        if (std::holds_alternative<render::TextureOwnedPtr>(storage))
            return *std::get<render::TextureOwnedPtr>(storage);
        else
            return *std::get<render::TextureObserverPtr>(storage);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::Texture &FrameGraphTexture::texture() const noexcept {
        if (std::holds_alternative<render::TextureOwnedPtr>(storage))
            return *std::get<render::TextureOwnedPtr>(storage);
        else
            return *std::get<render::TextureObserverPtr>(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline render::HardwareBuffer &FrameGraphBuffer::buffer() noexcept {
        if (std::holds_alternative<render::HardwareBufferOwnedPtr>(storage))
            return *std::get<render::HardwareBufferOwnedPtr>(storage);
        else
            return *std::get<render::HardwareBufferObserverPtr>(storage);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const render::HardwareBuffer &FrameGraphBuffer::buffer() const noexcept {
        if (std::holds_alternative<render::HardwareBufferOwnedPtr>(storage))
            return *std::get<render::HardwareBufferOwnedPtr>(storage);
        else
            return *std::get<render::HardwareBufferObserverPtr>(storage);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::string_view FramePassResourceBase::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt32 FramePassResourceBase::id() const noexcept { return m_id; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassResourceBase::isTransient() const noexcept { return m_is_transient; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt32 FramePassResourceBase::createdBy() const noexcept { return m_created_by; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::UInt32 FramePassResourceBase::destroyedIn() const noexcept {
        return m_destroyed_in;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::span<const core::UInt32> FramePassResourceBase::readIn() const noexcept {
        return m_read_in;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline core::span<const core::UInt32> FramePassResourceBase::writeIn() const noexcept {
        return m_write_in;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResource<Resource>::FramePassResource(std::string name,
                                                   core::UInt32 id,
                                                   Descriptor &&descriptor) noexcept
        : FramePassResourceBase { std::move(name), id, true }, m_descriptor {
              std::forward<Descriptor>(descriptor)
          } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    FramePassResource<Resource>::FramePassResource(std::string name,
                                                   core::UInt32 id,
                                                   Descriptor &&description,
                                                   Resource &&resource) noexcept
        : FramePassResourceBase { std::move(name), id, false },
          m_descriptor { std::forward<Descriptor>(description) }, m_resource {
              std::forward<Resource>(resource)
          } {}

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Description>
    FramePassResource<Description>::~FramePassResource() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Description>
    FramePassResource<Description>::FramePassResource(FramePassResource &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Description>
    FramePassResource<Description> &
        FramePassResource<Description>::operator=(FramePassResource &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    inline const typename Resource::Descriptor &FramePassResource<Resource>::descriptor() const
        noexcept {
        return m_descriptor;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    inline void FramePassResource<Resource>::realize(const render::Device &device) noexcept {
        if (m_is_transient) m_resource.realize(device, m_descriptor);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    inline void FramePassResource<Resource>::derealize() noexcept {
        if (m_is_transient) m_resource.derealize();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    inline Resource &FramePassResource<Resource>::resource() noexcept {
        return m_resource;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Resource>
    inline const Resource &FramePassResource<Resource>::resource() const noexcept {
        return m_resource;
    }
} // namespace storm::engine
