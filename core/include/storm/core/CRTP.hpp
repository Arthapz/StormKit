// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file CRTP.hpp
/// \author Arthapz

#pragma once

/// \module storm::core
namespace storm::core {

    /// \brief storm::core::CRTP is an utility class that simplify the use of
    /// CRTP \tparam T the data type holded
    template<typename T, template<typename> class crtp_type>
    struct CRTP {
        /// \output_section Public Member Functions
        /// \brief return a reference to the underlying value
        /// \returns a reference to the underlying value
        constexpr inline T &underlying() noexcept { return static_cast<T &>(*this); }

        /// \output_section Public Member Functions
        /// \brief return a reference to the underlying value
        /// \returns a const reference to the underlying value
        constexpr inline T const &underlying() const noexcept {
            return static_cast<T const &>(*this);
        }
    };
} // namespace storm::core
