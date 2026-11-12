// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/contract_macro.hpp>

export module stormkit.core.containers.sequence_view;

import std;

import stormkit.core.types;
import stormkit.core.typesafe;
import stormkit.core.function_ref;
import stormkit.core.meta;
import stormkit.core.contract;

namespace stdr = std::ranges;

export namespace stormkit { inline namespace core {
    template<typename T>
    class sequence_view_iterator final {
      public:
        using iterator_concept  = std::bidirectional_iterator_tag;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = ioffset;

        using element_type = T;
        using value_type   = meta::remove_cv_of<T>;

        using reference = T&;
        using pointer   = T*;

        constexpr sequence_view_iterator() noexcept;

        constexpr operator sequence_view_iterator<const T>() const noexcept;

        [[nodiscard]]
        constexpr auto operator*() const noexcept -> element_type;

        constexpr auto operator++() noexcept -> sequence_view_iterator&;
        constexpr auto operator++(int) noexcept -> sequence_view_iterator;

        constexpr auto operator--() noexcept -> sequence_view_iterator&;
        constexpr auto operator--(int) noexcept -> sequence_view_iterator;

        constexpr auto operator+=(difference_type n) noexcept -> sequence_view_iterator&;
        [[nodiscard]]
        constexpr auto operator+(difference_type n) const noexcept -> sequence_view_iterator;

        constexpr auto operator-=(difference_type n) noexcept -> sequence_view_iterator&;
        [[nodiscard]]
        constexpr auto operator-(difference_type n) const noexcept -> sequence_view_iterator;

        template<typename T2>
        [[nodiscard]]
        constexpr auto operator-(const sequence_view_iterator<T2>& rhs) const noexcept -> difference_type
            requires(meta::plain::is<T, T2>);

        [[nodiscard]]
        constexpr auto operator[](difference_type n) const noexcept -> reference;

        template<typename T2>
        [[nodiscard]]
        constexpr auto operator==(const sequence_view_iterator<T2>& rhs) const noexcept -> bool;
        template<typename T2>
        [[nodiscard]]
        constexpr auto operator<(const sequence_view_iterator<T2>& rhs) const noexcept -> bool;
        template<typename T2>
        [[nodiscard]]
        constexpr auto operator<=(const sequence_view_iterator<T2>& rhs) const noexcept -> bool;
        template<typename T2>
        [[nodiscard]]
        constexpr auto operator>(const sequence_view_iterator<T2>& rhs) const noexcept -> bool;
        template<typename T2>
        [[nodiscard]]
        constexpr auto operator>=(const sequence_view_iterator<T2>& rhs) const noexcept -> bool;
        template<typename T2>
        [[nodiscard]]
        constexpr auto operator<=>(const sequence_view_iterator<T2>& rhs) const noexcept -> std::strong_ordering;

      private:
        using to_element_type_fun = std23::function_ref<element_type(byte*)>;
        constexpr sequence_view_iterator(byte*, byte*, byte*, difference_type, to_element_type_fun) noexcept;

        byte* begin_   = nullptr;
        byte* current_ = nullptr;
        byte* end_     = nullptr;

        difference_type stride_ = 0;

        to_element_type_fun to_element_type_;

        template<typename>
        friend class sequence_view;
    };

    static_assert(std::bidirectional_iterator<sequence_view_iterator<int>>);

    template<typename T>
    class sequence_view final {
      public:
        using element_type    = T;
        using value_type      = meta::remove_cv_of<T>;
        using size_type       = usize;
        using difference_type = ioffset;

        using pointer       = T*;
        using const_pointer = const T*;

        using reference       = T&;
        using const_reference = const T&;

        using iterator       = sequence_view_iterator<T>;
        using const_iterator = std::const_iterator<iterator>;

        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr sequence_view() noexcept;

        template<stdr::contiguous_range R>
        constexpr sequence_view(R& range) noexcept
            requires(meta::convertible_to<meta::value_type<R>, element_type>);

        auto begin() const noexcept -> iterator;
        auto cbegin() const noexcept -> const_iterator;

        auto end() const noexcept -> iterator;
        auto cend() const noexcept -> const_iterator;

        auto rbegin() const noexcept -> iterator;
        auto crbegin() const noexcept -> const_iterator;

        auto rend() const noexcept -> iterator;
        auto crend() const noexcept -> const_iterator;

        auto front() const noexcept -> element_type;
        auto back() const noexcept -> element_type;
        auto at(size_type i) const noexcept -> element_type;
        auto operator[](size_type i) const noexcept -> element_type;
        auto data() const noexcept -> byte*;

        auto size() const noexcept -> size_type;
        auto size_bytes() const noexcept -> size_type;
        auto empty() const noexcept -> bool;

        // todo implement subviews
      private:
        using to_element_type_fun = element_type (*)(byte*);

        byte* range_  = nullptr;
        usize size_   = 0;
        usize stride_ = 0;

        to_element_type_fun to_element_type_;
    };

}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr sequence_view_iterator<T>::sequence_view_iterator() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr sequence_view_iterator<T>::sequence_view_iterator(byte*               begin,
                                                                byte*               current,
                                                                byte*               end,
                                                                difference_type     stride,
                                                                to_element_type_fun to_element_type) noexcept
        : begin_ { begin },
          current_ { current },
          end_ { end },
          stride_ { stride },
          to_element_type_ { std::move(to_element_type) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr sequence_view_iterator<T>::operator sequence_view_iterator<const T>() const noexcept {
        return { begin_, current_, end_, stride_, to_element_type_ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator*() const noexcept -> element_type {
        EXPECTS(current_ != nullptr);
        EXPECTS(current_ != end_);
        return to_element_type_(current_);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator++() noexcept -> sequence_view_iterator& {
        EXPECTS(current_ != nullptr);
        EXPECTS(current_ != end_);

        current_ += stride_;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator++(int) noexcept -> sequence_view_iterator {
        auto ret = auto(*this);
        ++(*this);
        return ret;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator--() noexcept -> sequence_view_iterator& {
        EXPECTS(current_ != nullptr);
        EXPECTS(current_ != end_);

        current_ -= stride_;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator--(int) noexcept -> sequence_view_iterator {
        auto ret = auto(*this);
        --(*this);
        return ret;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator+=(difference_type n) noexcept -> sequence_view_iterator& {
        const auto n2 = n * stride_;

        if (n != 0) EXPECTS(begin_ != nullptr and current_ != nullptr and end_ != nullptr);

        if (n > 0) EXPECTS(end_ - current_ >= n2);
        else if (n < 0)
            EXPECTS(current_ - begin_ >= -n2);

        current_ += n2;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator+(difference_type n) const noexcept -> sequence_view_iterator {
        auto ret = auto(*this);
        ret += n;
        return ret;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator-=(difference_type n) noexcept -> sequence_view_iterator& {
        const auto n2 = n * stride_;

        if (n != 0) EXPECTS(begin_ != nullptr and current_ != nullptr and end_ != nullptr);

        if (n > 0) EXPECTS(current_ - begin_ >= n2);
        else if (n < 0)
            EXPECTS(end_ - current_ >= -n2);

        current_ -= n2;
        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator-(difference_type n) const noexcept -> sequence_view_iterator {
        auto ret = auto(*this);
        ret -= n;
        return ret;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator-(const sequence_view_iterator<T2>& rhs) const noexcept -> difference_type
        requires(meta::plain::is<T, T2>)
    {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ - rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr auto sequence_view_iterator<T>::operator[](difference_type n) const noexcept -> reference {
        return *(*this + n);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator==(const sequence_view_iterator<T2>& rhs) const noexcept -> bool {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ == rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator<(const sequence_view_iterator<T2>& rhs) const noexcept -> bool {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ < rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator<=(const sequence_view_iterator<T2>& rhs) const noexcept -> bool {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ <= rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator>(const sequence_view_iterator<T2>& rhs) const noexcept -> bool {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ > rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator>=(const sequence_view_iterator<T2>& rhs) const noexcept -> bool {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ >= rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<typename T2>
    constexpr auto sequence_view_iterator<T>::operator<=>(const sequence_view_iterator<T2>& rhs) const noexcept
      -> std::strong_ordering {
        EXPECTS(begin_ == rhs.begin_ and end_ == rhs.end_);
        return current_ <=> rhs.current_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    constexpr sequence_view<T>::sequence_view() noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    template<stdr::contiguous_range R>
    constexpr sequence_view<T>::sequence_view(R& range) noexcept
        requires(meta::convertible_to<meta::value_type<R>, element_type>)
        : range_ { const_cast<byte*>(static_cast<const byte*>(stdr::data(range))) },
          size_ { stdr::size(range) },
          stride_ { sizeof(meta::value_type<R>) },
          to_element_type_ { [](byte* at) static noexcept { return element_type { *std::bit_cast<pointer>(at) }; } } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::begin() const noexcept -> iterator {
        EXPECTS(range_ != nullptr);
        return { range_, range_, range_ + size_, stride_, to_element_type_ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::cbegin() const noexcept -> const_iterator {
        EXPECTS(range_ != nullptr);
        return { range_, range_, range_ + size_, stride_, to_element_type_ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::end() const noexcept -> iterator {
        EXPECTS(range_ != nullptr);
        return { range_, range_ + size_, range_ + size_, stride_, to_element_type_ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::cend() const noexcept -> const_iterator {
        EXPECTS(range_ != nullptr);
        return { range_, range_ + size_, range_ + size_, stride_, to_element_type_ };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::rbegin() const noexcept -> iterator {
        return reverse_iterator { begin() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::crbegin() const noexcept -> const_iterator {
        return reverse_iterator { cbegin() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::rend() const noexcept -> iterator {
        return reverse_iterator { end() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::crend() const noexcept -> const_iterator {
        return reverse_iterator { cend() };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::front() const noexcept -> element_type {
        return to_element_type_(range_);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::back() const noexcept -> element_type {
        EXPECTS(range_ != nullptr);
        return to_element_type_(range_ + size_);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::at(size_type i) const noexcept -> element_type {
        EXPECTS(range_ != nullptr);
        return to_element_type_(range_ + (i * stride_));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::operator[](size_type i) const noexcept -> element_type {
        EXPECTS(range_ != nullptr);
        return to_element_type_(range_ + (i * stride_));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::data() const noexcept -> byte* {
        EXPECTS(range_ != nullptr);
        return range_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::size() const noexcept -> size_type {
        EXPECTS(range_ != nullptr);
        return size_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::size_bytes() const noexcept -> size_type {
        EXPECTS(range_ != nullptr);
        return size_ * stride_;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto sequence_view<T>::empty() const noexcept -> bool {
        EXPECTS(range_ != nullptr);
        return size_ == 0;
    }
}} // namespace stormkit::core
