// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file NonCopyable.hpp
/// \author Arthapz

#pragma once

#include <storm/core/Platform.hpp>

/// \module storm::core
namespace storm::core {
    /// \brief storm::core::NonCopyable<T> is a utility class to declare non
    /// copyable type
    ///
    /// This class ensure that the child class will be non copyable
    class STORM_PUBLIC NonCopyable {
      public:
        /// \output_section Public Special Member Functions
        /// \brief Default constructor
        constexpr NonCopyable() noexcept = default;

        /// \brief Destructor
        ~NonCopyable() noexcept = default;

        /// \brief Move constructor
        NonCopyable(NonCopyable &&) noexcept = default;

        /// \brief Move assignement
        NonCopyable &operator=(NonCopyable &&) noexcept = default;

        ///\exclude
        NonCopyable(const NonCopyable &) noexcept = delete;

        ///\exclude
        void operator=(const NonCopyable &) noexcept = delete;
    };
} // namespace storm::core
