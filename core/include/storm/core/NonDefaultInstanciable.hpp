// Copryright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/// \file NonDefaultInstanciable.hpp
/// \author Arthapz

#pragma once

#include <storm/core/Memory.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/NonMovable.hpp>

/// \module storm::core
namespace storm::core {
    /// \brief storm::core::NonDefaultInstanciable<T> is a utility class to
    /// declare non default instanciable type
    ///
    /// This class ensure that the child class will be non default instanciable
    class STORM_EBCO STORM_PUBLIC NonDefaultInstanciable: public NonCopyable, public NonMovable {
      public:
        /// \exclude
        NonDefaultInstanciable() = delete;

        /// \output_section Public Special Member Functions
        /// \brief Destructor
        ~NonDefaultInstanciable() noexcept = default;
    };
} // namespace storm::core
