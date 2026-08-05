// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core.meta.tag_invoke;

import std;

import stormkit.core.meta.type_manipulation;

namespace stormkit { inline namespace core { namespace meta::details {
    auto tag_invoke(auto&&...) = delete ("No default implementation for this CPO!");
}}} // namespace stormkit::core::meta::details

template<typename... Ts>
struct print_types;

export namespace stormkit { inline namespace core { namespace meta {
    inline constexpr struct final {
        template<typename Tag, typename... Args>
        static constexpr auto operator()(Tag tag, Args&&... args) noexcept
          -> decltype(tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...)) {
            using details::tag_invoke;

            return tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
        }
    } tag_invoke_cpo;

    template<typename Tag, typename... Args>
    using tag_invoke_result = decltype(tag_invoke(std::declval<Tag>(), std::declval<Args>()...));

    template<typename Customisation_point, typename... Args>
    concept Is_tag_invocable = requires(const Customisation_point& cpo, Args&&... args) {
        tag_invoke(cpo, std::forward<Args>(args)...);
    };
}}} // namespace stormkit::core::meta
