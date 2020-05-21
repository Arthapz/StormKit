// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorSet &BindableBase::descriptorSet() const noexcept {
        STORM_EXPECTS(m_descriptor_sets[m_current_descriptor_set]);

        return *m_descriptor_sets[m_current_descriptor_set];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const std::optional<core::UOffset> &BindableBase::offset() const noexcept { return m_offset; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    render::DescriptorSet &BindableBase::descriptorSet() noexcept {
        STORM_EXPECTS(m_descriptor_sets[m_current_descriptor_set]);

        return *m_descriptor_sets[m_current_descriptor_set];
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    void BindableBase::next() noexcept {
        m_current_descriptor_set += 1;

        const auto size = std::size(m_descriptor_sets);
        if (m_current_descriptor_set >= size) m_current_descriptor_set -= size;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    StaticBindable<T>::StaticBindable() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    StaticBindable<T>::~StaticBindable() = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    StaticBindable<T>::StaticBindable(StaticBindable &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    StaticBindable<T> &StaticBindable<T>::operator=(StaticBindable &&) = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    void StaticBindable<T>::initDescriptorLayout(const render::Device &device,
                                                 const InitFunc &func) {
        func(device, s_descriptor_set_layout);
    }
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    const render::DescriptorSetLayout &StaticBindable<T>::descriptorLayout() const noexcept {
        STORM_EXPECTS(s_descriptor_set_layout);

        return *s_descriptor_set_layout;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    const render::DescriptorSetLayout &Bindable::descriptorLayout() const noexcept {
        if (std::holds_alternative<render::DescriptorSetLayoutOwnedPtr>(m_descriptor_set_layout))
            return *std::get<render::DescriptorSetLayoutOwnedPtr>(m_descriptor_set_layout);
        else
            return *std::get<render::DescriptorSetLayoutConstObserverPtr>(m_descriptor_set_layout);
    }
} // namespace storm::engine
