// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <gsl/gsl_util>

#include <storm/core/Assert.hpp>
#include <storm/core/Memory.hpp>
#include <storm/core/Span.hpp>

namespace storm::core {
    template <typename T> class RingBuffer {
	  public:
		using value_type = T;
        using size_type  = std::size_t;

        RingBuffer(size_type capacity) : m_capacity{capacity} {
            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type>{getPtr(0), gsl::narrow_cast<typename core::span<const value_type>::index_type>(m_capacity)};
        }

        RingBuffer(const RingBuffer &copy) {
			m_head = copy.m_head;
			m_tail = copy.m_tail;

            m_full = copy.m_full;

            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type>{getPtr(0), gsl::narrow_cast<typename core::span<const value_type>::index_type>(m_capacity)};
            if (!empty())
                for (auto i = m_tail; i < m_head;) {
                    new (&m_buffer[i * sizeof(value_type)]) T{*copy.getPtr(i)};

                    i += 1;
                    if (i >= m_capacity)
                        i -= m_capacity;
                }
		}

        RingBuffer &operator=(const RingBuffer &copy) {
			if (&copy == this)
				return *this;

			m_head = copy.m_head;
			m_tail = copy.m_tail;

			m_full = copy.m_full;

            m_buffer.resize(m_capacity * sizeof(value_type));
            m_view = core::span<const value_type>{getPtr(0), gsl::narrow_cast<typename core::span<const value_type>::index_type>(m_capacity)};
            if (!empty())
                for (auto i = m_tail; i < m_head;) {
                    new (&m_buffer[i * sizeof(value_type)]) T{*copy.getPtr(i)};

                    i += 1;
                    if (i >= m_capacity)
                        i -= m_capacity;
                }

			return *this;
		}

		RingBuffer(RingBuffer &&moved) noexcept {
			m_buffer = std::move(moved.m_buffer);

			m_head = moved.m_head;
			m_tail = moved.m_tail;

			m_full = moved.m_full;

			moved.m_capacity = 0;
			moved.m_head	 = 0;
			moved.m_tail	 = 0;
			moved.m_full	 = false;
		}

		RingBuffer &operator=(RingBuffer &&moved) noexcept {
			if (&moved == this)
				return *this;

			m_buffer = std::move(moved.m_buffer);

			m_head = moved.m_head;
			m_tail = moved.m_tail;

			m_full = moved.m_full;

			moved.m_capacity = 0;
			moved.m_head	 = 0;
			moved.m_tail	 = 0;
			moved.m_full	 = false;

			return *this;
		}

        ~RingBuffer() {
            clear();
        }

        inline void clear() noexcept {
            while (!empty()) pop();
		}
        inline bool empty() const noexcept {
			return !m_full && (m_head == m_tail);
		}
        inline bool full() const noexcept {
			return m_full;
		}
        inline size_type size() noexcept {
            if (m_full)
                return m_capacity;

            if (m_head >= m_tail)
                return m_head - m_tail;

            return m_capacity + m_head - m_tail;
		}
        inline size_type capacity() noexcept {
            return m_capacity;
		}

        inline void push(value_type value) {
            emplace(std::move(value));
		}

        template <typename... Args>
        void emplace(Args &&... values) noexcept(
            std::is_nothrow_constructible<value_type, Args &&...>::value) {
            if (m_full)
                pop();

            new (&m_buffer[m_head * sizeof(value_type)])
                value_type{std::forward<Args>(values)...};

            m_head += 1;
            if (m_head >= m_capacity)
                m_head -= m_capacity;

            m_full = (m_head == m_tail);
        }

        void next() noexcept {
            m_tail += 1;
            if (m_tail >= m_capacity)
                m_tail -= m_capacity;

            m_full = (m_head == m_tail);
        }

        void pop() noexcept {
            STORM_EXPECTS(!empty());

            getPtr(m_head)->~value_type();

            m_full = false;
        }

        [[nodiscard]] inline value_type &get() noexcept {
            STORM_EXPECTS(!empty());

            return *getPtr(m_tail);
        }

        [[nodiscard]] inline const value_type &get() const noexcept {
            STORM_EXPECTS(!empty());

            return *getPtr(m_tail);
        }

        [[nodiscard]] inline core::span<const value_type> data() const noexcept {
            return m_view;
        }

      private:
        [[nodiscard]] inline value_type *getPtr(size_type pos) noexcept {
            return std::launder(
                reinterpret_cast<T *>(&m_buffer[pos * sizeof(value_type)]));
        }

        [[nodiscard]] inline const value_type *getPtr(size_type pos) const
            noexcept {
            return std::launder(reinterpret_cast<const T *>(
                &m_buffer[pos * sizeof(value_type)]));
        }

        size_type m_capacity;

        std::vector<std::byte> m_buffer;
        core::span<const value_type> m_view;

        size_type m_head = 0;
        size_type m_tail = 0;

		bool m_full = false;
	};
} // namespace storm::core
