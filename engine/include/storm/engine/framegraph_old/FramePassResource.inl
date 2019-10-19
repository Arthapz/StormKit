// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    inline bool FramePassBaseResource::isTransient() const noexcept { return m_is_transient; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline std::string_view FramePassBaseResource::name() const noexcept { return m_name; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline const FramePassBaseConstObserverPtr &FramePassBaseResource::createdBy() const noexcept {
        return m_create_by;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline FramePassBaseConstObserverPtrConstSpan FramePassBaseResource::readIn() const noexcept {
        return m_read_in;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////

    inline FramePassBaseConstObserverPtrConstSpan FramePassBaseResource::writeIn() const noexcept {
        return m_write_in;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename Description>
    FramePassResource<Description>::FramePassResource(std::string name,
                                                      Description &&description,
                                                      bool transient) noexcept
        : FramePassBaseResource { std::move(name), transient }, m_description {
              std::forward<Description>(description)
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
    template<typename Description>
    inline const Description &FramePassResource<Description>::description() const noexcept {
        return m_description;
    }
} // namespace storm::engine
