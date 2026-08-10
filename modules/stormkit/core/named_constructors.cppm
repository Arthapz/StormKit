// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>
#include <stormkit/core/try_expected.hpp>

export module stormkit.core.named_constructors;

import std;

import stormkit.core.meta;
import stormkit.core.heap;

namespace stormkit { inline namespace core {
    export {
        namespace meta {
            template<typename NewT, std_expected Oldreturn_type>
            using TransformExpectedValueTo = std::expected<NewT, typename Oldreturn_type::error_type>;
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
        struct ConstructorTs {};

        template<typename...>
        struct DoInitTs {};

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

        template<typename T, typename... TConstructorTs>
        class NamedConstructor<T, ConstructorTs<TConstructorTs...>>: public NamedConstructor<> {
            using value_type = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;

            [[nodiscard]]
            static constexpr auto create(TConstructorTs... c_args) noexcept -> value_type;

            [[nodiscard]]
            static constexpr auto allocate(TConstructorTs... c_args) noexcept -> heap_ptr<value_type>;
        };

        template<typename T>
        class NamedConstructor<T>: NamedConstructor<T, ConstructorTs<>> {
          public:
            using NamedConstructor<T, ConstructorTs<>>::NamedConstructor;
            using NamedConstructor<T, ConstructorTs<>>::operator=;
        };

        template<typename T, typename... TConstructorTs, typename... TDoInitTs>
        class NamedConstructor<T, ConstructorTs<TConstructorTs...>, DoInitTs<TDoInitTs...>>: public NamedConstructor<> {
            using value_type = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;

            [[nodiscard]]
            static constexpr auto create(TConstructorTs... c_args, TDoInitTs... d_args) noexcept -> decltype(auto);

            [[nodiscard]]
            static constexpr auto allocate(TConstructorTs... c_args, TDoInitTs... d_args) noexcept -> decltype(auto);
        };

        template<typename T, typename... TDoInitTs>
        class NamedConstructor<T, DoInitTs<TDoInitTs...>>: public NamedConstructor<> {
            using value_type = T;

          public:
            using NamedConstructor<>::NamedConstructor;
            using NamedConstructor<>::operator=;
            [[nodiscard]]
            static constexpr auto create(TDoInitTs... args) noexcept -> decltype(auto);

            [[nodiscard]]
            static constexpr auto allocate(TDoInitTs... args) noexcept -> decltype(auto);
        };
    }
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Ts...>::NamedConstructor() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Ts...>::~NamedConstructor() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<Ts...>::operator=(const NamedConstructor&) noexcept -> NamedConstructor& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Ts...>::NamedConstructor(const NamedConstructor&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<Ts...>::operator=(NamedConstructor&&) noexcept -> NamedConstructor& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename... Ts>
    STORMKIT_FORCE_INLINE
    constexpr NamedConstructor<Ts...>::NamedConstructor(NamedConstructor&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorTs>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<T, ConstructorTs<TConstructorTs...>>::create(TConstructorTs... args) noexcept
      -> value_type {
        return value_type { PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(args)... };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorTs>
    STORMKIT_FORCE_INLINE
    constexpr auto NamedConstructor<T, ConstructorTs<TConstructorTs...>>::allocate(TConstructorTs... args) noexcept
      -> heap_ptr<value_type> {
        return core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(args)...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorTs, typename... TDoInitTs>
    constexpr auto NamedConstructor<T, ConstructorTs<TConstructorTs...>, DoInitTs<TDoInitTs...>>::
      create(TConstructorTs... c_args, TDoInitTs... d_args) noexcept -> decltype(auto) {
        using DoInitreturn_type = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitTs>()...));
        if constexpr (not meta::std_expected<DoInitreturn_type>) {
            auto out = value_type { PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)... };
            out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...);
            return out;
        } else {
            using return_type = meta::TransformExpectedValueTo<value_type, DoInitreturn_type>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = value_type { PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)... };
            if (auto result = out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...); not result)
                return return_type { std::unexpect, std::move(result).error() };

            return return_type { std::in_place, std::move(out) };
#else
            auto out_expected = return_type { std::in_place, PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)... };
            if (auto result = out_expected.value().do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TConstructorTs, typename... TDoInitTs>
    constexpr auto NamedConstructor<T, ConstructorTs<TConstructorTs...>, DoInitTs<TDoInitTs...>>::
      allocate(TConstructorTs... c_args, TDoInitTs... d_args) noexcept -> decltype(auto) {
        using DoInitreturn_type = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitTs>()...));
        if constexpr (not meta::std_expected<DoInitreturn_type>) {
            auto out = core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)...);
            out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...);
            return out;
        } else {
            using return_type = meta::TransformExpectedValueTo<heap_ptr<value_type>, DoInitreturn_type>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)...);
            if (auto result = out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...); not result)
                return return_type { std::unexpect, std::move(result).error() };

            return return_type { std::in_place, std::move(out) };
#else
            auto out_expected = return_type {
                std::in_place,
                core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE, std::forward<TConstructorTs>(c_args)...)
            };
            if (auto result = out_expected.value()->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(d_args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TDoInitTs>
    constexpr auto NamedConstructor<T, DoInitTs<TDoInitTs...>>::create(TDoInitTs... args) noexcept -> decltype(auto) {
        using DoInitreturn_type = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitTs>()...));
        if constexpr (not meta::std_expected<DoInitreturn_type>) {
            auto out = value_type { PrivateTagBase::PRIVATE };
            out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...);
            return out;
        } else {
            using return_type = meta::TransformExpectedValueTo<value_type, DoInitreturn_type>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = value_type { PrivateTagBase::PRIVATE };
            if (auto result = out.do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...); not result)
                return return_type { std::unexpect, std::move(result).error() };

            return return_type { std::in_place, std::move(out) };
#else
            auto out_expected = return_type { std::in_place, PrivateTagBase::PRIVATE };
            if (auto result = out_expected.value().do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T, typename... TDoInitTs>
    constexpr auto NamedConstructor<T, DoInitTs<TDoInitTs...>>::allocate(TDoInitTs... args) noexcept -> decltype(auto) {
        using DoInitreturn_type = decltype(std::declval<T>().do_init(PRIVATE, std::declval<TDoInitTs>()...));
        if constexpr (not meta::std_expected<DoInitreturn_type>) {
            auto out = core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE);
            out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...);
            return out;
        } else {
            using return_type = meta::TransformExpectedValueTo<heap_ptr<value_type>, DoInitreturn_type>;

#ifdef STORMKIT_COMPILER_CLANG
            auto out = core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE);
            if (auto result = out->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...); not result)
                return return_type { std::unexpect, std::move(result).error() };

            return return_type { std::in_place, std::move(out) };
#else
            auto out_expected = return_type { std::in_place, core::allocate_unsafe<value_type>(PrivateTagBase::PRIVATE...) };
            if (auto result = out_expected.value()->do_init(PrivateTagBase::PRIVATE, std::forward<TDoInitTs>(args)...);
                not result)
                out_expected = std::unexpected { std::move(result).error() };

            return out_expected;
#endif
        }
    }
}} // namespace stormkit::core
