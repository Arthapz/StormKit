// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/memory_macro.hpp>

#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:utils.handle;

import std;

import :hash;
import :typesafe.safecasts;
import :typesafe.integer;
import :meta;

export namespace stormkit { inline namespace core {
    template<class T, meta::IsIntegral _ID>
    struct Handle {
        using ID = _ID;

        static constexpr auto INVALID_HANDLE_VALUE = std::numeric_limits<ID>::max();

        [[nodiscard]]
        constexpr auto operator<=>(const Handle<T, ID>&) const noexcept -> std::strong_ordering = default;

        template<std::derived_from<T> U>
        constexpr operator Handle<U, ID>() const noexcept;

        constexpr auto& operator++() noexcept;

        constexpr auto operator++(int) noexcept;

        constexpr auto& operator--() noexcept;

        constexpr auto operator--(int) noexcept;

        constexpr operator ID() const noexcept;

        [[nodiscard]]
        static constexpr auto invalid_handle() noexcept -> Handle;

        ID id = INVALID_HANDLE_VALUE;
    };

    template<class T>
    using Handle32 = Handle<T, u32>;

    template<class T>
    using Handle64 = Handle<T, u64>;

    template<meta::HashType Ret = hash32, class T, meta::IsIntegral _ID>
    constexpr auto hasher(const Handle<T, _ID>& value) noexcept -> Ret;

    template<class T, meta::IsIntegral _ID, typename FormatContext>
    auto format_as(const Handle<T, _ID>& value, FormatContext& ctx) noexcept -> decltype(ctx.out());
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    template<std::derived_from<T> U>
    constexpr Handle<T, _ID>::operator Handle<U, _ID>() const noexcept {
        return Handle<U, _ID> { id };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr auto& Handle<T, _ID>::operator++() noexcept {
        ++id;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr auto Handle<T, _ID>::operator++(int) noexcept {
        auto old = *this;
        operator++();
        return old;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr auto& Handle<T, _ID>::operator--() noexcept {
        --id;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr auto Handle<T, _ID>::operator--(int) noexcept {
        auto old = *this;
        operator--();
        return old;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr Handle<T, _ID>::operator ID() const noexcept {
        return id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID>
    constexpr auto Handle<T, _ID>::invalid_handle() noexcept -> Handle {
        return Handle {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<meta::HashType Ret, class T, meta::IsIntegral _ID>
    constexpr auto hasher(const Handle<T, _ID>& value) noexcept -> Ret {
        return hash<Ret>(value.id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, meta::IsIntegral _ID, typename FormatContext>
    auto format_as(const Handle<T, _ID>& value, FormatContext& ctx) noexcept -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[handle id: {}]", value.id);
    }
}} // namespace stormkit::core
