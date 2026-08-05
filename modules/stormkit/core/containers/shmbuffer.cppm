// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/contract_macro.hpp>
#include <stormkit/core/flags_macro.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.containers.shmbuffer;

import std;

import stormkit.core.typesafe;
import stormkit.core.functional;
import stormkit.core.contract;
import stormkit.core.types;
import stormkit.core.heap;
import stormkit.core.meta.concepts;
import stormkit.core.meta.type_manipulation;
import stormkit.core.filesystem;
import stormkit.core.private_tag;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {
    class STORMKIT_CORE_API SHMBuffer final {
        using private_tag = private_tag<SHMBuffer>;

      public:
        using ValueType = byte;

        template<typename T>
        using ExpectedType = std::expected<T, std::error_code>;

        using value_type = ValueType;

        static auto create(usize size, string name, io::Access access = io::Access::READ | io::Access::WRITE) noexcept
          -> ExpectedType<SHMBuffer>;
        static auto allocate(usize size, string name, io::Access access = io::Access::READ | io::Access::WRITE) noexcept
          -> ExpectedType<heap_ptr<SHMBuffer>>;

        explicit SHMBuffer(private_tag) noexcept;
        ~SHMBuffer();

        SHMBuffer(const SHMBuffer&)                    = delete;
        auto operator=(const SHMBuffer&) -> SHMBuffer& = delete;

        SHMBuffer(SHMBuffer&&) noexcept;
        auto operator=(SHMBuffer&&) noexcept -> SHMBuffer&;

        template<typename Self>
        auto begin(this Self&) noexcept -> decltype(auto);
        auto cbegin() const noexcept -> decltype(auto);

        template<typename Self>
        auto end(this Self&) noexcept -> decltype(auto);
        auto cend() const noexcept -> decltype(auto);

        template<typename Self>
        auto operator[](this Self&, usize index) noexcept -> meta::ForwardConst<Self, ValueType>&;
        template<typename Self>
        auto at(this Self&, usize index) noexcept -> meta::ForwardConst<Self, ValueType>&;

        auto size() const noexcept -> usize;
        template<typename Self>
        auto data(this Self&) noexcept -> array_view<meta::ForwardConst<Self, ValueType>>;
        template<typename Self>
        auto native_handle(this Self&) noexcept -> meta::ForwardConst<Self, void>*;
        auto name() const noexcept -> string_view;
        auto access() const noexcept -> io::Access;

      private:
        auto do_init(usize, string, io::Access) noexcept -> ExpectedType<void>;

        io::Access            m_access;
        void*                 m_handle = nullptr;
        usize                 m_size;
        string                m_name;
        array_view<ValueType> m_data;
    };
}} // namespace stormkit::core

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::create(usize size, string name, io::Access access) noexcept -> ExpectedType<SHMBuffer> {
        auto out = SHMBuffer { PRIVATE<SHMBuffer> };
        out.do_init(size, std::move(name), access);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::allocate(usize size, string name, io::Access access) noexcept -> ExpectedType<heap_ptr<SHMBuffer>> {
        auto out = allocate_unsafe<SHMBuffer>(PRIVATE<SHMBuffer>);
        out->do_init(size, std::move(name), access);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SHMBuffer::SHMBuffer(private_tag) noexcept {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline SHMBuffer::SHMBuffer(SHMBuffer&& other) noexcept
        : m_access { other.access() },
          m_handle { std::exchange(other.m_handle, nullptr) },
          m_size { std::exchange(other.m_size, 0u) },
          m_name { std::exchange(other.m_name, {}) },
          m_data { std::exchange(other.m_data, {}) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::operator=(SHMBuffer&& other) noexcept -> SHMBuffer& {
        if (&other == this) [[unlikely]]
            return *this;

        m_access = other.m_access;
        m_handle = std::exchange(other.m_handle, nullptr);
        m_data   = std::exchange(other.m_data, {});
        m_size   = std::exchange(other.m_size, 0u);
        m_name   = std::exchange(other.m_name, {});

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::begin(this Self& self) noexcept -> decltype(auto) {
        EXPECTS(self.m_handle);
        return stdr::begin(std::forward<Self&>(self).m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::cbegin() const noexcept -> decltype(auto) {
        EXPECTS(m_handle);
        return stdr::cbegin(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::end(this Self& self) noexcept -> decltype(auto) {
        EXPECTS(self.m_handle);
        return stdr::end(std::forward<Self&>(self).m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::cend() const noexcept -> decltype(auto) {
        EXPECTS(m_handle);
        return stdr::cend(m_data);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::operator[](this Self& self, usize index) noexcept -> meta::ForwardConst<Self, ValueType>& {
        EXPECTS(self.m_handle);
        EXPECTS(index < self.m_size);
        return std::forward<Self&>(self).m_data[index];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::at(this Self& self, usize index) noexcept -> meta::ForwardConst<Self, ValueType>& {
        EXPECTS(self.m_handle);
        EXPECTS(index < self.m_size);
        return std::forward<Self&>(self).m_data.at(index);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::size() const noexcept -> usize {
        return m_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::data(this Self& self) noexcept -> array_view<meta::ForwardConst<Self, ValueType>> {
        EXPECTS(self.m_handle);
        return std::forward<Self&>(self).m_data;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Self>
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::native_handle(this Self& self) noexcept -> meta::ForwardConst<Self, void>* {
        return std::forward<Self&>(self).m_handle;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::name() const noexcept -> string_view {
        return m_name;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto SHMBuffer::access() const noexcept -> io::Access {
        return m_access;
    }
}} // namespace stormkit::core
