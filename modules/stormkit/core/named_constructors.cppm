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
        namespace meta {
            template<typename NewT, IsStdExpected OldExpectedType>
            using TransformExpectedValueTo = std::expected<NewT, typename OldExpectedType::error_type>;
        } // namespace meta
    }

    struct PrivateTagBase {
        struct Tag {
          private:
            Tag() = default;
            friend struct PrivateTagBase;
        };

      private:
        static constexpr auto PRIVATE = Tag {};

        template<typename...>
        friend class NamedConstructor;
    };

    export {
        using PrivateTag = PrivateTagBase::Tag;

        template<typename...>
        struct ConstructorArgs {};

        template<typename...>
        struct DoInitArgs {};

        template<typename...>
        class NamedConstructor {
          protected:
            constexpr ~NamedConstructor() noexcept;

            constexpr NamedConstructor(const NamedConstructor&) noexcept;
            constexpr auto operator=(const NamedConstructor&) noexcept -> NamedConstructor&;

            constexpr NamedConstructor(NamedConstructor&&) noexcept;
            constexpr auto operator=(NamedConstructor&&) noexcept -> NamedConstructor&;

            static constexpr auto PRIVATE = PrivateTagBase::PRIVATE;

            constexpr NamedConstructor() noexcept;
        };

        template<typename T, typename... TConstructorArgs>
        class NamedConstructor<T, ConstructorArgs<TConstructorArgs...>>: public NamedConstructor<> {
            using ValueType = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;

            [[nodiscard]]
            static constexpr auto create(TConstructorArgs... c_args) noexcept -> ValueType;

            [[nodiscard]]
            static constexpr auto allocate(TConstructorArgs... c_args) noexcept -> Heap<ValueType>;
        };

        template<typename T>
        class NamedConstructor<T>: NamedConstructor<T, ConstructorArgs<>> {
          public:
            using NamedConstructor<T, ConstructorArgs<>>::NamedConstructor;
            using NamedConstructor<T, ConstructorArgs<>>::operator=;
        };

        template<typename T, typename... TConstructorArgs, typename... TDoInitArgs>
        class NamedConstructor<T, ConstructorArgs<TConstructorArgs...>, DoInitArgs<TDoInitArgs...>>: public NamedConstructor<> {
            using ValueType = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;

            [[nodiscard]]
            static constexpr auto create(TConstructorArgs... c_args, TDoInitArgs... d_args) noexcept -> decltype(auto);

            [[nodiscard]]
            static constexpr auto allocate(TConstructorArgs... c_args, TDoInitArgs... d_args) noexcept -> decltype(auto);
        };

        template<typename T, typename... TDoInitArgs>
        class NamedConstructor<T, DoInitArgs<TDoInitArgs...>>: public NamedConstructor<> {
            using ValueType = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;
            [[nodiscard]]
            static constexpr auto create(TDoInitArgs... args) noexcept -> decltype(auto);

            [[nodiscard]]
            static constexpr auto allocate(TDoInitArgs... args) noexcept -> decltype(auto);
        };
    }
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Args...>::NamedConstructor() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Args...>::~NamedConstructor() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<Args...>::operator=(const NamedConstructor&) noexcept -> NamedConstructor& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Args...>::NamedConstructor(const NamedConstructor&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<Args...>::operator=(NamedConstructor&&) noexcept -> NamedConstructor& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Args>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Args...>::NamedConstructor(NamedConstructor&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<T, ConstructorArgs<TConstructorArgs...>>::create(TConstructorArgs... args) noexcept
      -> ValueType {
        return ValueType { PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorArgs>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<T, ConstructorArgs<TConstructorArgs...>>::allocate(TConstructorArgs... args) noexcept
      -> Heap<ValueType> {
        return core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorArgs, typename... TDoInitArgs>
    constexpr auto NamedConstructor<T, ConstructorArgs<TConstructorArgs...>, DoInitArgs<TDoInitArgs...>>::
      create(TConstructorArgs... c_args, TDoInitArgs... d_args) noexcept -> decltype(auto) {
        using DoInitReturnType = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitArgs>()...));
        if constexpr (not meta::IsStdExpected<DoInitReturnType>) {
            auto out = ValueType { PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)... };
            out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...);
            return out;
        } else {
            using ReturnType = meta::TransformExpectedValueTo<ValueType, DoInitReturnType>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = ValueType { PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)... };
            if (auto result = out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...); not result)
                return ReturnType { std::unexpect, std::move(result).error() };

            return ReturnType { std::in_place, std::move(out) };
#else
            auto out_expected = ReturnType { std::in_place, PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)... };
            if (auto result = out_expected.value().do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorArgs, typename... TDoInitArgs>
    constexpr auto NamedConstructor<T, ConstructorArgs<TConstructorArgs...>, DoInitArgs<TDoInitArgs...>>::
      allocate(TConstructorArgs... c_args, TDoInitArgs... d_args) noexcept -> decltype(auto) {
        using DoInitReturnType = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitArgs>()...));
        if constexpr (not meta::IsStdExpected<DoInitReturnType>) {
            auto out = core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)...);
            out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...);
            return out;
        } else {
            using ReturnType = meta::TransformExpectedValueTo<Heap<ValueType>, DoInitReturnType>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)...);
            if (auto result = out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...); not result)
                return ReturnType { std::unexpect, std::move(result).error() };

            return ReturnType { std::in_place, std::move(out) };
#else
            auto out_expected = ReturnType {
                std::in_place,
                core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE, std::forward<TConstructorArgs>(c_args)...)
            };
            if (auto result = out_expected.value()->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(d_args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TDoInitArgs>
    constexpr auto NamedConstructor<T, DoInitArgs<TDoInitArgs...>>::create(TDoInitArgs... args) noexcept -> decltype(auto) {
        using DoInitReturnType = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitArgs>()...));
        if constexpr (not meta::IsStdExpected<DoInitReturnType>) {
            auto out = ValueType { PrivateTagBase::PRIVATE };
            out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...);
            return out;
        } else {
            using ReturnType = meta::TransformExpectedValueTo<ValueType, DoInitReturnType>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = ValueType { PrivateTagBase::PRIVATE };
            if (auto result = out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...); not result)
                return ReturnType { std::unexpect, std::move(result).error() };

            return ReturnType { std::in_place, std::move(out) };
#else
            auto out_expected = ReturnType { std::in_place, PrivateTagBase::PRIVATE };
            if (auto result = out_expected.value().do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TDoInitArgs>
    constexpr auto NamedConstructor<T, DoInitArgs<TDoInitArgs...>>::allocate(TDoInitArgs... args) noexcept -> decltype(auto) {
        using DoInitReturnType = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitArgs>()...));
        if constexpr (not meta::IsStdExpected<DoInitReturnType>) {
            auto out = core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE);
            out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...);
            return out;
        } else {
            using ReturnType = meta::TransformExpectedValueTo<Heap<ValueType>, DoInitReturnType>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE);
            if (auto result = out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...); not result)
                return ReturnType { std::unexpect, std::move(result).error() };

            return ReturnType { std::in_place, std::move(out) };
#else
            auto out_expected = ReturnType { std::in_place, core::allocate_unsafe<ValueType>(PrivateTagBase::PRIVATE...) };
            if (auto result = out_expected.value()->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitArgs>(args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }
}} // namespace stormkit::core
