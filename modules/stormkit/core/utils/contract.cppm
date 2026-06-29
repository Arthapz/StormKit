// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

#ifndef STORMKIT_ASSERT
    #define STORMKIT_ASSERT 1
#endif

export module stormkit.core:utils.contract;

import std;
import frozen;

import :utils.stracktrace;
import :containers.aliases;
import :string.aliases;

import :meta;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {
    enum class AssertType {
        Assertion,
        PreCondition,
        PostCondition,
    };

    constexpr auto as_string(AssertType type) noexcept -> string_view;
    constexpr auto to_string(AssertType type) noexcept -> string;

    STORMKIT_CORE_API
    auto assert_base(bool                        cond,
                     AssertType                  type,
                     string_view                 message,
                     const std::source_location& location = std::source_location::current()) noexcept -> void;

    consteval auto consteval_assert_base(bool cond, AssertType type, string_view message) noexcept -> void;

    constexpr auto assert(bool                        cond,
                          string_view                 message,
                          const std::source_location& location = std::source_location::current()) noexcept -> void;

    constexpr auto assert(bool cond, const std::source_location& location = std::source_location::current()) noexcept -> void;

    constexpr auto expects(bool                        cond,
                           string_view                 message,
                           const std::source_location& location = std::source_location::current()) noexcept -> void;

    constexpr auto expects(bool cond, const std::source_location& location = std::source_location::current()) noexcept -> void;

    constexpr auto ensures(bool                        cond,
                           string_view                 message,
                           const std::source_location& location = std::source_location::current()) noexcept -> void;

    constexpr auto ensures(bool cond, const std::source_location& location = std::source_location::current()) noexcept -> void;

    namespace casts::core {
        template<meta::SameAsAnyOf<string, string_view> To>
        [[nodiscard]]
        constexpr auto as(AssertType t) noexcept -> To;
    }
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

using namespace std::literals;
using namespace frozen::string_literals;

namespace stormkit { inline namespace core {
    namespace casts::core {
        constexpr auto AssertTypeToContractName = frozen::make_unordered_map<AssertType, frozen::string>({
          { AssertType::Assertion,     "Contract check"_s       },
          { AssertType::PreCondition,  "Pre condition check"_s  },
          { AssertType::PostCondition, "Post condition check"_s },
        });

    } // namespace casts::core

    struct StringLiteral {
        array<char, 512> buff;
        std::size_t      size;

        consteval auto view() noexcept -> string_view { return { std::data(buff), size }; }
    };

    auto constevalFailure(StringLiteral) -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto as_string(AssertType type) noexcept -> string_view {
        const auto t = casts::core::AssertTypeToContractName.at(type);
        return { stdr::data(t), stdr::size(t) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE STORMKIT_CONST
    constexpr auto to_string(AssertType type) noexcept -> string {
        const auto t = casts::core::AssertTypeToContractName.at(type);
        return { stdr::data(t), stdr::size(t) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    consteval auto generateConstevalMessage(AssertType type, string_view message) noexcept -> StringLiteral {
        auto       result = StringLiteral {};
        const auto str    = "[Assertion]"s + to_string(type) + ": " + string { message };
        std::ranges::copy(str, std::begin(result.buff));
        result.size = std::size(str);
        return result;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    consteval auto consteval_assert_base(bool cond, AssertType type, string_view message) noexcept -> void {
        if (not cond) [[unlikely]] { constevalFailure(generateConstevalMessage(type, message)); }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto assert(bool cond, string_view message, [[maybe_unused]] const std::source_location& location) noexcept
      -> void {
        if consteval {
            consteval_assert_base(cond, AssertType::Assertion, message);
        } else {
            assert_base(cond, AssertType::Assertion, message, location);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto assert(bool cond, const std::source_location& location) noexcept -> void {
        assert(cond, "", location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto expects(bool cond, string_view message, const std::source_location& location) noexcept -> void {
        if consteval {
            consteval_assert_base(cond, AssertType::PreCondition, message);
        } else {
            assert_base(cond, AssertType::PreCondition, message, location);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto expects(bool cond, const std::source_location& location) noexcept -> void {
        expects(cond, "", location);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto ensures(bool cond, string_view message, const std::source_location& location) noexcept -> void {
        if consteval {
            consteval_assert_base(cond, AssertType::PostCondition, message);
        } else {
            assert_base(cond, AssertType::PostCondition, message, location);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    constexpr auto ensures(bool cond, const std::source_location& location) noexcept -> void {
        ensures(cond, "", location);
    }
}} // namespace stormkit::core
