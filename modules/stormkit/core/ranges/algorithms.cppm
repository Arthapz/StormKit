// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

export module stormkit.core.ranges.algorithms;

import std;

import stormkit.core.meta;
import stormkit.core.containers;
import stormkit.core.types;

namespace stdr = std::ranges;
namespace stdv = std::views;

export namespace stormkit { inline namespace core {
    namespace meta {
        template<typename T>
        concept range_predicate = meta::unary_predicate<meta::range_type<T>>;

        namespace plain {
            template<typename T>
            concept range_predicate = meta::plain::apply_to<range_predicate, T>;
        }

        template<typename T, typename U>
        concept range_unary_op = meta::regular_invocable<T, meta::range_type_ref<U>>;

        namespace plain {
            template<typename T, typename U>
            concept range_unary_op = meta::plain::apply_to<range_unary_op, T, U>;
        }
    } // namespace meta

    template<stdr::input_range Range, meta::plain::range_predicate Predicate>
    [[nodiscard]]
    constexpr auto copy_if(Range&& input, Predicate&& predicate) noexcept -> decltype(auto);

    template<stdr::input_range Range, meta::plain::range_unary_op Operation>
    [[nodiscard]]
    constexpr auto transform(Range&& input, Operation&& op) noexcept -> decltype(auto);

    template<stdr::input_range Range, meta::plain::range_predicate Predicate, meta::plain::range_unary_op Operation>
    [[nodiscard]]
    constexpr auto transform_if(Range&& input, Predicate&& predicate, Operation&& op) noexcept -> decltype(auto);

    template<stdr::input_range                                                                         Range,
             meta::plain::range_predicate                                                              Predicate,
             meta::plain::range_unary_op                                                               Operation,
             std::output_iterator<std::invoke_result_t<Operation, meta::plain::range_type_ref<Range>>> Iterator>
    constexpr auto transform_if(Range&& input, Iterator&& it, Predicate&& predicate, Operation&& op) noexcept -> void;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range Range, meta::plain::range_predicate Predicate>
    constexpr auto copy_if(Range&& input, Predicate&& predicate) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::filter(std::forward<Predicate>(predicate))
               | stdr::to<dynarray<meta::range_type<Range>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range Range, meta::plain::range_unary_op Operation>
    constexpr auto transform(Range&& input, Operation&& op) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::transform(lambda)
               | stdr::to<dynarray<std::invoke_result_t<Operation, meta::range_type<meta::to_plain_type<Range>>>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range Range, meta::plain::range_predicate Predicate, meta::plain::range_unary_op Operation>
    constexpr auto transform_if(Range&& input, Predicate&& predicate, Operation&& op) noexcept -> decltype(auto) {
        return std::forward<Range>(input)
               | stdv::filter(std::forward<Predicate>(predicate))
               | stdv::transform(std::forward<Operation>(op))
               | stdr::to<dynarray<std::invoke_result_t<Operation, meta::range_type<meta::to_plain_type<Range>>>>>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<stdr::input_range                                                                                       Range,
             meta::plain::range_predicate                                                                            Predicate,
             meta::plain::range_unary_op                                                                             Operation,
             std::output_iterator<std::invoke_result_t<Operation, meta::range_type_ref<meta::to_plain_type<Range>>>> Iterator>
    constexpr auto transform_if(Range&& input, Iterator&& it, Predicate&& predicate, Operation&& op) noexcept -> void {
        stdr::for_each(std::forward<Range>(input), [&it, &predicate, &lambda](auto&& elem) {
            if (predicate(elem)) *it++ = lambda(elem);
        });
    }
}} // namespace stormkit::core
