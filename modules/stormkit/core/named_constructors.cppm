// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

export module stormkit.core:named_constructors;

import std;

import :meta;
import :utils.allocation;

namespace stormkit { inline namespace core {
    export {
        struct TemplatedNamedConstructorHelper {
          protected:
            static constexpr struct PrivateTag {
            } PRIVATE;

            template<typename, typename, typename...>
            friend class UseNamedConstructors;
        };

        namespace meta {
            template<typename T>
            concept HasCreateAllocateProtected = requires() {
                { T::CREATE_ALLOCATE_PROTECTED } -> IsBooleanTestable;
            } and T::CREATE_ALLOCATE_PROTECTED;

            template<typename T>
            concept DoInitHasExpectedType = IsStdExpected<ReturnType<decltype(&T::do_init)>>;

            template<typename T>
            using DoInitExpectedType = ReturnType<decltype(&T::do_init)>;

            template<typename NewT, IsStdExpected OldExpectedType>
            using TransformExpectedValueTo = std::expected<NewT, typename OldExpectedType::error_type>;
        } // namespace meta
    }

    export {
        template<typename T,
                 typename DoInitRetType = meta::If<meta::DoInitHasExpectedType<T>, meta::DoInitExpectedType<T>, void>,
                 typename... ConstructorArgs>
        class UseNamedConstructors {
            using ValueType = T;
            using RetType   = DoInitRetType;

          public:
            constexpr ~UseNamedConstructors() noexcept;

            constexpr UseNamedConstructors(const UseNamedConstructors&) noexcept;
            constexpr auto operator=(const UseNamedConstructors&) noexcept -> UseNamedConstructors&;

            constexpr UseNamedConstructors(UseNamedConstructors&&) noexcept;
            constexpr auto operator=(UseNamedConstructors&&) noexcept -> UseNamedConstructors&;

            template<typename... Args>
                requires(meta::IsStdExpected<DoInitRetType>)
            [[nodiscard]]
            static constexpr auto create(ConstructorArgs... c_args, Args&&... args) noexcept
              -> meta::TransformExpectedValueTo<ValueType, RetType>;

            template<typename... Args>
                requires(not meta::IsStdExpected<DoInitRetType>)
            [[nodiscard]]
            static constexpr auto create(ConstructorArgs... c_args, Args&&... args) noexcept -> ValueType;

            template<typename... Args>
                requires(meta::IsStdExpected<DoInitRetType>)
            [[nodiscard]]
            static constexpr auto allocate(ConstructorArgs... c_args, Args&&... args) noexcept
              -> meta::TransformExpectedValueTo<Heap<ValueType>, RetType>;

            template<typename... Args>
                requires(not meta::IsStdExpected<DoInitRetType>)
            [[nodiscard]]
            static constexpr auto allocate(ConstructorArgs... c_args, Args&&... args) noexcept -> Heap<ValueType>;

          protected:
            using PrivateTag              = TemplatedNamedConstructorHelper::PrivateTag;
            static constexpr auto PRIVATE = TemplatedNamedConstructorHelper::PRIVATE;

            constexpr UseNamedConstructors() noexcept;
        };
    }
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::UseNamedConstructors() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::~UseNamedConstructors() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::operator=(const UseNamedConstructors&) noexcept
      -> UseNamedConstructors& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::
      UseNamedConstructors(const UseNamedConstructors&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::operator=(UseNamedConstructors&&) noexcept
      -> UseNamedConstructors& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::
      UseNamedConstructors(UseNamedConstructors&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    template<typename... Args>
        requires(meta::IsStdExpected<DoInitRetType>)
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::create(ConstructorArgs... c_args,
                                                                                      Args&&... args) noexcept
      -> meta::TransformExpectedValueTo<ValueType, RetType> {
        using ReturnValue = meta::TransformExpectedValueTo<ValueType, RetType>;

        auto out_expected = ReturnValue { std::in_place, PRIVATE, c_args... };
        if (auto result = out_expected.value().do_init(PRIVATE, std::forward<Args>(args)...); not result)
            out_expected = ReturnValue { std::unexpect, std::move(result).error() };

        return out_expected;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    template<typename... Args>
        requires(not meta::IsStdExpected<DoInitRetType>)
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::create(ConstructorArgs... c_args,
                                                                                      Args&&... args) noexcept -> ValueType {
        auto out = ValueType { PRIVATE, c_args... };
        out.do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    template<typename... Args>
        requires(meta::IsStdExpected<DoInitRetType>)
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::allocate(ConstructorArgs... c_args,
                                                                                        Args&&... args) noexcept
      -> meta::TransformExpectedValueTo<Heap<ValueType>, RetType> {
        using ReturnValue = meta::TransformExpectedValueTo<Heap<ValueType>, RetType>;

        auto out_expected = ReturnValue { std::in_place, core::allocate_unsafe(PRIVATE, c_args...) };
        if (auto result = out_expected.value().do_init(PRIVATE, std::forward<Args>(args)...); not result)
            out_expected = ReturnValue { std::unexpect, std::move(result).error() };

        return out_expected;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename DoInitRetType, typename... ConstructorArgs>
    template<typename... Args>
        requires(not meta::IsStdExpected<DoInitRetType>)
    STORMKIT_FORCE_INLINE
    constexpr auto UseNamedConstructors<T, DoInitRetType, ConstructorArgs...>::allocate(ConstructorArgs... c_args,
                                                                                        Args&&... args) noexcept
      -> Heap<ValueType> {
        auto out = core::allocate_unsafe<ValueType>(PRIVATE, c_args...);
        out->do_init(PRIVATE, std::forward<Args>(args)...);
        return out;
    }
}} // namespace stormkit::core
