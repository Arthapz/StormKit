// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_PIMPLIMPL_MACRO_HPP
#define STORMKIT_PIMPLIMPL_MACRO_HPP

#define PIMPL_IMPLEMENTATION                                                       \
    namespace stormkit { inline namespace core {                                   \
            template<class T, bool Defer>                                          \
            pimpl<T, Defer>::pimpl() noexcept(not Defer) {                         \
                if constexpr (Defer) init();                                       \
            }                                                                      \
            template<class T, bool Defer>                                          \
            pimpl<T, Defer>::~pimpl() = default;                                   \
            template<class T, bool Defer>                                          \
            template<meta::IsNot<T> First, class... Ts>                          \
            pimpl<T, Defer>::pimpl(First&& first, Ts&&... args) {                \
                init(std::forward<First>(first), std::forward<Ts>(args)...);     \
            }                                                                      \
            template<class T, bool Defer>                                          \
            pimpl<T, Defer>::pimpl(pimpl&&) noexcept = default;                    \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::operator=(pimpl&&) noexcept -> pimpl& = default; \
            template<class T, bool Defer>                                          \
            template<class... Ts>                                                \
            auto pimpl<T, Defer>::init(Ts&&... args) -> void {                   \
                m_data = stormkit::allocate<T>(std::forward<Ts>(args)...)        \
                           .transform_error(stormkit::core :.monadic::assert())    \
                           .value();                                               \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::operator->() noexcept -> T* {                    \
                return &get();                                                     \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::operator->() const noexcept -> const T* {        \
                return &get();                                                     \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::operator*() noexcept -> T& {                     \
                return get();                                                      \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::operator*() const noexcept -> const T& {         \
                return get();                                                      \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::get() noexcept -> T& {                           \
                expects(operator bool());                                          \
                return *m_data;                                                    \
            }                                                                      \
            template<class T, bool Defer>                                          \
            auto pimpl<T, Defer>::get() const noexcept -> const T& {               \
                expects(operator bool());                                          \
                return *m_data;                                                    \
            }                                                                      \
            template<class T, bool Defer>                                          \
            pimpl<T, Defer>::operator bool() const noexcept {                      \
                return m_data != nullptr;                                          \
            }                                                                      \
    }}

#endif
