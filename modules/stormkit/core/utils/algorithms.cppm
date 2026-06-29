// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core:utils.algorithms;

import std;

import :meta;
import :containers.aliases;

namespace stdr = std::ranges;
namespace stdv = std::views;

export namespace stormkit { inline namespace core {
    template<stdr::input_range Range, meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type> Predicate>
    [[nodiscard]]
    constexpr auto copy_if(Range&& input, Predicate&& predicate) noexcept -> decltype(auto);

    template<stdr::input_range Range, std::invocable<const typename meta::CanonicalT<Range>::value_type&> Lambda>
    [[nodiscard]]
    constexpr auto transform(Range&& input, Lambda&& lambda) noexcept -> decltype(auto);

    template<stdr::input_range                                                       Range,
             meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type> Predicate,
             std::invocable<const typename meta::CanonicalT<Range>::value_type&>  Lambda>
    [[nodiscard]]
    constexpr auto transform_if(Range&& input, Predicate&& predicate, Lambda&& lambda) noexcept -> decltype(auto);

    template<stdr::input_range                                                                                          Range,
             meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type>                                    Predicate,
             std::invocable<const typename meta::CanonicalT<Range>::value_type&>                                     Lambda,
             std::output_iterator<std::invoke_result_t<Lambda, const typename meta::CanonicalT<Range>::value_type&>> Iterator>
    constexpr auto transform_if(Range&& input, Iterator&& it, Predicate&& predicate, Lambda&& lambda) noexcept -> void;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range Range, meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type> Predicate>
    constexpr auto copy_if(Range&& input, Predicate&& predicate) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::filter(std::forward<Predicate>(predicate))
               | stdr::to<dynarray<typename Range::value_type>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range Range, std::invocable<const typename meta::CanonicalT<Range>::value_type&> Lambda>
    constexpr auto transform(Range&& input, Lambda&& lambda) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::transform(lambda)
               | stdr::to<dynarray<std::invoke_result_t<Lambda, const typename meta::CanonicalT<Range>::value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range                                                       Range,
             meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type> Predicate,
             std::invocable<const typename meta::CanonicalT<Range>::value_type&>  Lambda>
    constexpr auto transform_if(Range&& input, Predicate&& predicate, Lambda&& lambda) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::filter(std::forward<Predicate>(predicate))
               | stdv::transform(std::forward<Lambda>(lambda))
               | stdr::to<dynarray<std::invoke_result_t<Lambda, const typename meta::CanonicalT<Range>::value_type>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range                                                                                          Range,
             meta::IsUnaryPredicate<typename meta::CanonicalT<Range>::value_type>                                    Predicate,
             std::invocable<const typename meta::CanonicalT<Range>::value_type&>                                     Lambda,
             std::output_iterator<std::invoke_result_t<Lambda, const typename meta::CanonicalT<Range>::value_type&>> Iterator>
    constexpr auto transform_if(Range&& input, Iterator&& it, Predicate&& predicate, Lambda&& lambda) noexcept -> void {
        stdr::for_each(std::forward<Range>(input), [&it, &predicate, &lambda](auto&& elem) {
            if (predicate(elem)) *it++ = lambda(elem);
        });
    }
}} // namespace stormkit::core
